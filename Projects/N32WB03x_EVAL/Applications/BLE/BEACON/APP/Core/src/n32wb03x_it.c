/**
 * @file n32wb03x_it.c
 * @brief Interrupt handlers not co-located with their own driver file.
 *
 * trace_if.c/timer_if.c each define their own ISR directly; the PB1 button
 * and PB3 wake-button EXTI handlers live here instead (deliberate exception
 * -- see CLAUDE.md BEACON/APP section).
 */

#include "n32wb03x.h"
#include "n32_seq.h"
#include "app_config.h"
#include "gpio.h"

void EXTI0_1_IRQHandler(void)
{
    if (EXTI_GetITStatus(APP_BUTTON_EXTI_LINE) != RESET)
    {
        EXTI_ClrITPendBit(APP_BUTTON_EXTI_LINE);
        UTIL_SEQ_SetTask((1U << TASK_PULSE_COUNTER_ID), CFG_SEQ_Prio_0);
    }
}

void EXTI2_3_IRQHandler(void)
{
    if (EXTI_GetITStatus(APP_WAKE_BUTTON_EXTI_LINE) != RESET)
    {
        EXTI_ClrITPendBit(APP_WAKE_BUTTON_EXTI_LINE);
        UTIL_SEQ_SetTask((1U << TASK_WAKE_WINDOW_ID), CFG_SEQ_Prio_0);
    }
}
