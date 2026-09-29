/**
 * @file adc.h
 * @brief Internal-channel ADC readings (supply voltage, die temperature).
 *
 * Project-local, like rtc.c/gpio.c -- pure N32-specific hardware code, no
 * STM32 Utilities/ counterpart exists for this. Reads the two internal-only
 * ADC channels (n32wb03x.h): ADC_CTRL_CH_6 (trim-calibrated supply/VREF
 * voltage) and ADC_CTRL_CH_7 (die temperature, requires ADC_EnableTS()).
 * Neither needs an external GPIO pin -- confirmed against every N32 SDK ADC
 * reference example, which only configure a pin for the *external* channel
 * they use.
 */

#ifndef APP_ADC_H
#define APP_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Sentinel returned by APP_ADC_ReadVrefMv() on an ADC_FLAG_DONE timeout. */
#define APP_ADC_INVALID_MV   (-1)

/** Sentinel returned by APP_ADC_ReadTemperatureC() on an ADC_FLAG_DONE timeout. */
#define APP_ADC_INVALID_TEMP_C (-1000)

/**
 * @brief (Re-)initialize the ADC peripheral and its clock. Idempotent --
 * safe to call before every read, which is exactly what
 * APP_ADC_ReadVrefMv()/APP_ADC_ReadTemperatureC() do, so a read is always
 * correct regardless of what happened to the peripheral since the last one
 * (including a Sleep-mode cycle -- see the file header in lpm_if.c).
 */
void APP_ADC_Init(void);

/**
 * @brief Read the internal supply/VREF channel (ADC_CTRL_CH_6),
 * trim-calibrated by the SDK's own ADC_ConverValueToVoltage().
 * @return Voltage in mV, or APP_ADC_INVALID_MV on a conversion timeout.
 */
int32_t APP_ADC_ReadVrefMv(void);

/**
 * @brief Read the internal die-temperature channel (ADC_CTRL_CH_7),
 * trim-calibrated by the SDK's own ADC_ConverValueToTemperature().
 * @return Temperature in whole degrees C, or APP_ADC_INVALID_TEMP_C on a
 * conversion timeout.
 */
int32_t APP_ADC_ReadTemperatureC(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_ADC_H */
