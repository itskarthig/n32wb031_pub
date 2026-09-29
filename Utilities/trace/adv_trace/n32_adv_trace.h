/**
 * @file n32_adv_trace.h
 * @brief Generic ring-buffer trace/logging engine for N32WB031.
 *
 * Hardware-agnostic: decoupled from any UART/LPUART peripheral via the
 * UTIL_ADV_TRACE_Driver_s function-pointer table (extern UTIL_TraceDriver),
 * which the board-specific trace_if.c/.h is responsible for populating.
 */

#ifndef N32_ADV_TRACE_H
#define N32_ADV_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "utilities_conf.h"

/* Exported types ------------------------------------------------------------*/

/**
 *  @brief prototype of the time stamp function.
 */
typedef void cb_timestamp(uint8_t *pData, uint16_t *Size);

/**
 *  @brief prototype of the overrun function.
 */
typedef void cb_overrun(uint8_t **pData, uint16_t *size);

/**
 *  @brief  List the Advanced trace function status.
 *  Any negative value corresponds to an error.
 */
typedef enum {
    UTIL_ADV_TRACE_OK              =  0,     /*!< Operation terminated successfully.*/
    UTIL_ADV_TRACE_INVALID_PARAM   = -1,     /*!< Invalid Parameter.                */
    UTIL_ADV_TRACE_HW_ERROR        = -2,     /*!< Hardware Error.                   */
    UTIL_ADV_TRACE_MEM_FULL        = -3,     /*!< Memory fifo full.                 */
    UTIL_ADV_TRACE_UNKNOWN_ERROR   = -4,     /*!< Unknown Error.                    */
#if defined(UTIL_ADV_TRACE_CONDITIONNAL)
    UTIL_ADV_TRACE_GIVEUP          = -5,     /*!< trace give up                     */
    UTIL_ADV_TRACE_REGIONMASKED    = -6      /*!< trace region masked               */
#endif
} UTIL_ADV_TRACE_Status_t;

/**
 * @brief Advanced trace driver definition -- populated by trace_if.c
 */
typedef struct {
    UTIL_ADV_TRACE_Status_t (* Init)(void (*cb)(void *ptr));                                       /*!< Media initialization.      */
    UTIL_ADV_TRACE_Status_t (* DeInit)(void);                                                       /*!< Media Un-initialization.   */
    UTIL_ADV_TRACE_Status_t (* StartRx)(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error));  /*!< Media to start RX process. */
    UTIL_ADV_TRACE_Status_t (* Send)(uint8_t *pdata, uint16_t size);                                /*!< Media to send data.        */
} UTIL_ADV_TRACE_Driver_s;

/* External variables --------------------------------------------------------*/

/**
 *  @brief This structure links the engine to the trace_if.c hardware driver.
 */
extern const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver;

/* Exported functions ------------------------------------------------------- */

/** @brief Initializes the trace engine and the underlying hardware driver. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_Init(void);

/** @brief De-initializes the trace engine and the underlying hardware driver. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_DeInit(void);

/** @brief Returns 1 if the ring buffer is empty, else 0. */
uint8_t UTIL_ADV_TRACE_IsBufferEmpty(void);

/**
 * @brief Number of messages silently dropped so far because the trace FIFO
 * didn't have room for them at the moment they were sent (TRACE_AllocateBufer()
 * returning -1 -- UTIL_ADV_TRACE_MEM_FULL to the caller). Purely diagnostic;
 * reset to 0 by UTIL_ADV_TRACE_Init(). A nonzero, growing value means real
 * log lines are being lost, invisibly, at the call site (every APP_LOG/
 * NS_LOG_DEBUG caller in this codebase discards the MEM_FULL return code) --
 * see CLAUDE.md LORAWAN section, "[APP_DemoTask] log lines silently missing"
 * investigation, for the burst-overflow scenario this was added to detect.
 */
uint32_t UTIL_ADV_TRACE_GetDropCount(void);

/** @brief Starts the RX process via the hardware driver. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_StartRxProcess(void (*UserCallback)(uint8_t *PData, uint16_t Size, uint8_t Error));

/** @brief Formats strFormat and posts it to the ring buffer for TX. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_FSend(const char *strFormat, ...);

/** @brief Posts raw data to the ring buffer for TX. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_Send(const uint8_t *pdata, uint16_t length);

/** @brief Zero-copy allocation: reserve space in the ring buffer to write directly into it. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_ZCSend_Allocation(uint16_t Length, uint8_t **pData, uint16_t *FifoSize, uint16_t *WritePos);

/** @brief Zero-copy finalize: commit a prior ZCSend_Allocation for TX. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_ZCSend_Finalize(void);

/** @brief Weak hook invoked when a TX burst starts (e.g. to block low-power STOP). */
void UTIL_ADV_TRACE_PreSendHook(void);

/** @brief Weak hook invoked when the TX burst fully drains. */
void UTIL_ADV_TRACE_PostSendHook(void);

#if defined(UTIL_ADV_TRACE_OVERRUN)
/** @brief Register a function used to add overrun info inside the trace. */
void UTIL_ADV_TRACE_RegisterOverRunFunction(cb_overrun *cb);
#endif

#if defined(UTIL_ADV_TRACE_CONDITIONNAL)

/** @brief Conditional FSend gated by VerboseLevel/Region, with optional timestamp. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_FSend(uint32_t VerboseLevel, uint32_t Region, uint32_t TimeStampState, const char *strFormat, ...);

/** @brief Conditional zero-copy allocation, gated by VerboseLevel/Region. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_ZCSend_Allocation(uint32_t VerboseLevel, uint32_t Region, uint32_t TimeStampState, uint16_t length, uint8_t **pData, uint16_t *FifoSize, uint16_t *WritePos);

/** @brief Conditional zero-copy finalize. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_ZCSend_Finalize(void);

/** @brief Conditional Send gated by VerboseLevel/Region, with optional timestamp. */
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_Send(uint32_t VerboseLevel, uint32_t Region, uint32_t TimeStampState, const uint8_t *pdata, uint16_t length);

/** @brief Register a function used to add a timestamp prefix inside the trace. */
void UTIL_ADV_TRACE_RegisterTimeStampFunction(cb_timestamp *cb);

/** @brief Set the current verbose level (0-255). */
void UTIL_ADV_TRACE_SetVerboseLevel(uint8_t Level);

/** @brief Get the current verbose level. */
uint8_t UTIL_ADV_TRACE_GetVerboseLevel(void);

/** @brief OR a bit field into the enabled region mask. */
void UTIL_ADV_TRACE_SetRegion(uint32_t Region);

/** @brief Get the current region mask. */
uint32_t UTIL_ADV_TRACE_GetRegion(void);

/** @brief Clear a bit field from the enabled region mask. */
void UTIL_ADV_TRACE_ResetRegion(uint32_t Region);

#endif /* UTIL_ADV_TRACE_CONDITIONNAL */

#ifdef __cplusplus
}
#endif

#endif /* N32_ADV_TRACE_H */
