/**
 * @file gpio.c
 * @brief Board GPIO pin init (button on PB1).
 */

#include "gpio.h"

void APP_GPIO_ButtonInit(void)
{
    GPIO_InitType GPIO_InitStructure = {0};
    EXTI_InitType EXTI_InitStructure = {0};
    NVIC_InitType NVIC_InitStructure = {0};

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStructure.Pin         = APP_BUTTON_PIN;
    GPIO_InitStructure.GPIO_Pull   = GPIO_PULL_UP;
    GPIO_InitPeripheral(APP_BUTTON_PORT, &GPIO_InitStructure);

    GPIO_ConfigEXTILine(APP_BUTTON_EXTI_PORT_SOURCE, APP_BUTTON_EXTI_PIN_SOURCE);

    EXTI_InitStructure.EXTI_Line    = APP_BUTTON_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel         = APP_BUTTON_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void APP_GPIO_WakeButtonInit(void)
{
    GPIO_InitType GPIO_InitStructure = {0};
    EXTI_InitType EXTI_InitStructure = {0};
    NVIC_InitType NVIC_InitStructure = {0};

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStructure.Pin         = APP_WAKE_BUTTON_PIN;
    GPIO_InitStructure.GPIO_Pull   = GPIO_NO_PULL;
    GPIO_InitPeripheral(APP_WAKE_BUTTON_PORT, &GPIO_InitStructure);

    GPIO_ConfigEXTILine(APP_WAKE_BUTTON_EXTI_PORT_SOURCE, APP_WAKE_BUTTON_EXTI_PIN_SOURCE);

    EXTI_InitStructure.EXTI_Line    = APP_WAKE_BUTTON_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel         = APP_WAKE_BUTTON_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
