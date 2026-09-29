/**
 * @file lpm_if.c
 * @brief Project-local Low Power Manager hardware driver for BEACON/APP.
 *
 * See lpm_if.h for the tier-mapping rationale. Each Enter* function enables
 * the PWR APB1 clock first, matching both vendor reference projects
 * (peripheral/PWR/SLEEP and peripheral/PWR/PD main.c).
 *
 * LpmIf_EnterStopMode() (User Manual S3.3.4 "Sleep mode", the deep tier)
 * directly reproduces ns_library's own entry_sleep() (ns_sleep.c) register
 * sequence -- verbatim, not a call into it or into ns_sleep()/ns_sleep_lock
 * (this project deliberately keeps the plain UTIL_LPM voter-bitmask engine
 * as the sole sleep-decision mechanism, consistent with every other module;
 * this is our own code in our own file, just the proven-correct register
 * operations written directly instead of relying on the SDK's
 * PWR_EnterSLEEPMode() wrapper).
 *
 * Why not PWR_EnterSLEEPMode()? Two rounds of hardware testing found it
 * insufficient once BLE is linked. Round 1: it matches entry_sleep() on
 * the *entry* side (BLE_BB deep-sleep-request at 0x40028030, wait
 * PWR->CR1 OSC_EN clear, SLEEPDEEP+WFI) but never does entry_sleep()'s
 * *exit*-side BLE wake request (PWR->CR2|=0x100, wait OSC_EN set) or its
 * EXTI_PA11_Configuration() re-arm -- confirmed on hardware: BLE
 * advertised fine through the wake window (StopMode blocked, no real
 * sleep happened) then died permanently the moment the first real
 * StopMode cycle occurred. Round 2: composing PWR_EnterSLEEPMode() with
 * those two missing lines bolted on afterward *hung* on hardware (~1.2 mA,
 * stuck spinning in the OSC_EN wait) -- because PWR_EnterSLEEPMode() only
 * does a narrow, reversible single-bit toggle on register 0x40011004
 * (|=0x40 before sleep, &=~0x40 after), never touching
 * RCC->LSCTRL/CFG/APB1PCLKEN/APB2PCLKEN at all, whereas entry_sleep()
 * saves/zeroes/restores that *entire* register and re-enables those RCC
 * clocks after WFI -- evidently required before PWR->CR1's OSC_EN bit can
 * ever be set again by the BLE hardware. Composing two independently-
 * correct-but-different sequences produced neither; reproducing
 * entry_sleep() faithfully avoids the whole class of mismatch.
 *
 * Residual known trade-off, unchanged from before: unlike ns_sleep()
 * (which first asks the ROM's rwip_sleep() whether a BLE radio event is
 * due imminently before allowing deep sleep at all), UTIL_LPM has no
 * visibility into BLE's own scheduling -- StopMode could still
 * occasionally be entered moments before a due ADV/connection event,
 * which the BLE core would then miss (soft degradation -- a skipped
 * interval, not the "dead forever" or "hung forever" failures the two
 * rounds above addressed). Accepted for now; revisit if this proves
 * observable on hardware.
 *
 * LpmIf_ExitStopMode() re-arms EXTI->IMASK (a confirmed real hardware
 * side-effect of this wake path) and calls TraceIf_WakeReinit() to fully
 * restore the trace UART, symmetric with TraceIf_PreSleepDeinit().
 * LpmIf_ExitSleepMode()/ExitOffMode() need no such fixup -- PWR_EnterIDLEMode()
 * is a plain WFI, and PWR_EnterPDMode() resets the MCU on wake (everything
 * re-inits from Reset_Handler, so EXTI/UART state is moot).
 */

#include "lpm_if.h"
#include "n32_lpm.h"
#include "n32wb03x.h"
#include "trace_if.h"
#include "app_config.h" /* CFG_LPM_UART_TX_Id -- see LpmIf_ExitStopMode()'s force-release */

/* Genuine ROM function (resolved via symbol_g15.txt, exactly like
 * rwip_schedule()) -- declared extern in ns_ble_stack/stack_common/
 * global_func.h, redeclared here rather than pulling in that header
 * wholesale for one BLE-internal declaration. Arms the BLE baseband's
 * wake-IRQ path (PA11/EXTI_LINE11); must be called before every deep-sleep
 * request, not just once at boot -- confirmed by ns_sleep.c calling it from
 * inside entry_sleep()/entry_idle() every cycle, never during init. */
extern void EXTI_PA11_Configuration(void);

/* ns_ble_stack/arch/TypeDefine.h defines the same macro, but that header
 * isn't worth pulling in here just for this (drags in BLE-internal
 * UINT32-style typedefs). */
#define REG32(addr) (*(volatile uint32_t *)(addr))

static void LpmIf_EnterSleepMode(void);
static void LpmIf_ExitSleepMode(void);
static void LpmIf_EnterStopMode(void);
static void LpmIf_ExitStopMode(void);
static void LpmIf_EnterOffMode(void);
static void LpmIf_ExitOffMode(void);

const struct UTIL_LPM_Driver_s UTIL_PowerDriver = {
    LpmIf_EnterSleepMode,
    LpmIf_ExitSleepMode,
    LpmIf_EnterStopMode,
    LpmIf_ExitStopMode,
    LpmIf_EnterOffMode,
    LpmIf_ExitOffMode,
};

static void LpmIf_EnterSleepMode(void)
{
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    PWR_EnterIDLEMode(DISABLE, PWR_IDLEENTRY_WFI);
}

static void LpmIf_ExitSleepMode(void)
{
}

static void LpmIf_EnterStopMode(void)
{
    uint32_t reg_rt_0;

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);

    /* Cleanly shut down the trace UART before the Sleep-mode transition --
     * see file header / trace_if.c for why this matters. */
    TraceIf_PreSleepDeinit();

    /* entry_sleep()'s exact sequence (ns_sleep.c), single-WFI form (no
     * SLEEP_LP_TIMER_ENABLE) -- see file header for why this is
     * reproduced directly rather than composed from PWR_EnterSLEEPMode(). */
    reg_rt_0 = REG32(0x40011004);
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR;
    REG32(0x40028030) |= 0x07;
    REG32(0x40011004)  = 0x00;
    EXTI_PA11_Configuration();

    /* Re-arm BEFORE sleep, not just in LpmIf_ExitStopMode() -- confirmed
     * against app_ble_cellular_wmbus's proven st_lpm_if.c (~1.6 uA,
     * hardware-tested): EXTI_PA11_Configuration() is a BLE ROM function
     * that WRITES EXTI->IMASK directly (no bitwise-OR), wiping every bit
     * except the BLE PA11 wakeup source. Re-arming only after wake (as this
     * function previously did) leaves EXTI_LINE9 (RTC WUT) and our button
     * lines completely unarmed for the ENTIRE upcoming __WFI() -- during
     * actual sleep, only a genuine BLE radio event can wake the MCU at all.
     * The periodic task's 5 s heartbeat kept working regardless, but only
     * because pending RTC WUT events get serviced retroactively on the next
     * incidental (BLE-driven) wake, not because RTC WUT itself ever woke
     * anything -- confirmed harmless in outcome but not structurally
     * correct. Re-arming here, before __WFI(), makes every wake source
     * genuinely live during the sleep itself, matching the reference. */
    EXTI->IMASK |= (EXTI_LINE1 | EXTI_LINE3 | EXTI_LINE9);

    while (PWR->CR1 & PWR_CR1_OSC_EN)
    {
    }

    PWR->CR1  = 0x0A;
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    __WFI();
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);

    RCC->LSCTRL |= 1;
    RCC->CFG    |= RCC_HCLK_DIV2;
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR;
    RCC->APB2PCLKEN |= RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO;
    REG32(0x40011004) = reg_rt_0;

    PWR->CR2 |= 0x100;
    while (!(PWR->CR1 & PWR_CR1_OSC_EN))
    {
    }
}

static void LpmIf_ExitStopMode(void)
{
    /* Re-arm every EXTI line this project uses as a wakeup source -- see
     * file header. EXTI_LINE1 = PB1 button (gpio.c), EXTI_LINE3 = PB3 wake
     * button (gpio.c), EXTI_LINE9 = RTC WUT (timer_if.c). */
    EXTI->IMASK |= (EXTI_LINE1 | EXTI_LINE3 | EXTI_LINE9);

    /* Re-arm the same three IRQs ns_ble_stack_vtor_init() (ns_ble.c) enables
     * once at boot -- same class of bug as the EXTI->IMASK re-arm above
     * (this Sleep-mode tier wipes peripheral-register state on every wake,
     * confirmed hardware behavior, not just EXTI). Without this,
     * BLE_SLP_IRQn's handler (rwip_slp_isr(), rwip_driver.c) -- the only
     * call site for rwip_wakeup(), which resumes BLE's internal scheduler
     * after a real sleep -- never runs again after the first StopMode
     * cycle: the low-level PWR/BLE_BB handshake completes fine (no hang),
     * but BLE's own scheduler stays parked forever, so advertising goes
     * silent even though the MCU itself keeps running normally. */
    NVIC_EnableIRQ(BLE_FIFO_IRQn);
    NVIC_EnableIRQ(BLE_SLP_IRQn);
    NVIC_EnableIRQ(EXTI4_12_IRQn);

    /* Symmetric with TraceIf_PreSleepDeinit() -- fully re-init the trace
     * UART and force-complete any transmission that was genuinely in
     * flight across the transition. See trace_if.c. */
    TraceIf_WakeReinit();

    /* Force-release CFG_LPM_UART_TX_Id unconditionally on every wake --
     * matches app_ble_cellular_wmbus bug 51 exactly (CLAUDE.md), now
     * confirmed reproducible here too: if a trace UART TX starts
     * (PreSendHook sets this voter DISABLE) right as this function's
     * caller is mid-transition into StopMode, __WFI() gates the APB1
     * clock before that specific transfer's TC interrupt can fire --
     * PostSendHook then never runs, and the voter stays DISABLE forever
     * (observed on hardware: StopMode worked for ~30 s, then silently
     * never re-entered again, current settling at the ~1 mA WFI-only
     * plateau). Safe here regardless -- TraceIf_WakeReinit() just
     * force-completed/reset the UART above, so no genuine in-flight
     * transfer can still be pending by this point. */
    UTIL_LPM_SetStopMode((1U << CFG_LPM_UART_TX_Id), UTIL_LPM_ENABLE);
}

static void LpmIf_EnterOffMode(void)
{
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    PWR_EnterPDMode(PWR_PDENTRY_WFI);
}

static void LpmIf_ExitOffMode(void)
{
}
