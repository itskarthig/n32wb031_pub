/**
 * @file ns_log_if.c
 * @brief Staging ring buffer + drain task backing ns_log_if.h's NS_LOG_x
 * macros -- spreads the BLE SDK's own bursty NS_LOG_DEBUG sequences (several
 * calls in one scheduler tick, e.g. app_create_advertising/app_set_adv_data/
 * app_set_scan_rsp_data/app_start_advertising, each logging) out over
 * multiple UTIL_SEQ_Run() passes instead of pushing them all into the
 * shared trace FIFO (Utilities/trace/adv_trace/n32_adv_trace.c) at once.
 *
 * Root problem this fixes: TRACE_AllocateBufer() silently drops a whole
 * message with zero error propagation when its ring buffer lacks room --
 * raising UTIL_ADV_TRACE_FIFO_SIZE (utilities_conf.h, 512->1024) closed
 * most of the gap, but the single densest burst (the very first ADV-start
 * call at cold boot, landing right after BLE stack init + WMBus TX
 * hex-dumps) can still overflow it. Since a formatted message can't be
 * "frozen" and reformatted later, this defers where the already-formatted
 * string goes next: one message drains into the real trace engine per
 * pump cycle, giving the UART's own TC-interrupt drain time to free up
 * FIFO space between each push, instead of N messages landing in the
 * same instant.
 *
 * Scope: NS_LOG_x only, per explicit design decision -- this project's own
 * direct APP_LOG() calls are not bursty and stay on the existing
 * synchronous path unchanged.
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_compiler.h" /* __get_PRIMASK, __disable_irq, __set_PRIMASK */

#include "app_config.h" /* TASK_NS_LOG_DRAIN_ID, CFG_SEQ_Prio_1 */
#include "app_log.h"    /* APP_LOG (this file's own forwarding target) */
#include "n32_seq.h"    /* UTIL_SEQ_SetTask */
#include "ns_log_if.h"

/** Ring capacity in bytes. Only needs to survive one burst's worth of
 * messages before draining (one per pump cycle) -- not accumulate
 * indefinitely. Half the current 1024 B trace FIFO; power-of-2 to match
 * that file's own sizing convention. */
#define NS_LOG_IF_RING_SIZE 512U

/** Max bytes of a single formatted message (vsnprintf truncates beyond
 * this, matching how UTIL_ADV_TRACE_COND_FSend's own scratch buffer
 * already behaves today). */
#define NS_LOG_IF_MSG_MAX 160U

/* Length-prefixed framing: [1-byte length][payload], mirroring
 * n32_adv_trace.c's own FIFO design. */
static uint8_t s_ring[NS_LOG_IF_RING_SIZE];
static uint16_t s_head; /* next byte to write */
static uint16_t s_tail; /* next byte to read */
static uint16_t s_used; /* bytes currently occupied */

/* Purely diagnostic -- incremented when a message can't fit and is
 * silently dropped, mirroring n32_adv_trace.c's own
 * UTIL_ADV_TRACE_GetDropCount() pattern. Never checked by any call site
 * today; available for a future hardware capture to confirm the ring
 * never actually overflows at this sizing. */
static uint32_t s_ns_log_if_drop_count;

static uint32_t NsLogIf_PrimSave(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void NsLogIf_PrimRestore(uint32_t primask)
{
    __set_PRIMASK(primask);
}

/** Pushes one length-prefixed frame. Caller already holds the critical
 * section. Returns false (and drops silently) if the frame doesn't fit. */
static bool NsLogIf_Push(const uint8_t *data, uint8_t len)
{
    uint16_t frame_len = (uint16_t)len + 1U;

    if ((uint32_t)s_used + frame_len > NS_LOG_IF_RING_SIZE)
    {
        return false;
    }

    s_ring[s_head] = len;
    s_head = (uint16_t)((s_head + 1U) % NS_LOG_IF_RING_SIZE);
    for (uint8_t i = 0U; i < len; i++)
    {
        s_ring[s_head] = data[i];
        s_head = (uint16_t)((s_head + 1U) % NS_LOG_IF_RING_SIZE);
    }
    s_used = (uint16_t)(s_used + frame_len);
    return true;
}

/** Pops one length-prefixed frame into dst (must be >= NS_LOG_IF_MSG_MAX+1).
 * Caller already holds the critical section. Returns the payload length,
 * or 0 if the ring is empty. NUL-terminates dst. */
static uint8_t NsLogIf_Pop(uint8_t *dst)
{
    uint8_t len;

    if (s_used == 0U)
    {
        return 0U;
    }

    len = s_ring[s_tail];
    s_tail = (uint16_t)((s_tail + 1U) % NS_LOG_IF_RING_SIZE);
    for (uint8_t i = 0U; i < len; i++)
    {
        dst[i] = s_ring[s_tail];
        s_tail = (uint16_t)((s_tail + 1U) % NS_LOG_IF_RING_SIZE);
    }
    dst[len] = 0U;
    s_used = (uint16_t)(s_used - (uint32_t)len - 1U);
    return len;
}

void NsLogIf_Enqueue(const char *fmt, ...)
{
    char buf[NS_LOG_IF_MSG_MAX];
    va_list args;
    int written;
    uint32_t primask;
    bool pushed;

    va_start(args, fmt);
    written = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (written < 0)
    {
        return;
    }
    if ((uint32_t)written >= sizeof(buf))
    {
        written = (int)sizeof(buf) - 1;
    }

    primask = NsLogIf_PrimSave();
    pushed = NsLogIf_Push((const uint8_t *)buf, (uint8_t)written);
    if (!pushed)
    {
        s_ns_log_if_drop_count++;
    }
    NsLogIf_PrimRestore(primask);

    if (pushed)
    {
        UTIL_SEQ_SetTask((1U << TASK_NS_LOG_DRAIN_ID), CFG_SEQ_Prio_1);
    }
}

void NsLogIf_DrainTask(void)
{
    char buf[NS_LOG_IF_MSG_MAX + 1U];
    uint32_t primask;
    uint8_t len;
    bool more;

    primask = NsLogIf_PrimSave();
    len = NsLogIf_Pop((uint8_t *)buf);
    more = (s_used != 0U);
    NsLogIf_PrimRestore(primask);

    if (len != 0U)
    {
        /* Pre-formatted text passed as a %s ARGUMENT, never re-parsed as a
         * format string -- safe even if the original message contains
         * literal '%' characters. */
        APP_LOG(TS_OFF, VLEVEL_M, "%s", buf);
    }

    if (more)
    {
        /* CFG_SEQ_Prio_1, not _Prio_0 -- a Prio_0 self-requeue while the
         * queue stays non-empty would starve ble_pump_task (same
         * starvation class as this project's own documented Bug 39,
         * TASK_CELL_LOG_NOTIFY, from the old app_ble_cellular_wmbus
         * project). */
        UTIL_SEQ_SetTask((1U << TASK_NS_LOG_DRAIN_ID), CFG_SEQ_Prio_1);
    }
}

uint32_t NsLogIf_GetDropCount(void)
{
    return s_ns_log_if_drop_count;
}
