/**
 * @file iwdg_if.c
 * @brief Independent hardware watchdog -- see iwdg_if.h for the why and the switches.
 *
 * Facts this relies on (N32WB03x User Manual ch.12, SDK n32wb03x_iwdg.c):
 *  - clock is LSI; starting the IWDG forces LSI on and it cannot be turned off again. LSXSEL (the RTC /
 *    BLE low-speed clock selector) is not affected, so RTC and BLE stay on LSE.
 *  - /256 prescaler. Time base: LSI is nominally 32 kHz (the vendor IWDG_Reset example's own comment: DIV128
 *    + reload 75 = 300 ms; measured here: 32032 Hz) => 8 ms per count, reload 0xFFF = 32760 ms. The manual's
 *    Table 12-1 (6.4 ms per count, 26214 ms) only holds for a 40 kHz clock and is NOT used. LSI is an RC
 *    oscillator, so treat the timeout as nominal (a few tens of percent of tolerance is normal).
 *  - IWDG_PREDIV / IWDG_RELV are write protected: write 0x5555 to KEY first, then wait for STS.PVU /
 *    STS.CRVU to clear before the first reload.
 *  - RCC->DBGMCU_CR DBG_IWDG_STOP freezes the counter while the core is halted by SWD, so a probe
 *    halt does not reset the board.
 *  - PWR_CTRL2.IWDGRSTEN defaults to 1 (counter reaching 0 resets the chip). No SDK macro exists for it,
 *    so the default is relied upon and proven by the self-test / a forced-hang test.
 *
 * Unknown until measured on hardware: whether the IWDG keeps counting while the core is in deep Sleep
 * (UM 3.3: CORE powered off; UM 4.2.6: LSI clocks the IWDG in Sleep; the vendor re-runs its IWDG config
 * after every wake). IWDG_SELFTEST answers it.
 */

#include "iwdg_if.h"

#if (IWDG_ENABLE == 1)

#include <stdbool.h>
#include <stdint.h>

#include "n32wb03x.h"
#include "n32wb03x_iwdg.h"
#include "n32wb03x_rcc.h"
#include "app_log.h"

/* /256 at a nominal 32 kHz LSI = 8 ms per count; 4095 counts max. */
#define IWDG_MS_PER_COUNT    8U
#define IWDG_MAX_TIMEOUT_MS  (4095U * IWDG_MS_PER_COUNT)
#define IWDG_PVU_CRVU_WAIT   200000U /* bounded wait for the LSI-domain register update */

static bool s_prev_reset_was_iwdg = false;

static uint16_t iwdg_reload_for_ms(uint32_t ms)
{
    if (ms >= IWDG_MAX_TIMEOUT_MS)
    {
        return 0xFFFU;
    }
    uint32_t counts = (ms + IWDG_MS_PER_COUNT - 1U) / IWDG_MS_PER_COUNT;
    if (counts == 0U)
    {
        counts = 1U;
    }
    return (uint16_t)counts;
}

void IwdgIf_CaptureResetCause(void)
{
    /* Same condition the vendor uses in ns_peripheral_config(): IWDG flag set and no power-on reset. */
    s_prev_reset_was_iwdg =
        ((RCC->CTRLSTS & (RCC_CTRLSTS_PORRSTF | RCC_CTRLSTS_IWDGRSTF)) == RCC_CTRLSTS_IWDGRSTF);
}

void IwdgIf_Init(void)
{
    if (s_prev_reset_was_iwdg)
    {
        APP_LOG(TS_OFF, VLEVEL_M, "[IWDG] previous reset was caused by the watchdog\r\n");
    }

    const uint16_t reload = iwdg_reload_for_ms((uint32_t)IWDG_TIMEOUT_MS);

    RCC->DBGMCU_CR |= DBG_IWDG_STOP;

    IWDG_WriteConfig(IWDG_WRITE_ENABLE);
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV256);
    IWDG_CntReload(reload);

    /* Same order as the vendor IWDG_Reset example. The PVU/CRVU flags can only clear once the LSI-clocked
     * update has run, and LSI is switched on by IWDG_Enable() itself -- so waiting for them BEFORE the enable
     * can never succeed (seen on hardware: spurious "wait timed out"). Wait after the enable instead. */
    IWDG_ReloadKey();
    IWDG_Enable();

    uint32_t n = IWDG_PVU_CRVU_WAIT;
    while ((IWDG_GetStatus(IWDG_PVU_FLAG) != RESET || IWDG_GetStatus(IWDG_CRVU_FLAG) != RESET) && (--n != 0U))
    {
    }
    IWDG_ReloadKey(); /* start the first full period from the now-applied reload value */

    APP_LOG(TS_OFF, VLEVEL_M, "[IWDG] enabled, timeout=%lu ms (reload=0x%03X)%s\r\n",
            (unsigned long)((uint32_t)reload * IWDG_MS_PER_COUNT), (unsigned)reload,
            (n == 0U) ? " WARNING: PVU/CRVU wait timed out" : "");
}

void IwdgIf_Feed(void)
{
    IWDG_ReloadKey();
}

#if (IWDG_SELFTEST == 1)

#include "n32_timer.h"
#include "trace_if.h"

extern void EXTI_PA11_Configuration(void); /* ROM, see lpm_if.c */

#define REG32(addr)               (*(volatile uint32_t *)(addr))
#define IWDG_SELFTEST_DELAY_MS    30000U
#define BB_DEEPSLCNTL             0x40028030U
#define BB_DEEPSLWKUP             0x40028034U

/**
 * Deep-sleep with the BB wake timer pushed out to ~36 h and PB3 (EXTI3) as the ONLY wake source, with the
 * pump (the only feeder) not running:
 *   - IWDG works in deep Sleep     -> the board resets about IWDG_TIMEOUT_MS later (next boot logs
 *                                      "[IWDG] previous reset was caused by the watchdog", current rises).
 *   - IWDG does NOT run in Sleep   -> the board stays at ~13 uA until PB3 is pressed.
 * After a PB3 wake the BB clock correction was never applied (rwip_wakeup() not run), so BLE timing is
 * unreliable: RESET THE BOARD after the experiment. One-shot, default off.
 */
void IwdgIf_SelfTestPoll(void)
{
    static bool started = false;
    static bool done = false;
    static UTIL_TIMER_Time_t t0;

    if (done)
    {
        return;
    }
    if (!started)
    {
        started = true;
        t0 = UTIL_TIMER_GetCurrentTime();
        return;
    }
    if (UTIL_TIMER_GetElapsedTime(t0) < IWDG_SELFTEST_DELAY_MS)
    {
        return;
    }
    done = true;

    APP_LOG(TS_OFF, VLEVEL_M, "[IWDG-TEST] entering deep Sleep, wake = PB3 only; expect a watchdog reset in ~%lu ms if the IWDG runs in Sleep\r\n",
            (unsigned long)IWDG_TIMEOUT_MS);
    TraceIf_Flush();

    uint32_t primask;
    __asm volatile("mrs %0, primask\n\tcpsid i" : "=r"(primask) : : "memory");

    const uint32_t imask_saved = EXTI->IMASK;
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR;
    const uint32_t reg_rt_0 = REG32(0x40011004);

    REG32(BB_DEEPSLWKUP) = 0xFFFFFFFFU; /* no BB wake for ~36 h */
    REG32(BB_DEEPSLCNTL) |= 0x07;       /* BB sleep request */

    TraceIf_PreSleepDeinit();
    REG32(0x40011004) = 0x00;
    EXTI_PA11_Configuration();
    EXTI->IMASK = EXTI_LINE3; /* PB3 only: no RTC (line 9), no DIO1 (line 0), no BB (line 11) */

    while (PWR->CR1 & PWR_CR1_OSC_EN)
    {
    }

    PWR->CR1  = 0x0A;
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    __WFI();
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);

    /* Only reached if PB3 woke us, i.e. the IWDG did NOT reset the chip. Mirror lpm_if.c's restore. */
    RCC->LSCTRL |= 1;
    RCC->CFG    |= RCC_HCLK_DIV2;
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR;
    RCC->APB2PCLKEN |= RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO;
    REG32(0x40011004) = reg_rt_0;
    PWR->CR2 |= 0x100;
    while (!(PWR->CR1 & PWR_CR1_OSC_EN))
    {
    }
    EXTI->IMASK = imask_saved | EXTI_LINE3;
    TraceIf_WakeReinit();

    __asm volatile("msr primask, %0" : : "r"(primask) : "memory");

    APP_LOG(TS_OFF, VLEVEL_M, "[IWDG-TEST] woken by PB3 with NO watchdog reset: the IWDG does NOT run in deep Sleep. Reset the board.\r\n");
}

#endif /* IWDG_SELFTEST */

#endif /* IWDG_ENABLE */
