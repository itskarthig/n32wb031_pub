/**
 * @file app_fuota.c
 * @brief FUOTA application layer -- flash write, install flag update, sequencer task.
 *
 * Protocol:
 *   AD22 Base Address -- action codes control the transfer state machine
 *   AD23 Confirmation -- device indicates Ready/Reboot/Error to client
 *   AD24 Raw Data     -- client streams firmware chunks (max 240 B, multiple of 4)
 *
 * Flash layout (fixed):
 *   Download Slot = APP2 at 0x01040000 (240 KB)
 *   App always runs from APP1 at 0x01004000.
 *
 * Deferred execution:
 *   Write handlers store work type + data, then call UTIL_SEQ_SetTask().
 *   APP_FUOTA_WorkTask() runs in sequencer context outside the BLE kernel --
 *   safe to call Qflash_Erase_Sector() and Qflash_Write() here.
 *
 * Install flag (STM32-style single-bank approach):
 *   On FINISH, InstallFlag_t at 0x01002000 is written:
 *     magic = INSTALL_MAGIC, src_addr = APP2, size, crc
 *   P2PS_OTA_INSTALLER detects magic on next reset, copies APP2 -> APP1,
 *   erases the flag, then resets again -> clean boot from APP1. This
 *   project's own install_flag.h (Projects/.../P2PS_OTA/Common/install_flag.h) has
 *   a project-unique INSTALL_MAGIC ("P2_1") -- resolved via bare #include
 *   below, whose search order already places the project root ahead of
 *   common/ in CMakeLists.txt (confirmed, no change needed there).
 *
 * Ported from app_ble_cellular_wmbus's app_fuota.c essentially unchanged --
 * already fully generic. The old file's #include "dfu_crc.h" was dead code
 * (dfu_crc32() is declared but never called -- the CRC32 here is computed
 * entirely via direct CRC-peripheral register access) and has been dropped
 * rather than carried forward.
 */

#include "rwip_config.h"

#if (BLE_APP_FUOTA)

#include "app_fuota.h"
#include <string.h>
#include "fuotas.h"
#include "fuotas_task.h"
#include "attm.h"
#include "ke_task.h"
#include "gapc.h"
#include "gapc_task.h"
#include "gattc_task.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "ke_msg.h"
#include "ns_ble.h"
#include "app_log.h"
#include "n32_seq.h"
#include "n32_lpm.h"
#include "n32wb03x.h"
#include "n32wb03x_qflash.h"
#include "install_flag.h"
#include "app_p2ps.h"

/* OTA magic keyword -- placed last in flash by the .tag_ota_end linker section.
 * FUOTA finish-check: *(FUOTA_DL_ADDR + s_image_size - 4) == 0x94448A29U.
 * `used` prevents --gc-sections from discarding it.                          */
const uint32_t MagicKeywordValue
    __attribute__((section(".tag_ota_end"), used)) = 0x94448A29U;

/* ---- GATT attribute database ---------------------------------------------- */

struct attm_desc_128 fuota_att_db[FUOTA_IDX_NB] =
    {
        /* Primary service declaration */
        [FUOTA_IDX_SVC] = {{0x00, 0x28}, PERM(RD, ENABLE), 0, 0},

        /* Base Address: write-without-response, 5 bytes */
        [FUOTA_IDX_BASE_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},
        [FUOTA_IDX_BASE_VAL] = {CHAR_FUOTA_BASE,
                                PERM(WRITE_COMMAND, ENABLE),
                                PERM_VAL(UUID_LEN, 0x02), 9},

        /* Confirmation: indicate, 1 byte */
        [FUOTA_IDX_CFM_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},
        [FUOTA_IDX_CFM_VAL] = {CHAR_FUOTA_CFM,
                               PERM(IND, ENABLE),
                               PERM(RI, ENABLE) | PERM_VAL(UUID_LEN, 0x02), 1},
        [FUOTA_IDX_CFM_CFG] = {{0x02, 0x29}, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},

        /* Raw Data: write-without-response, 240 bytes max */
        [FUOTA_IDX_DATA_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},
        [FUOTA_IDX_DATA_VAL] = {CHAR_FUOTA_DATA,
                                PERM(WRITE_COMMAND, ENABLE),
                                PERM_VAL(UUID_LEN, 0x02), 240},
};

/* ---- Private state --------------------------------------------------------- */

/* Work type queued for APP_FUOTA_WorkTask() */
#define FUOTA_WORK_NONE 0
#define FUOTA_WORK_START 1  /* erase Download Slot, then indicate Ready     */
#define FUOTA_WORK_FINISH 3 /* verify magic, update install flag, reboot    */
#define FUOTA_WORK_CANCEL 4 /* reset state, indicate Ready                  */

/* Ring buffer for incoming firmware chunks.
 * Qflash_Write(256 B) masks ALL IRQs for 2-3 ms (the Qflash driver sets PRIMASK
 * internally). Draining the full ring per work_task invocation can mask IRQs for
 * up to 4 x 3 ms = 12 ms, during which the BLE controller buffers chunks.
 * 4 slots (3 usable) is sufficient at standard BLE CI rates.
 * Sleep lock (CFG_LPM_FUOTA_Id voter) prevents deep sleep for the full transfer
 * so the sequencer stays responsive between page writes.
 * Each slot is 242 B -> 4x242 + 256 page buf = 1224 B total. */
#define FUOTA_RING_SLOTS 4

typedef struct
{
    uint8_t data[240];
    uint16_t len;
} fuota_chunk_t;
static fuota_chunk_t s_ring[FUOTA_RING_SLOTS];
static uint8_t s_ring_head = 0; /* producer writes here (handler context) */
static uint8_t s_ring_tail = 0; /* consumer reads here  (sequencer context) */

/* Page buffer: Qflash_Write requires 256-byte aligned addresses and writes in
 * 256-byte pages. Chunks (240 B) are fed byte-by-byte into this buffer; a page
 * write fires only when the buffer is full. The last partial page is zero-padded
 * and flushed on FINISH before the magic/CRC check.                             */
#define FUOTA_PAGE_SIZE 256
/* aligned(4): the Qflash ROM driver reads the source buffer as uint32_t words.
 * Cortex-M0 HardFaults on unaligned word access -- explicit 4-byte alignment is
 * required to guarantee the driver reads the correct bytes.                   */
static uint8_t s_page_buf[FUOTA_PAGE_SIZE] __attribute__((aligned(4)));
static uint16_t s_page_fill = 0; /* bytes currently in s_page_buf */

static uint32_t s_write_ptr = 0;        /* next flash write address (always 256-aligned) */
static uint32_t s_image_size = 0;       /* true received bytes (before page padding)     */
static uint32_t s_declared_size = 0;    /* file size the mobile app sent in START_APP (0 = not declared,
                                          * older app build -- skip the size check at FINISH) */
static uint16_t s_drop_count = 0;       /* chunks dropped (ring full or post-EOF) -- logged on FINISH */
static bool s_accepting_data = false;   /* gate: reject AD24 data before START or after EOF */
static bool s_ind_enable = false;       /* client subscribed to CFM indicate   */
static bool s_reboot_after_ind = false; /* trigger reset on indicate delivery  */
static uint8_t s_fuota_work = FUOTA_WORK_NONE;

/* ---- Write indication handler ---------------------------------------------- */

static int fuota_val_write_ind_handler(ke_msg_id_t const msgid,
                                       void const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    const struct fuotas_val_write_ind *p = (const struct fuotas_val_write_ind *)param;

    switch (p->handle)
    {
    case FUOTA_IDX_CFM_CFG:
    {
        if (p->length == 2)
        {
            uint16_t cfg = (uint16_t)(p->value[0] | (p->value[1] << 8));
            s_ind_enable = (cfg == PRF_CLI_START_IND);
            APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] indicate: %s\r\n", s_ind_enable ? "enabled" : "disabled");
        }
        break;
    }

    case FUOTA_IDX_BASE_VAL:
    {
        if (!APP_P2PS_IsAuthenticated())
        {
            APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] BASE: not authenticated\r\n");
            break;
        }
        if (p->length >= 1)
        {
            uint8_t action = p->value[0];
            switch (action)
            {
            case FUOTA_ACTION_START_APP:
                s_write_ptr = FUOTA_DL_ADDR;
                s_image_size = 0;
                /* value[0]=action, value[1..4]=base address (accepted, never
                 * actually parsed -- s_write_ptr is always FUOTA_DL_ADDR).
                 * value[5..8], if present, is the mobile app's declared file
                 * size (little-endian uint32) -- a newer addition; length==5
                 * (no size byte) is an older app build and leaves the check
                 * at FINISH disabled via s_declared_size==0. */
                s_declared_size = 0;
                if (p->length >= 9)
                {
                    s_declared_size = (uint32_t)p->value[5] |
                                       ((uint32_t)p->value[6] << 8) |
                                       ((uint32_t)p->value[7] << 16) |
                                       ((uint32_t)p->value[8] << 24);
                    APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] declared size=%lu B\r\n",
                            (unsigned long)s_declared_size);
                }
                s_drop_count = 0;        /* reset drop counter for new transfer  */
                s_accepting_data = true; /* open the data gate                   */
                s_page_fill = 0;         /* reset page buffer                    */
                s_ring_head = 0;         /* flush any stale ring entries from a  */
                s_ring_tail = 0;         /* previous aborted transfer            */
                s_fuota_work = FUOTA_WORK_START;
                UTIL_SEQ_SetTask(1U << TASK_FUOTA_WORK, CFG_SEQ_Prio_0);
                APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] START_APP queued\r\n");
                break;

            case FUOTA_ACTION_EOF:
                /* Close the data gate -- reject any AD24 chunks that arrive
                 * after EOF (in-flight in the BLE controller's buffer).
                 * Chunks already in the ring are still drained by work_task. */
                s_accepting_data = false;
                APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] EOF received (image_size=%lu)\r\n",
                        (unsigned long)s_image_size);
                break;

            case FUOTA_ACTION_FINISH:
                s_accepting_data = false;
                s_fuota_work = FUOTA_WORK_FINISH;
                UTIL_SEQ_SetTask(1U << TASK_FUOTA_WORK, CFG_SEQ_Prio_0);
                APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] FINISH queued\r\n");
                break;

            case FUOTA_ACTION_CANCEL:
            case FUOTA_ACTION_STOP:
                s_accepting_data = false;
                s_fuota_work = FUOTA_WORK_CANCEL;
                UTIL_SEQ_SetTask(1U << TASK_FUOTA_WORK, CFG_SEQ_Prio_0);
                APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] CANCEL queued\r\n");
                break;

            default:
                APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] unknown action 0x%02X\r\n", action);
                break;
            }
        }
        break;
    }

    case FUOTA_IDX_DATA_VAL:
    {
        if (!APP_P2PS_IsAuthenticated())
            break;
        if (p->length > 0 && p->length <= 240)
        {
            if (!s_accepting_data)
            {
                /* Post-EOF or pre-START in-flight chunk -- discard silently */
                s_drop_count++;
                break;
            }
            uint8_t next = (s_ring_head + 1) % FUOTA_RING_SLOTS;
            if (next != s_ring_tail)
            {
                memcpy(s_ring[s_ring_head].data, p->value, p->length);
                s_ring[s_ring_head].len = (uint16_t)p->length;
                s_ring_head = next;
                UTIL_SEQ_SetTask(1U << TASK_FUOTA_WORK, CFG_SEQ_Prio_0);
            }
            else
            {
                s_drop_count++; /* ring full -- reported at FINISH, not per-drop */
            }
        }
        else
        {
            APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] DATA invalid length %u\r\n", p->length);
        }
        break;
    }

    default:
        break;
    }

    return KE_MSG_CONSUMED;
}

/* ---- Disconnect handler ---------------------------------------------------- */

static int fuota_disconnect_handler(ke_msg_id_t const msgid,
                                    void const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    s_ind_enable = false;
    /* Always release the sleep block on disconnect -- idempotent if not held */
    UTIL_LPM_SetStopMode((1U << CFG_LPM_FUOTA_Id), UTIL_LPM_ENABLE);
    return KE_MSG_CONSUMED;
}

/* ---- Message handler table ------------------------------------------------- */

const struct ke_msg_handler fuota_app_msg_handler_list[] =
    {
        {FUOTA_VAL_WRITE_IND, fuota_val_write_ind_handler},
        {FUOTA_DISCONNECT, fuota_disconnect_handler},
    };

const struct app_subtask_handlers fuota_app_handlers = APP_HANDLERS(fuota_app);

/* ---- Sequencer task ------------------------------------------------------- */

static void fuota_clear_install_flag(void)
{
    Qflash_Init();
    Qflash_Erase_Sector(INSTALL_FLAG_ADDR);
}

/**
 * @brief Deferred OTA work: erase, flash write, finish (verify + install flag update).
 *        Called by the sequencer -- runs outside the BLE kernel event loop.
 *        Qflash_Erase_Sector / Qflash_Write are blocking but safe here.
 */
void APP_FUOTA_WorkTask(void)
{
    /* Always re-initialise Qflash before any erase/write operation.
     * Deep sleep may power-cycle the Qflash controller, invalidating the
     * RAM function pointers (CMD_FLASH_SE etc.) set by the startup call. */
    Qflash_Init();

    /* Drain all pending write chunks from the ring buffer into the page buffer.
     * rwip_schedule() can dispatch multiple FUOTA_VAL_WRITE_IND messages in
     * one pass before the sequencer gets CPU time. The ring buffer captures
     * every chunk; the page buffer assembles them into 256-byte aligned pages
     * before calling Qflash_Write (Qflash requires 256-byte page alignment). */
    while (s_ring_tail != s_ring_head)
    {
        uint8_t t = s_ring_tail;
        uint16_t len = s_ring[t].len;
        uint16_t src_off = 0;

        s_image_size += len; /* track true received byte count (before padding) */

        while (src_off < len)
        {
            uint16_t room = (uint16_t)(FUOTA_PAGE_SIZE - s_page_fill);
            uint16_t copy = len - src_off;
            if (copy > room)
                copy = room;

            memcpy(s_page_buf + s_page_fill, s_ring[t].data + src_off, copy);
            s_page_fill += copy;
            src_off += copy;

            if (s_page_fill == FUOTA_PAGE_SIZE)
            {
                if (s_write_ptr + FUOTA_PAGE_SIZE > FUOTA_DL_ADDR + FUOTA_BANK_SIZE)
                {
                    APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] Write overflow! ptr=0x%08lX\r\n",
                            (unsigned long)s_write_ptr);
                    APP_FUOTA_IndicateSend(FUOTA_CFM_ERROR);
                    return;
                }
                Qflash_Write(s_write_ptr, s_page_buf, FUOTA_PAGE_SIZE);
                s_write_ptr += FUOTA_PAGE_SIZE;
                s_page_fill = 0;
            }
        }

        s_ring_tail = (t + 1) % FUOTA_RING_SLOTS;
    }

    uint8_t work = s_fuota_work;
    s_fuota_work = FUOTA_WORK_NONE;

    switch (work)
    {
    /* ---- Erase Download Slot ------------------------------------------ */
    case FUOTA_WORK_START:
    {
        /* Prevent STOP sleep for the entire transfer.
         * BLE write-without-response has no retransmit -- packets sent while
         * the radio is off are lost. Qflash operations also require the
         * controller to stay powered.
         * UTIL_LPM_SetStopMode is idempotent (calling DISABLE twice is safe). */
        UTIL_LPM_SetStopMode((1U << CFG_LPM_FUOTA_Id), UTIL_LPM_DISABLE);
        fuota_clear_install_flag();

        uint32_t nsectors = FUOTA_BANK_SIZE / FLASH_SECTOR_SIZE;
        APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] Erasing Download Slot %u sectors @ 0x%08lX...\r\n",
                (unsigned)nsectors, (unsigned long)FUOTA_DL_ADDR);

        for (uint32_t i = 0; i < nsectors; i++)
            Qflash_Erase_Sector(FUOTA_DL_ADDR + i * FLASH_SECTOR_SIZE);

        APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] Erase done. Ready.\r\n");
        APP_FUOTA_IndicateSend(FUOTA_CFM_READY);
        break;
    }

    /* ---- Verify + update install flag + reboot ------------------------ */
    case FUOTA_WORK_FINISH:
    {
        /* Flush any partial page (zero-pad to 256 bytes).
         * The magic keyword is within the real binary bytes already written
         * to s_page_buf; zero padding goes after it -- magic check still uses
         * s_image_size as the true binary byte count.                       */
        APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] FINISH: fill=%u  ptr=0x%08lX  size=%lu  drops=%u\r\n",
                (unsigned)s_page_fill, (unsigned long)s_write_ptr,
                (unsigned long)s_image_size, (unsigned)s_drop_count);
        if (s_drop_count > 0)
        {
            APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] %u chunk(s) dropped -- rejecting\r\n",
                    (unsigned)s_drop_count);
            fuota_clear_install_flag();
            s_reboot_after_ind = true;
            APP_FUOTA_IndicateSend(FUOTA_CFM_ERROR);
            break;
        }
        if (s_page_fill > 0)
        {
            memset(s_page_buf + s_page_fill, 0x00,
                   (size_t)(FUOTA_PAGE_SIZE - s_page_fill));
            Qflash_Init(); /* re-assert WEL for partial-page flush */
            Qflash_Write(s_write_ptr, s_page_buf, FUOTA_PAGE_SIZE);
            s_write_ptr += FUOTA_PAGE_SIZE;
            s_page_fill = 0;
        }

        /* Ensure RAM stub pointers (CMD_FLASH_READ etc.) are valid before
         * the Qflash_Read calls below. If s_page_fill was 0 the partial-page
         * flush block above (which calls Qflash_Init) was skipped entirely. */
        Qflash_Init();

        if (s_image_size < 4)
        {
            APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] Image too small (%lu B)\r\n", (unsigned long)s_image_size);
            fuota_clear_install_flag();
            s_reboot_after_ind = true;
            APP_FUOTA_IndicateSend(FUOTA_CFM_ERROR);
            break;
        }

        /* Declared-vs-received size check. s_declared_size==0 means the
         * mobile app didn't send one (older build, 5-byte START_APP
         * payload) -- skip rather than reject, for backward compatibility.
         * A genuine mismatch here (chunk(s) lost without tripping the
         * drop-counter/ring-full path) is a real completeness failure the
         * magic-keyword/CRC checks below can't catch on their own -- they
         * only verify the data that arrived, not that all of it arrived. */
        if ((s_declared_size != 0) && (s_image_size != s_declared_size))
        {
            APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] Size mismatch: declared=%lu received=%lu\r\n",
                    (unsigned long)s_declared_size, (unsigned long)s_image_size);
            fuota_clear_install_flag();
            s_reboot_after_ind = true;
            APP_FUOTA_IndicateSend(FUOTA_CFM_ERROR);
            break;
        }

        /* Magic keyword check: last 4 bytes of downloaded image.
         * Use Qflash_Read() -- NOT a memory-mapped XIP dereference.
         * The QSPI AHB read buffer is NOT invalidated after Qflash_Write
         * command-mode operations; XIP reads return stale data.         */
        uint32_t magic_val = 0;
        Qflash_Read(FUOTA_DL_ADDR + s_image_size - 4,
                    (uint8_t *)&magic_val, sizeof(magic_val));
        if (magic_val != 0x94448A29U)
        {
            APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] Magic check FAILED: 0x%08lX\r\n",
                    (unsigned long)magic_val);
            fuota_clear_install_flag();
            s_reboot_after_ind = true;
            APP_FUOTA_IndicateSend(FUOTA_CFM_ERROR);
            break;
        }
        APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] Magic OK. Image %lu B, CRC...\r\n",
                (unsigned long)s_image_size);

        /* CRC of downloaded image -- Qflash_Read page-by-page (no XIP).
         * Direct XIP reads of APP2 return stale data after Qflash_Write.
         * Reuses s_page_buf (page writes are done; buffer no longer needed).
         * Feeds data as big-endian uint32 words.                            */
        uint32_t img_crc;
        {
            uint32_t remaining = s_image_size;
            uint32_t faddr = FUOTA_DL_ADDR;
            RCC->AHBPCLKEN |= RCC_AHB_PERIPH_CRC;
            CRC->CRC32CTRL = CRC32_CTRL_RESET;
            while (remaining > 0)
            {
                uint32_t chunk = (remaining >= FUOTA_PAGE_SIZE)
                                     ? FUOTA_PAGE_SIZE
                                     : remaining;
                Qflash_Read(faddr, s_page_buf, chunk);
                for (uint32_t ci = 0; ci + 4u <= chunk; ci += 4)
                {
                    CRC->CRC32DAT =
                        ((uint32_t)s_page_buf[ci] << 24) |
                        ((uint32_t)s_page_buf[ci + 1] << 16) |
                        ((uint32_t)s_page_buf[ci + 2] << 8) |
                        (uint32_t)s_page_buf[ci + 3];
                }
                faddr += chunk;
                remaining -= chunk;
            }
            img_crc = CRC->CRC32DAT;
        }

        /* Write install flag -- P2PS_OTA_INSTALLER will copy APP2->APP1 on next boot. */
        InstallFlag_t flag;
        flag.magic = INSTALL_MAGIC;
        flag.src_addr = FUOTA_DL_ADDR;
        flag.size = s_image_size;
        flag.crc = img_crc;
        Qflash_Erase_Sector(INSTALL_FLAG_ADDR);
        Qflash_Write(INSTALL_FLAG_ADDR, (uint8_t *)&flag, sizeof(InstallFlag_t));

        APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] Install flag written. img_crc=0x%08lX. Indicating reboot...\r\n",
                (unsigned long)img_crc);

        /* Release STOP sleep block before reboot indication -- device is
         * about to reset so sleep prevention is no longer needed.          */
        UTIL_LPM_SetStopMode((1U << CFG_LPM_FUOTA_Id), UTIL_LPM_ENABLE);

        /* Set flag before sending -- reset fires in APP_FUOTA_IndicateComplete()
         * once the BLE stack confirms indication delivery (GATTC_CMP_EVT). */
        s_reboot_after_ind = true;
        APP_FUOTA_IndicateSend(FUOTA_CFM_REBOOT);
        break;
    }

    /* ---- Cancel ------------------------------------------------------- */
    case FUOTA_WORK_CANCEL:
    {
        s_write_ptr = FUOTA_DL_ADDR;
        s_image_size = 0;
        s_page_fill = 0;
        /* Release STOP sleep block -- idempotent if not currently held */
        UTIL_LPM_SetStopMode((1U << CFG_LPM_FUOTA_Id), UTIL_LPM_ENABLE);
        fuota_clear_install_flag();
        APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] Transfer cancelled.\r\n");
        APP_FUOTA_IndicateSend(FUOTA_CFM_READY);
        break;
    }

    default:
        break;
    }
}

/* ---- Indicate send --------------------------------------------------------- */

void APP_FUOTA_IndicateSend(uint8_t status_code)
{
    if (!s_ind_enable)
    {
        APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] indicate not subscribed (code=0x%02X)\r\n", status_code);
        return;
    }

    if (ke_state_get(ns_ble_fuotas_task) == FUOTAS_BUSY)
    {
        APP_LOG(TS_OFF, VLEVEL_L, "[FUOTA] indicate busy (code=0x%02X)\r\n", status_code);
        return;
    }

    ke_state_set(ns_ble_fuotas_task, FUOTAS_BUSY);

    struct fuotas_env_tag *fuotas_env = PRF_ENV_GET(FUOTA, fuotas);
    struct gattc_send_evt_cmd *req = KE_MSG_ALLOC_DYN(
        GATTC_SEND_EVT_CMD,
        KE_BUILD_ID(TASK_GATTC, app_env.conidx),
        fuotas_env->prf_env.prf_task,
        gattc_send_evt_cmd,
        1);

    req->operation = GATTC_INDICATE;
    req->handle = fuotas_env->shdl + FUOTA_IDX_CFM_VAL;
    req->length = 1;
    req->value[0] = status_code;
    ke_msg_send(req);
}

/* ---- Indicate complete (called from fuotas_task gattc_cmp_evt_handler) -----
 * Name kept lowercase (app_fuota_*, not this file's own APP_FUOTA_* style)
 * -- it's a hardcoded external reference from the shared, non-project-local
 * profiles/fuota/fuotas/src/fuotas_task.c (confirmed by direct read). */

void app_fuota_indicate_complete(void)
{
    if (s_reboot_after_ind)
    {
        s_reboot_after_ind = false;
        APP_LOG(TS_OFF, VLEVEL_M, "[FUOTA] Indicate confirmed. Rebooting now.\r\n");
        NVIC_SystemReset();
    }
}

/* ---- Public API ------------------------------------------------------------ */

void APP_FUOTA_AddFuota(void)
{
    struct fuotas_db_cfg *db_cfg;
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(
        GAPM_PROFILE_TASK_ADD_CMD,
        TASK_GAPM,
        TASK_APP,
        gapm_profile_task_add_cmd,
        sizeof(struct fuotas_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH);
    req->prf_task_id = TASK_ID_FUOTA;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct fuotas_db_cfg *)req->param;
    db_cfg->att_tbl = NULL;
    db_cfg->cfg_flag = 0;
    db_cfg->features = 0;

    ke_msg_send(req);
    APP_FUOTA_Init();
}

void APP_FUOTA_Init(void)
{
    struct prf_task_t prf;
    prf.prf_task_id = TASK_ID_FUOTA;
    prf.prf_task_handler = &fuota_app_handlers;
    ns_ble_prf_task_register(&prf);

    struct prf_get_func_t get_func;
    get_func.task_id = TASK_ID_FUOTA;
    get_func.prf_itf_get_func = fuotas_prf_itf_get;
    prf_get_itf_func_register(&get_func);
}

#endif /* BLE_APP_FUOTA */
