/**
 * @file trace_if.c
 * @brief Unified interrupt-driven UART/LPUART hardware driver for n32_adv_trace.
 *
 * Implements the UTIL_ADV_TRACE_Driver_s contract (Init/DeInit/StartRx/Send)
 * declared in n32_adv_trace.h. TX is fully interrupt-driven: Send() arms the
 * peripheral and returns immediately; the ISR feeds subsequent bytes and
 * calls the completion callback registered via Init() once the buffer has
 * fully drained -- no CPU stall/polling anywhere in the TX path.
 *
 * Peripheral/pin selection is compile-time via USE_LPUART1 / USE_USART1 /
 * USE_USART2 (and the pin-variant sub-selects), defined in the project's
 * utilities_conf.h. RX is gated by ONLY_TX (default 1 = TX-only, StartRx()
 * a stub; 0 = RX pin also configured and StartRx() arms real single-byte
 * interrupt-driven receive) -- see trace_if.h.
 */

#include "trace_if.h"
#include "n32wb03x.h"

/* ---------------------------------------------------------------------------
 * Peripheral/pin selection
 * ------------------------------------------------------------------------- */

#if (defined(USE_LPUART1) + defined(USE_USART1) + defined(USE_USART2)) != 1
#error "trace_if.c: define exactly one of USE_LPUART1, USE_USART1, USE_USART2 in utilities_conf.h"
#endif

#ifndef TRACE_BAUDRATE
#define TRACE_BAUDRATE 115200U
#endif

#if defined(USE_LPUART1)

#if defined(LPUART1_PINS_PB11_PB12)
#define TRACE_TX_PIN       GPIO_PIN_12
#define TRACE_TX_GPIO_AF   GPIO_AF2_LPUART1
#define TRACE_RX_PIN       GPIO_PIN_11
#define TRACE_RX_GPIO_AF   GPIO_AF2_LPUART1
#else
#define TRACE_TX_PIN       GPIO_PIN_1
#define TRACE_TX_GPIO_AF   GPIO_AF4_LPUART1
#define TRACE_RX_PIN       GPIO_PIN_2
#define TRACE_RX_GPIO_AF   GPIO_AF4_LPUART1
#endif
#define TRACE_TX_GPIO_PORT    GPIOB
#define TRACE_TX_GPIO_CLK     RCC_APB2_PERIPH_GPIOB
#define TRACE_RX_GPIO_PORT    GPIOB
#define TRACE_RX_GPIO_CLK     RCC_APB2_PERIPH_GPIOB

#elif defined(USE_USART1)

#define TRACE_TX_PIN       GPIO_PIN_6
#define TRACE_TX_GPIO_AF   GPIO_AF4_USART1
#define TRACE_RX_PIN       GPIO_PIN_7
#define TRACE_RX_GPIO_AF   GPIO_AF4_USART1
#define TRACE_TX_GPIO_PORT    GPIOB
#define TRACE_TX_GPIO_CLK     RCC_APB2_PERIPH_GPIOB
#define TRACE_RX_GPIO_PORT    GPIOB
#define TRACE_RX_GPIO_CLK     RCC_APB2_PERIPH_GPIOB

#elif defined(USE_USART2)

#if defined(USART2_PINS_PA6_PB10)
#define TRACE_TX_PIN       GPIO_PIN_6
/* GPIO_AF2_USART2 on PA6/PB10 is inferred from the datasheet AF table -- no
 * SDK reference example exercises this exact pin/peripheral combination. */
#define TRACE_TX_GPIO_AF   GPIO_AF2_USART2
#define TRACE_TX_GPIO_PORT GPIOA
#define TRACE_TX_GPIO_CLK  RCC_APB2_PERIPH_GPIOA
#define TRACE_RX_PIN       GPIO_PIN_10
#define TRACE_RX_GPIO_AF   GPIO_AF2_USART2
#define TRACE_RX_GPIO_PORT GPIOB
#define TRACE_RX_GPIO_CLK  RCC_APB2_PERIPH_GPIOB
#else
#define TRACE_TX_PIN       GPIO_PIN_4
#define TRACE_TX_GPIO_AF   GPIO_AF3_USART2
#define TRACE_TX_GPIO_PORT GPIOB
#define TRACE_TX_GPIO_CLK  RCC_APB2_PERIPH_GPIOB
#define TRACE_RX_PIN       GPIO_PIN_5
#define TRACE_RX_GPIO_AF   GPIO_AF3_USART2
#define TRACE_RX_GPIO_PORT GPIOB
#define TRACE_RX_GPIO_CLK  RCC_APB2_PERIPH_GPIOB
#endif

#endif /* USE_LPUART1 / USE_USART1 / USE_USART2 */

/* ---------------------------------------------------------------------------
 * Private state
 * ------------------------------------------------------------------------- */

static void (*s_tx_cplt_cb)(void *ptr) = NULL;
static const uint8_t *s_tx_buf = NULL;
static uint16_t s_tx_len = 0u;
static uint16_t s_tx_idx = 0u;

#if (ONLY_TX == 0)
static void (*s_rx_cplt_cb)(uint8_t *pdata, uint16_t size, uint8_t error) = NULL;
static uint8_t s_rx_byte;
#endif

/* ---------------------------------------------------------------------------
 * Driver functions
 * ------------------------------------------------------------------------- */

/**
 * @brief Hardware-only (re-)init of the selected trace UART peripheral --
 * clocks, GPIO AF, peripheral config, NVIC. Extracted so it can be called
 * both from TraceIf_Init() (full init, incl. storing the completion
 * callback) and from TraceIf_WakeReinit() (hardware-only, callback
 * pointer unchanged) -- see trace_if.h.
 */
static void TraceIf_HwInit(void)
{
    GPIO_InitType GPIO_InitStructure = {0};
    NVIC_InitType NVIC_InitStructure = {0};

#if defined(USE_LPUART1)

#if (ONLY_TX == 0)
    RCC_EnableAPB2PeriphClk(TRACE_TX_GPIO_CLK | TRACE_RX_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
#else
    RCC_EnableAPB2PeriphClk(TRACE_TX_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
#endif
    RCC_ConfigLpuartClk(RCC_LPUART1CLK, RCC_LPUARTCLK_SRC_APB1);
    RCC_EnableLpuartClk(ENABLE);

    GPIO_InitStructure.Pin            = TRACE_TX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = TRACE_TX_GPIO_AF;
    GPIO_InitPeripheral(TRACE_TX_GPIO_PORT, &GPIO_InitStructure);

#if (ONLY_TX == 0)
    GPIO_InitStructure.Pin            = TRACE_RX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = TRACE_RX_GPIO_AF;
    GPIO_InitPeripheral(TRACE_RX_GPIO_PORT, &GPIO_InitStructure);
#else
    /* RX unused -- ANALOG disables the digital input buffer, eliminating
     * shoot-through leakage current from an otherwise-floating pin (same
     * class of leakage the sibling app_ble_* projects hit and fixed --
     * CLAUDE.md bug 5). */
    RCC_EnableAPB2PeriphClk(TRACE_RX_GPIO_CLK, ENABLE);
    GPIO_InitStructure.Pin       = TRACE_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(TRACE_RX_GPIO_PORT, &GPIO_InitStructure);
#endif

    LPUART_InitType LPUART_InitStructure = {0};
    LPUART_InitStructure.BaudRate            = TRACE_BAUDRATE;
    LPUART_InitStructure.Parity              = LPUART_PE_NO;
#if (ONLY_TX == 0)
    LPUART_InitStructure.Mode                = LPUART_MODE_TX | LPUART_MODE_RX;
#else
    LPUART_InitStructure.Mode                = LPUART_MODE_TX;
#endif
    LPUART_InitStructure.RtsThreshold        = LPUART_RTSTH_FIFOFU;
    LPUART_InitStructure.HardwareFlowControl = LPUART_HFCTRL_NONE;
    LPUART_DeInit(LPUART1);
    LPUART_Init(LPUART1, &LPUART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel         = LPUART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#elif defined(USE_USART1)

#if (ONLY_TX == 0)
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1 | TRACE_TX_GPIO_CLK | TRACE_RX_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
#else
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1 | TRACE_TX_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
#endif

    GPIO_InitStructure.Pin            = TRACE_TX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = TRACE_TX_GPIO_AF;
    GPIO_InitPeripheral(TRACE_TX_GPIO_PORT, &GPIO_InitStructure);

#if (ONLY_TX == 0)
    GPIO_InitStructure.Pin            = TRACE_RX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = TRACE_RX_GPIO_AF;
    GPIO_InitPeripheral(TRACE_RX_GPIO_PORT, &GPIO_InitStructure);
#else
    /* RX unused -- ANALOG disables the digital input buffer, eliminating
     * shoot-through leakage current from an otherwise-floating pin (same
     * class of leakage the sibling app_ble_* projects hit and fixed --
     * CLAUDE.md bug 5). */
    RCC_EnableAPB2PeriphClk(TRACE_RX_GPIO_CLK, ENABLE);
    GPIO_InitStructure.Pin       = TRACE_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(TRACE_RX_GPIO_PORT, &GPIO_InitStructure);
#endif

    USART_InitType USART_InitStructure = {0};
    USART_InitStructure.BaudRate            = TRACE_BAUDRATE;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
#if (ONLY_TX == 0)
    USART_InitStructure.Mode                = USART_MODE_TX | USART_MODE_RX;
#else
    USART_InitStructure.Mode                = USART_MODE_TX;
#endif
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_DeInit(USART1);
    USART_Init(USART1, &USART_InitStructure);
    USART_Enable(USART1, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel         = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#elif defined(USE_USART2)

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, ENABLE);
#if (ONLY_TX == 0)
    RCC_EnableAPB2PeriphClk(TRACE_TX_GPIO_CLK | TRACE_RX_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
#else
    RCC_EnableAPB2PeriphClk(TRACE_TX_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);
#endif

    GPIO_InitStructure.Pin            = TRACE_TX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = TRACE_TX_GPIO_AF;
    GPIO_InitPeripheral(TRACE_TX_GPIO_PORT, &GPIO_InitStructure);

#if (ONLY_TX == 0)
    GPIO_InitStructure.Pin            = TRACE_RX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = TRACE_RX_GPIO_AF;
    GPIO_InitPeripheral(TRACE_RX_GPIO_PORT, &GPIO_InitStructure);
#else
    /* RX unused -- ANALOG disables the digital input buffer, eliminating
     * shoot-through leakage current from an otherwise-floating pin (same
     * class of leakage the sibling app_ble_* projects hit and fixed --
     * CLAUDE.md bug 5). */
    RCC_EnableAPB2PeriphClk(TRACE_RX_GPIO_CLK, ENABLE);
    GPIO_InitStructure.Pin       = TRACE_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(TRACE_RX_GPIO_PORT, &GPIO_InitStructure);
#endif

    USART_InitType USART_InitStructure = {0};
    USART_InitStructure.BaudRate            = TRACE_BAUDRATE;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
#if (ONLY_TX == 0)
    USART_InitStructure.Mode                = USART_MODE_TX | USART_MODE_RX;
#else
    USART_InitStructure.Mode                = USART_MODE_TX;
#endif
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_DeInit(USART2);
    USART_Init(USART2, &USART_InitStructure);
    USART_Enable(USART2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel         = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#endif
}

static UTIL_ADV_TRACE_Status_t TraceIf_Init(void (*cb)(void *ptr))
{
    s_tx_cplt_cb = cb;
    s_tx_buf = NULL;
    s_tx_len = 0u;
    s_tx_idx = 0u;

    TraceIf_HwInit();

    return UTIL_ADV_TRACE_OK;
}

/**
 * @brief Re-init the trace UART hardware after a low-power wake, and force
 * any transmission that was genuinely in flight when the MCU went to
 * sleep to complete -- its ISR chain is dead now that the peripheral has
 * just been reset, so without this s_tx_idx/s_tx_len (and the driver's
 * Busy/voter state one layer up in n32_adv_trace.c) would stay wedged
 * forever. Called from lpm_if.c's LpmIf_ExitStopMode() on every Standby
 * wake -- see CLAUDE.md BEACON/APP bugs for the diagnosis (this MCU's
 * "peripheral interfaces closed" Standby-mode transition can leave USART1
 * unable to complete a transmission that was in progress across the
 * transition).
 */
void TraceIf_WakeReinit(void)
{
    TraceIf_HwInit();

    if (s_tx_idx < s_tx_len)
    {
        s_tx_idx = s_tx_len;
        if (s_tx_cplt_cb != NULL)
        {
            s_tx_cplt_cb(NULL);
        }
    }
}

/**
 * @brief Deinit the trace UART hardware before entering Sleep mode, mirroring
 * the vendor's own non-BLE reference (N32WB03x_SDK_V2.0.0/projects/
 * n32wb03x_EVAL/application/peripheral_alone/src/app_usart.c,
 * USART_Deinitializes()) -- disables the peripheral and sets both TX and RX
 * pins to ANALOG "to save power" (that reference's own comment). Called
 * from lpm_if.c's LpmIf_EnterStopMode() right before PWR_EnterSLEEPMode().
 * Symmetric with TraceIf_WakeReinit(), which fully re-inits on wake.
 */
void TraceIf_PreSleepDeinit(void)
{
    GPIO_InitType GPIO_InitStructure = {0};

#if defined(USE_LPUART1)
    LPUART_DeInit(LPUART1);
#elif defined(USE_USART1)
    USART_Enable(USART1, DISABLE);
#elif defined(USE_USART2)
    USART_Enable(USART2, DISABLE);
#endif

    GPIO_InitStructure.Pin       = TRACE_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(TRACE_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = TRACE_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(TRACE_RX_GPIO_PORT, &GPIO_InitStructure);
}

void TraceIf_Flush(void)
{
    /* Timeout budget matches the proven old-structure uart2_trace_flush()
     * exactly (~120 ms @ 64 MHz) -- see trace_if.h for the full rationale.
     * s_tx_idx/s_tx_len are this driver's own in-flight-chunk state;
     * UTIL_ADV_TRACE_IsBufferEmpty() additionally catches the case where
     * the current hardware chunk just finished but more chunks are still
     * queued in the ring buffer above this driver (TraceIf_Send() would
     * still be about to be re-armed by TRACE_TxCpltCallback()'s chaining
     * branch when that happens). */
    uint32_t timeout = 120000U;
    while (((s_tx_idx < s_tx_len) || (UTIL_ADV_TRACE_IsBufferEmpty() == 0U)) && (--timeout != 0U))
    {
    }
}

static UTIL_ADV_TRACE_Status_t TraceIf_DeInit(void)
{
    NVIC_InitType NVIC_InitStructure = {0};

#if defined(USE_LPUART1)
#if (ONLY_TX == 0)
    LPUART_ConfigInt(LPUART1, LPUART_INT_FIFO_NE, DISABLE);
#endif
    NVIC_InitStructure.NVIC_IRQChannel    = LPUART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    LPUART_DeInit(LPUART1);
    RCC_EnableLpuartClk(DISABLE);
#elif defined(USE_USART1)
#if (ONLY_TX == 0)
    USART_ConfigInt(USART1, USART_INT_RXDNE, DISABLE);
#endif
    NVIC_InitStructure.NVIC_IRQChannel    = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_DeInit(USART1);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, DISABLE);
#elif defined(USE_USART2)
#if (ONLY_TX == 0)
    USART_ConfigInt(USART2, USART_INT_RXDNE, DISABLE);
#endif
    NVIC_InitStructure.NVIC_IRQChannel    = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_DeInit(USART2);
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, DISABLE);
#endif

    s_tx_cplt_cb = NULL;
#if (ONLY_TX == 0)
    s_rx_cplt_cb = NULL;
#endif
    return UTIL_ADV_TRACE_OK;
}

static UTIL_ADV_TRACE_Status_t TraceIf_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error))
{
#if (ONLY_TX == 0)
    s_rx_cplt_cb = cb;

#if defined(USE_LPUART1)
    LPUART_ConfigInt(LPUART1, LPUART_INT_FIFO_NE, ENABLE);
#elif defined(USE_USART1)
    USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
#elif defined(USE_USART2)
    USART_ConfigInt(USART2, USART_INT_RXDNE, ENABLE);
#endif

    return UTIL_ADV_TRACE_OK;
#else
    /* TX-only driver (ONLY_TX=1) -- RX not implemented. */
    (void)cb;
    return UTIL_ADV_TRACE_HW_ERROR;
#endif
}

static UTIL_ADV_TRACE_Status_t TraceIf_Send(uint8_t *pdata, uint16_t size)
{
    if (size == 0u)
    {
        return UTIL_ADV_TRACE_OK;
    }

    s_tx_buf = pdata;
    s_tx_len = size;
    s_tx_idx = 0u;

#if defined(USE_LPUART1)
    /* Manually kick off byte 0; the ISR (LPUART_INT_TXC) feeds the rest. */
    s_tx_idx = 1u;
    LPUART_SendData(LPUART1, s_tx_buf[0]);
    LPUART_ConfigInt(LPUART1, LPUART_INT_TXC, ENABLE);
#elif defined(USE_USART1)
    /* Clear any stale TXC left over from a prior burst before re-arming --
     * see USART1_IRQHandler() for why TXC (not TXDE) gates completion. */
    USART_ClrFlag(USART1, USART_FLAG_TXC);
    USART_ConfigInt(USART1, USART_INT_TXDE, ENABLE);
#elif defined(USE_USART2)
    USART_ClrFlag(USART2, USART_FLAG_TXC);
    USART_ConfigInt(USART2, USART_INT_TXDE, ENABLE);
#endif

    return UTIL_ADV_TRACE_OK;
}

/* ---------------------------------------------------------------------------
 * Driver table
 * ------------------------------------------------------------------------- */

const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver = {
    TraceIf_Init,
    TraceIf_DeInit,
    TraceIf_StartRx,
    TraceIf_Send,
};

/* ---------------------------------------------------------------------------
 * Interrupt handlers -- override the weak defaults in startup_n32wb03x_gcc.s
 * ------------------------------------------------------------------------- */

#if defined(USE_LPUART1)
void LPUART1_IRQHandler(void)
{
    if (LPUART_GetIntStatus(LPUART1, LPUART_INT_TXC) != RESET)
    {
        LPUART_ClrIntPendingBit(LPUART1, LPUART_INT_TXC);

        if (s_tx_idx < s_tx_len)
        {
            LPUART_SendData(LPUART1, s_tx_buf[s_tx_idx]);
            s_tx_idx++;
        }
        else
        {
            LPUART_ConfigInt(LPUART1, LPUART_INT_TXC, DISABLE);
            if (s_tx_cplt_cb != NULL)
            {
                s_tx_cplt_cb(NULL);
            }
        }
    }

#if (ONLY_TX == 0)
    if (LPUART_GetIntStatus(LPUART1, LPUART_INT_FIFO_NE) != RESET)
    {
        s_rx_byte = LPUART_ReceiveData(LPUART1);
        if (s_rx_cplt_cb != NULL)
        {
            s_rx_cplt_cb(&s_rx_byte, 1, 0);
        }
    }
#endif
}
#elif defined(USE_USART1)
void USART1_IRQHandler(void)
{
    if (USART_GetIntStatus(USART1, USART_INT_TXDE) != RESET)
    {
        if (s_tx_idx < s_tx_len)
        {
            USART_SendData(USART1, s_tx_buf[s_tx_idx]);
            s_tx_idx++;
        }
        else
        {
            /* Last byte has been moved into the shift register but is not
             * yet physically transmitted -- switch to TXC (true completion,
             * stop bit clocked out) before signaling s_tx_cplt_cb(), or
             * PostSendHook() releases the StopMode block while the byte is
             * still on the wire and STOP can gate the peripheral clock
             * mid-shift-out. This mirrors the standard "buffer-empty vs.
             * transmission-complete" distinction most UART drivers make
             * for exactly this reason. */
            USART_ConfigInt(USART1, USART_INT_TXDE, DISABLE);
            USART_ConfigInt(USART1, USART_INT_TXC, ENABLE);
        }
    }

    if (USART_GetIntStatus(USART1, USART_INT_TXC) != RESET)
    {
        USART_ClrIntPendingBit(USART1, USART_INT_TXC);
        USART_ConfigInt(USART1, USART_INT_TXC, DISABLE);
        if (s_tx_cplt_cb != NULL)
        {
            s_tx_cplt_cb(NULL);
        }
    }

#if (ONLY_TX == 0)
    if (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET)
    {
        s_rx_byte = (uint8_t)USART_ReceiveData(USART1);
        if (s_rx_cplt_cb != NULL)
        {
            s_rx_cplt_cb(&s_rx_byte, 1, 0);
        }
    }
#endif
}
#elif defined(USE_USART2)
void USART2_IRQHandler(void)
{
    if (USART_GetIntStatus(USART2, USART_INT_TXDE) != RESET)
    {
        if (s_tx_idx < s_tx_len)
        {
            USART_SendData(USART2, s_tx_buf[s_tx_idx]);
            s_tx_idx++;
        }
        else
        {
            /* See USART1_IRQHandler() above -- same TXDE/TXC completion
             * split, kept in sync for whichever peripheral is selected. */
            USART_ConfigInt(USART2, USART_INT_TXDE, DISABLE);
            USART_ConfigInt(USART2, USART_INT_TXC, ENABLE);
        }
    }

    if (USART_GetIntStatus(USART2, USART_INT_TXC) != RESET)
    {
        USART_ClrIntPendingBit(USART2, USART_INT_TXC);
        USART_ConfigInt(USART2, USART_INT_TXC, DISABLE);
        if (s_tx_cplt_cb != NULL)
        {
            s_tx_cplt_cb(NULL);
        }
    }

#if (ONLY_TX == 0)
    if (USART_GetIntStatus(USART2, USART_INT_RXDNE) != RESET)
    {
        s_rx_byte = (uint8_t)USART_ReceiveData(USART2);
        if (s_rx_cplt_cb != NULL)
        {
            s_rx_cplt_cb(&s_rx_byte, 1, 0);
        }
    }
#endif
}
#endif
