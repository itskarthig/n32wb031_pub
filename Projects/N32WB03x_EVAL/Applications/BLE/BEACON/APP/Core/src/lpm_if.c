/**
 * @file lpm_if.c
 * @brief Project-local Low Power Manager hardware driver.
 *
 * LpmIf_EnterStopMode() reproduces the vendor's entry_sleep() (ns_library/sleep/ns_sleep.c, non-LPT path)
 * with this board's peripheral handling added around it. The BB sleep request is the FIRST action (as in the
 * vendor), peripheral deinit follows it. APP_RTC_ResyncAfterWake() runs after the wake, in
 * LpmIf_ExitStopMode(). Both PWR_CR1.BLE_OSC_EN waits are unbounded, exactly as in the vendor; the optional
 * IWDG (app_config.h, IWDG_ENABLE) is the safety net for an awake-CPU hang there.
 * Ported from LORAWAN_WMBUS_AMR_V20 (87 h clean run) on 2026-11-17, see CLAUDE.md.
 */

#include <stdbool.h>

#include "lpm_if.h"
#include "n32_lpm.h"
#include "n32wb03x.h"
#include "trace_if.h"
#include "rtc.h"        /* APP_RTC_ResyncAfterWake() -- see LpmIf_ExitStopMode() */
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
static volatile uint32_t s_stop_enter_count = 0U;
uint32_t LpmIf_GetStopEnterCount(void) { return s_stop_enter_count; }

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

    s_stop_enter_count++;

    /* entry_sleep() (ns_sleep.c), single-WFI form. The BB sleep request is the FIRST action, as in the
     * vendor: rwip_sleep() has already read the BB clock and programmed the wake-up, so every
     * microsecond between that call and this write eats the ~0.94 ms (3 half-slots) margin of the
     * wake compare. Our peripheral deinit therefore runs AFTER the request, while the BB is already
     * going down (the vendor does NS_LOG_DEINIT() at the same point, right before the WFI). */
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR;
    reg_rt_0 = REG32(0x40011004);
    REG32(0x40028030) |= 0x07; /* BB sleep request */

    TraceIf_PreSleepDeinit();

    REG32(0x40011004)  = 0x00;
    EXTI_PA11_Configuration();
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

    PWR->CR2 |= 0x100; /* BB wake request */
    while (!(PWR->CR1 & PWR_CR1_OSC_EN))
    {
    }
}

/* entry_idle() (ns_sleep.c): CPU sleep with the BB kept running. Called with PRIMASK=1 (a pending
 * interrupt still wakes the WFI, then runs when the caller restores PRIMASK). */
void LpmIf_EntryIdle(void)
{
    EXTI_PA11_Configuration();
    PWR->CR1 &= 0xF0;
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);
    __WFI();
}

static void LpmIf_ExitStopMode(void)
{
    EXTI->IMASK |= (EXTI_LINE1 | EXTI_LINE3 | EXTI_LINE9);

    APP_RTC_ResyncAfterWake(); /* UM S20.3.11: calendar shadow registers are stale after Sleep */

    NVIC_EnableIRQ(BLE_FIFO_IRQn);
    NVIC_EnableIRQ(BLE_SLP_IRQn);
    NVIC_EnableIRQ(EXTI4_12_IRQn);

    TraceIf_WakeReinit();
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
