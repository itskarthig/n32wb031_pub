/**
 * @file gpio.h
 * @brief Board GPIO pin init (button on PB1).
 *
 * Pin init only -- the EXTI0_1_IRQHandler ISR lives in n32wb03x_it.c, not
 * here, unlike trace_if.c/timer_if.c's own-file-owns-its-ISR pattern.
 * Constants below are shared with n32wb03x_it.c's EXTI_GetITStatus/
 * EXTI_ClrITPendBit calls.
 *
 * Reference: N32WB03x_SDK_V2.0.0/projects/n32wb03x_EVAL/peripheral/EXTI/
 * KeyInterrupt (GPIO_ConfigEXTILine() AFIO mapping, PB1 -> EXTI_LINE1 ->
 * EXTI0_1_IRQn, confirmed against n32wb03x.h's own EXTI_LINE1 comment:
 * "Connected to the PA2 PA3 PB1").
 */

#ifndef APP_GPIO_H
#define APP_GPIO_H

#include "n32wb03x.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APP_BUTTON_PORT             GPIOB
#define APP_BUTTON_PIN              GPIO_PIN_1
#define APP_BUTTON_EXTI_LINE        EXTI_LINE1
#define APP_BUTTON_EXTI_PORT_SOURCE GPIOB_PORT_SOURCE
#define APP_BUTTON_EXTI_PIN_SOURCE  GPIO_PIN_SOURCE1
#define APP_BUTTON_IRQn             EXTI0_1_IRQn

/**
 * @brief Configure PB1 as an active-low (pull-up, falling-edge) EXTI button
 *        input, with its NVIC interrupt enabled.
 */
void APP_GPIO_ButtonInit(void);

/* PB3 wake button -- externally pulled to GND; press pulls it HIGH (opposite
 * wiring of PB1's active-low button above). Rising-edge EXTI wakes the MCU
 * from STOP and (via application.c's wake-window task) holds it awake for
 * 60 s so a debugger can attach and reflash -- see CLAUDE.md LPM section. */
#define APP_WAKE_BUTTON_PORT             GPIOB
#define APP_WAKE_BUTTON_PIN              GPIO_PIN_3
#define APP_WAKE_BUTTON_EXTI_LINE        EXTI_LINE3
#define APP_WAKE_BUTTON_EXTI_PORT_SOURCE GPIOB_PORT_SOURCE
#define APP_WAKE_BUTTON_EXTI_PIN_SOURCE  GPIO_PIN_SOURCE3
#define APP_WAKE_BUTTON_IRQn             EXTI2_3_IRQn

/**
 * @brief Configure PB3 as a floating (external-pulldown), rising-edge EXTI
 *        wake button input, with its NVIC interrupt enabled.
 */
void APP_GPIO_WakeButtonInit(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_GPIO_H */
