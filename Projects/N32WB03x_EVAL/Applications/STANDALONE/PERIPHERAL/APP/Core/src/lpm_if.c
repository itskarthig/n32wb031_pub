/**
 * @file lpm_if.c
 * @brief Project-local Low Power Manager hardware driver for PERIPHERAL/APP.
 *
 * See lpm_if.h for the tier-mapping rationale. Each Enter* function enables
 * the PWR APB1 clock first, matching both vendor reference projects
 * (peripheral/PWR/SLEEP and peripheral/PWR/PD main.c).
 *
 * LpmIf_EnterStopMode() uses the SDK's PWR_EnterSLEEPMode() (User Manual
 * S3.3.4 "Sleep mode"). An earlier version of this file avoided that
 * function, believing its internal BLE-oscillator handshake required an
 * initialized BLE stack -- that premise was based on reading ns_sleep.c
 * (the BLE *library's own* internal sleep routine, which genuinely does
 * need a BLE-ROM function). The plain SDK function is a different thing:
 * confirmed via the vendor's own non-BLE reference
 * (N32WB03x_SDK_V2.0.0/projects/n32wb03x_EVAL/application/peripheral_alone)
 * that it works standalone with zero BLE linkage, provided the active
 * trace UART is cleanly deinitialized first -- see TraceIf_PreSleepDeinit()
 * below and CLAUDE.md's BEACON/APP bugs for the full history.
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
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);

    /* Cleanly shut down the trace UART before the Sleep-mode transition --
     * see file header / trace_if.c for why this matters. */
    TraceIf_PreSleepDeinit();

    PWR_EnterSLEEPMode(PWR_SLEEPENTRY_WFI);
}

static void LpmIf_ExitStopMode(void)
{
    /* Re-arm every EXTI line this project uses as a wakeup source -- see
     * file header. EXTI_LINE1 = PB1 button (gpio.c), EXTI_LINE3 = PB3 wake
     * button (gpio.c), EXTI_LINE9 = RTC WUT (timer_if.c). */
    EXTI->IMASK |= (EXTI_LINE1 | EXTI_LINE3 | EXTI_LINE9);

    /* Symmetric with TraceIf_PreSleepDeinit() -- fully re-init the trace
     * UART and force-complete any transmission that was genuinely in
     * flight across the transition. See trace_if.c. */
    TraceIf_WakeReinit();
}

static void LpmIf_EnterOffMode(void)
{
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    PWR_EnterPDMode(PWR_PDENTRY_WFI);
}

static void LpmIf_ExitOffMode(void)
{
}
