/**
 * @file adc.c
 * @brief Internal-channel ADC readings (supply voltage, die temperature).
 *
 * Register-level sequence confirmed against three genuine N32 SDK
 * references (N32WB03x_SDK_V2.0.0/projects/n32wb03x_EVAL/):
 *   - application/peripheral_alone/src/app_adc.c -- RCC/ADC init shape
 *   - peripheral/ADC/ADC_SingleRead/src/main.c -- blocking single-read
 *     sequence (ConfigChannel -> Enable -> poll DONE -> ClearFlag -> GetDat)
 *   - peripheral/ADC/ADC_Temperature/src/main.c -- ADC_EnableTS() +
 *     ADC_ConverValueToTemperature() for the internal temperature channel
 *
 * ADC_CTRL_CH_6 and ADC_CTRL_CH_7 are the two internal-only channels
 * (n32wb03x.h) -- neither needs a GPIO pin configured, unlike the external
 * channels (CH_1..CH_5) every reference example also exercises.
 */

#include "adc.h"
#include "n32wb03x.h"

/** Bounded spin count for the ADC_FLAG_DONE poll -- conversion completes in
 * low microseconds on real hardware; this is a generous margin so a genuine
 * hardware fault can't hang the caller (APP_PeriodicTask()). */
#define ADC_DONE_TIMEOUT_LOOPS (0xFFFFU)

void APP_ADC_Init(void)
{
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_ADC, ENABLE);
    RCC_ConfigAdcClk(RCC_ADCCLK_SRC_AUDIOPLL);
    RCC_Enable_ADC_CLK_SRC_AUDIOPLL(ENABLE);

    ADC_DeInit(ADC);
    ADC_EnableBypassFilter(ADC, ENABLE);
}

/**
 * @brief Blocking single-shot read of one ADC channel.
 * @param channel ADC_CTRL_CH_x.
 * @param raw_out Receives the 12-bit conversion result.
 * @return 1 on success, 0 on an ADC_FLAG_DONE timeout.
 */
static uint8_t AdcIf_ReadChannelBlocking(uint16_t channel, uint16_t *raw_out)
{
    uint32_t timeout = ADC_DONE_TIMEOUT_LOOPS;

    ADC_ConfigChannel(ADC, channel);
    ADC_Enable(ADC, ENABLE);

    while (ADC_GetFlagStatus(ADC, ADC_FLAG_DONE) == RESET)
    {
        timeout--;
        if (timeout == 0U)
        {
            return 0U;
        }
    }
    ADC_ClearFlag(ADC, ADC_FLAG_DONE);
    *raw_out = ADC_GetDat(ADC);

    return 1U;
}

int32_t APP_ADC_ReadVrefMv(void)
{
    uint16_t raw;

    APP_ADC_Init();

    if (AdcIf_ReadChannelBlocking(ADC_CTRL_CH_6, &raw) == 0U)
    {
        return APP_ADC_INVALID_MV;
    }

    return (int32_t)ADC_ConverValueToVoltage(raw, ADC_CTRL_CH_6);
}

int32_t APP_ADC_ReadTemperatureC(void)
{
    uint16_t raw;
    uint8_t ok;

    APP_ADC_Init();

    ADC_EnableTS(ADC, ENABLE);
    ok = AdcIf_ReadChannelBlocking(ADC_CTRL_CH_7, &raw);
    ADC_EnableTS(ADC, DISABLE);

    if (ok == 0U)
    {
        return APP_ADC_INVALID_TEMP_C;
    }

    return (int32_t)ADC_ConverValueToTemperature(raw);
}
