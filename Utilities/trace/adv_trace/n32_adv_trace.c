/**
 * @file n32_adv_trace.c
 * @brief Generic ring-buffer trace/logging engine for N32WB031.
 *
 * Ring-buffer/locking logging engine, decoupled from hardware via
 * UTIL_TraceDriver (see trace_if.c). UTIL_ADV_TRACE_UNCHUNK_MODE and
 * UTIL_ADV_TRACE_OVERRUN are intentionally not used by this project's
 * utilities_conf.h, so those optional code paths are compiled out.
 */

#include "n32_adv_trace.h"
#include "stdarg.h"
#include "stdio.h"

/* Private defines -----------------------------------------------------------*/

#if !defined(UTIL_ADV_TRACE_MEMLOCATION)
#define UTIL_ADV_TRACE_MEMLOCATION
#endif

#ifndef UTIL_ADV_TRACE_DEBUG
#define UTIL_ADV_TRACE_DEBUG(...)
#endif

/* Private typedef -----------------------------------------------------------*/

/**
 *  @brief  ADV_TRACE_Context -- holds all trace engine state.
 */
typedef struct {
#if defined(UTIL_ADV_TRACE_CONDITIONNAL)
    cb_timestamp *timestamp_func;  /*!< ptr of function used to insert time stamp. */
    uint8_t  CurrentVerboseLevel;  /*!< verbose level used.                        */
    uint32_t RegionMask;           /*!< mask of the enabled region.                */
#endif
    uint16_t TraceRdPtr;    /*!< read pointer of the trace ring buffer.  */
    uint16_t TraceWrPtr;    /*!< write pointer of the trace ring buffer. */
    uint16_t TraceSentSize; /*!< size of the latest transfer.            */
    uint16_t TraceLock;     /*!< lock counter of the trace system.       */
} ADV_TRACE_Context;

/* Private variables ---------------------------------------------------------*/

static ADV_TRACE_Context ADV_TRACE_Ctx;
static UTIL_ADV_TRACE_MEMLOCATION uint8_t ADV_TRACE_Buffer[UTIL_ADV_TRACE_FIFO_SIZE];

/* Count of messages silently dropped by TRACE_AllocateBufer() because the
 * FIFO didn't have room -- see UTIL_ADV_TRACE_GetDropCount()'s own comment
 * in the header. Purely additive/diagnostic; does not change overflow
 * behavior (still a silent drop -- this only makes an already-existing
 * condition visible to a caller that wants to check). Shared across every
 * project that links this engine; zero behavior change for any project
 * that never calls the getter. */
static uint32_t s_trace_drop_count;

/* Private function prototypes ------------------------------------------------*/

static void TRACE_TxCpltCallback(void *Ptr);
static int16_t TRACE_AllocateBufer(uint16_t Size, uint16_t *Pos);
static UTIL_ADV_TRACE_Status_t TRACE_Send(void);

static void TRACE_Lock(void);
static void TRACE_UnLock(void);
static uint32_t TRACE_IsLocked(void);

/* Functions Definition -------------------------------------------------------*/

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_Init(void)
{
    (void)UTIL_ADV_TRACE_MEMSET8(&ADV_TRACE_Ctx, 0x0, sizeof(ADV_TRACE_Context));
    (void)UTIL_ADV_TRACE_MEMSET8(&ADV_TRACE_Buffer, 0x0, sizeof(ADV_TRACE_Buffer));
    s_trace_drop_count = 0U;

    UTIL_ADV_TRACE_INIT_CRITICAL_SECTION();

    return UTIL_TraceDriver.Init(TRACE_TxCpltCallback);
}

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_DeInit(void)
{
    return UTIL_TraceDriver.DeInit();
}

uint8_t UTIL_ADV_TRACE_IsBufferEmpty(void)
{
    if (ADV_TRACE_Ctx.TraceWrPtr == ADV_TRACE_Ctx.TraceRdPtr)
    {
        return 1;
    }
    return 0;
}

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_StartRxProcess(void (*UserCallback)(uint8_t *PData, uint16_t Size, uint8_t Error))
{
    return UTIL_TraceDriver.StartRx(UserCallback);
}

#if defined(UTIL_ADV_TRACE_CONDITIONNAL)
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_FSend(uint32_t VerboseLevel, uint32_t Region, uint32_t TimeStampState, const char *strFormat, ...)
{
    va_list vaArgs;
    uint8_t buf[UTIL_ADV_TRACE_TMP_BUF_SIZE + UTIL_ADV_TRACE_TMP_MAX_TIMESTMAP_SIZE];
    uint16_t buff_size = 0u;

    if (!(ADV_TRACE_Ctx.CurrentVerboseLevel >= VerboseLevel))
    {
        return UTIL_ADV_TRACE_GIVEUP;
    }

    if ((Region & ADV_TRACE_Ctx.RegionMask) != Region)
    {
        return UTIL_ADV_TRACE_REGIONMASKED;
    }

    if ((ADV_TRACE_Ctx.timestamp_func != NULL) && (TimeStampState != 0u))
    {
        ADV_TRACE_Ctx.timestamp_func(buf, &buff_size);
    }

    va_start(vaArgs, strFormat);
    buff_size += (uint16_t)UTIL_ADV_TRACE_VSNPRINTF((char *)(buf + buff_size), UTIL_ADV_TRACE_TMP_BUF_SIZE, strFormat, vaArgs);
    va_end(vaArgs);

    return UTIL_ADV_TRACE_Send(buf, buff_size);
}
#endif

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_FSend(const char *strFormat, ...)
{
    uint8_t buf[UTIL_ADV_TRACE_TMP_BUF_SIZE];
    va_list vaArgs;

    va_start(vaArgs, strFormat);
    uint16_t bufSize = (uint16_t)UTIL_ADV_TRACE_VSNPRINTF((char *)buf, UTIL_ADV_TRACE_TMP_BUF_SIZE, strFormat, vaArgs);
    va_end(vaArgs);

    return UTIL_ADV_TRACE_Send(buf, bufSize);
}

#if defined(UTIL_ADV_TRACE_CONDITIONNAL)
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_ZCSend_Allocation(uint32_t VerboseLevel, uint32_t Region, uint32_t TimeStampState, uint16_t length, uint8_t **pData, uint16_t *FifoSize, uint16_t *WritePos)
{
    UTIL_ADV_TRACE_Status_t ret = UTIL_ADV_TRACE_OK;
    uint16_t writepos;
    uint8_t timestamp_ptr[UTIL_ADV_TRACE_TMP_MAX_TIMESTMAP_SIZE];
    uint16_t timestamp_size = 0u;

    if (!(ADV_TRACE_Ctx.CurrentVerboseLevel >= VerboseLevel))
    {
        return UTIL_ADV_TRACE_GIVEUP;
    }

    if ((Region & ADV_TRACE_Ctx.RegionMask) != Region)
    {
        return UTIL_ADV_TRACE_REGIONMASKED;
    }

    if ((ADV_TRACE_Ctx.timestamp_func != NULL) && (TimeStampState != 0u))
    {
        ADV_TRACE_Ctx.timestamp_func(timestamp_ptr, &timestamp_size);
    }

    TRACE_Lock();

    if (TRACE_AllocateBufer((uint16_t)(length + timestamp_size), &writepos) != -1)
    {
        for (uint16_t index = 0u; index < timestamp_size; index++)
        {
            ADV_TRACE_Buffer[writepos] = timestamp_ptr[index];
            writepos = (uint16_t)((writepos + 1u) % UTIL_ADV_TRACE_FIFO_SIZE);
        }

        *pData = ADV_TRACE_Buffer;
        *FifoSize = (uint16_t)UTIL_ADV_TRACE_FIFO_SIZE;
        *WritePos = writepos;
    }
    else
    {
        TRACE_UnLock();
        ret = UTIL_ADV_TRACE_MEM_FULL;
    }
    return ret;
}

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_ZCSend_Finalize(void)
{
    return UTIL_ADV_TRACE_ZCSend_Finalize();
}
#endif

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_ZCSend_Allocation(uint16_t Length, uint8_t **pData, uint16_t *FifoSize, uint16_t *WritePos)
{
    UTIL_ADV_TRACE_Status_t ret = UTIL_ADV_TRACE_OK;
    uint16_t writepos;

    TRACE_Lock();

    if (TRACE_AllocateBufer(Length, &writepos) != -1)
    {
        *pData = ADV_TRACE_Buffer;
        *FifoSize = UTIL_ADV_TRACE_FIFO_SIZE;
        *WritePos = writepos;
    }
    else
    {
        TRACE_UnLock();
        ret = UTIL_ADV_TRACE_MEM_FULL;
    }

    return ret;
}

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_ZCSend_Finalize(void)
{
    TRACE_UnLock();
    return TRACE_Send();
}

#if defined(UTIL_ADV_TRACE_CONDITIONNAL)
UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_COND_Send(uint32_t VerboseLevel, uint32_t Region, uint32_t TimeStampState, const uint8_t *pData, uint16_t Length)
{
    UTIL_ADV_TRACE_Status_t ret;
    uint16_t writepos;
    uint32_t idx;
    uint8_t timestamp_ptr[UTIL_ADV_TRACE_TMP_MAX_TIMESTMAP_SIZE];
    uint16_t timestamp_size = 0u;

    if (!(ADV_TRACE_Ctx.CurrentVerboseLevel >= VerboseLevel))
    {
        return UTIL_ADV_TRACE_GIVEUP;
    }

    if ((Region & ADV_TRACE_Ctx.RegionMask) != Region)
    {
        return UTIL_ADV_TRACE_REGIONMASKED;
    }

    if ((ADV_TRACE_Ctx.timestamp_func != NULL) && (TimeStampState != 0u))
    {
        ADV_TRACE_Ctx.timestamp_func(timestamp_ptr, &timestamp_size);
    }

    TRACE_Lock();

    if (TRACE_AllocateBufer((uint16_t)(Length + timestamp_size), &writepos) != -1)
    {
        for (idx = 0; idx < timestamp_size; idx++)
        {
            ADV_TRACE_Buffer[writepos] = timestamp_ptr[idx];
            writepos = (uint16_t)((writepos + 1u) % UTIL_ADV_TRACE_FIFO_SIZE);
        }

        for (idx = 0u; idx < Length; idx++)
        {
            ADV_TRACE_Buffer[writepos] = pData[idx];
            writepos = (uint16_t)((writepos + 1u) % UTIL_ADV_TRACE_FIFO_SIZE);
        }

        TRACE_UnLock();
        ret = TRACE_Send();
    }
    else
    {
        TRACE_UnLock();
        ret = UTIL_ADV_TRACE_MEM_FULL;
    }

    return ret;
}
#endif

UTIL_ADV_TRACE_Status_t UTIL_ADV_TRACE_Send(const uint8_t *pData, uint16_t Length)
{
    UTIL_ADV_TRACE_Status_t ret;
    uint16_t writepos;
    uint32_t idx;

    TRACE_Lock();

    if (TRACE_AllocateBufer(Length, &writepos) != -1)
    {
        for (idx = 0u; idx < Length; idx++)
        {
            ADV_TRACE_Buffer[writepos] = pData[idx];
            writepos = (uint16_t)((writepos + 1u) % UTIL_ADV_TRACE_FIFO_SIZE);
        }
        TRACE_UnLock();

        ret = TRACE_Send();
    }
    else
    {
        TRACE_UnLock();
        ret = UTIL_ADV_TRACE_MEM_FULL;
    }

    return ret;
}

#if defined(UTIL_ADV_TRACE_CONDITIONNAL)
void UTIL_ADV_TRACE_RegisterTimeStampFunction(cb_timestamp *cb)
{
    ADV_TRACE_Ctx.timestamp_func = *cb;
}

void UTIL_ADV_TRACE_SetVerboseLevel(uint8_t Level)
{
    ADV_TRACE_Ctx.CurrentVerboseLevel = Level;
}

uint8_t UTIL_ADV_TRACE_GetVerboseLevel(void)
{
    return ADV_TRACE_Ctx.CurrentVerboseLevel;
}

void UTIL_ADV_TRACE_SetRegion(uint32_t Region)
{
    ADV_TRACE_Ctx.RegionMask |= Region;
}

uint32_t UTIL_ADV_TRACE_GetRegion(void)
{
    return ADV_TRACE_Ctx.RegionMask;
}

void UTIL_ADV_TRACE_ResetRegion(uint32_t Region)
{
    ADV_TRACE_Ctx.RegionMask &= ~Region;
}
#endif

#if defined(__GNUC__)
__attribute__((weak))
#endif
void UTIL_ADV_TRACE_PreSendHook(void)
{
}

#if defined(__GNUC__)
__attribute__((weak))
#endif
void UTIL_ADV_TRACE_PostSendHook(void)
{
}

/* Private function definitions -----------------------------------------------*/

/**
 * @brief send the data currently in the ring buffer to the low layer driver.
 * @retval Status based on @ref UTIL_ADV_TRACE_Status_t
 */
static UTIL_ADV_TRACE_Status_t TRACE_Send(void)
{
    UTIL_ADV_TRACE_Status_t ret = UTIL_ADV_TRACE_OK;
    uint8_t *ptr = NULL;

    UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION();

    if (TRACE_IsLocked() == 0u)
    {
        TRACE_Lock();

        if (ADV_TRACE_Ctx.TraceRdPtr != ADV_TRACE_Ctx.TraceWrPtr)
        {
            if (ADV_TRACE_Ctx.TraceWrPtr > ADV_TRACE_Ctx.TraceRdPtr)
            {
                ADV_TRACE_Ctx.TraceSentSize = ADV_TRACE_Ctx.TraceWrPtr - ADV_TRACE_Ctx.TraceRdPtr;
            }
            else /* TraceRdPtr > TraceWrPtr */
            {
                ADV_TRACE_Ctx.TraceSentSize = UTIL_ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceRdPtr;
            }

            ptr = &ADV_TRACE_Buffer[ADV_TRACE_Ctx.TraceRdPtr];

            UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
            UTIL_ADV_TRACE_PreSendHook();

            UTIL_ADV_TRACE_DEBUG("\n--TRACE_Send(%d-%d)--\n", ADV_TRACE_Ctx.TraceRdPtr, ADV_TRACE_Ctx.TraceSentSize);
            ret = UTIL_TraceDriver.Send(ptr, ADV_TRACE_Ctx.TraceSentSize);
        }
        else
        {
            TRACE_UnLock();
            UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
        }
    }
    else
    {
        UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
    }

    return ret;
}

/**
 * @brief Tx callback invoked by trace_if.c when a chunk transfer completes.
 * @param Ptr unused -- kept for HAL-style callback signature compatibility.
 */
static void TRACE_TxCpltCallback(void *Ptr)
{
    uint8_t *ptr = NULL;
    (void)Ptr;

    UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION();

    ADV_TRACE_Ctx.TraceRdPtr = (ADV_TRACE_Ctx.TraceRdPtr + ADV_TRACE_Ctx.TraceSentSize) % UTIL_ADV_TRACE_FIFO_SIZE;

    if ((ADV_TRACE_Ctx.TraceRdPtr != ADV_TRACE_Ctx.TraceWrPtr) && (1u == ADV_TRACE_Ctx.TraceLock))
    {
        if (ADV_TRACE_Ctx.TraceWrPtr > ADV_TRACE_Ctx.TraceRdPtr)
        {
            ADV_TRACE_Ctx.TraceSentSize = ADV_TRACE_Ctx.TraceWrPtr - ADV_TRACE_Ctx.TraceRdPtr;
        }
        else /* TraceRdPtr > TraceWrPtr */
        {
            ADV_TRACE_Ctx.TraceSentSize = UTIL_ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceRdPtr;
        }
        ptr = &ADV_TRACE_Buffer[ADV_TRACE_Ctx.TraceRdPtr];
        UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
        UTIL_ADV_TRACE_DEBUG("\n--TRACE_Send(%d-%d)--\n", ADV_TRACE_Ctx.TraceRdPtr, ADV_TRACE_Ctx.TraceSentSize);
        UTIL_TraceDriver.Send(ptr, ADV_TRACE_Ctx.TraceSentSize);
    }
    else
    {
        UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
        UTIL_ADV_TRACE_PostSendHook();
        TRACE_UnLock();
    }
}

/**
 * @brief  allocate space inside the ring buffer to push data.
 * @param  Size to allocate within fifo
 * @param  Pos write position within the fifo
 * @retval 0 on success, -1 if no space available.
 */
static int16_t TRACE_AllocateBufer(uint16_t Size, uint16_t *Pos)
{
    uint16_t freesize;
    int16_t ret = -1;

    UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION();

    if (ADV_TRACE_Ctx.TraceWrPtr == ADV_TRACE_Ctx.TraceRdPtr)
    {
        freesize = (uint16_t)UTIL_ADV_TRACE_FIFO_SIZE;
    }
    else
    {
        if (ADV_TRACE_Ctx.TraceWrPtr > ADV_TRACE_Ctx.TraceRdPtr)
        {
            freesize = UTIL_ADV_TRACE_FIFO_SIZE - ADV_TRACE_Ctx.TraceWrPtr + ADV_TRACE_Ctx.TraceRdPtr;
        }
        else
        {
            freesize = ADV_TRACE_Ctx.TraceRdPtr - ADV_TRACE_Ctx.TraceWrPtr;
        }
    }

    if (freesize > Size)
    {
        *Pos = ADV_TRACE_Ctx.TraceWrPtr;
        ADV_TRACE_Ctx.TraceWrPtr = (ADV_TRACE_Ctx.TraceWrPtr + Size) % UTIL_ADV_TRACE_FIFO_SIZE;
        ret = 0;

        UTIL_ADV_TRACE_DEBUG("\n--TRACE_AllocateBufer(%d-%d::%d-%d)--\n", freesize - Size, Size, ADV_TRACE_Ctx.TraceRdPtr, ADV_TRACE_Ctx.TraceWrPtr);
    }
    else
    {
        s_trace_drop_count++;
    }

    UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
    return ret;
}

/**
 * @brief  Number of messages silently dropped so far because the trace
 *         FIFO didn't have room for them (see TRACE_AllocateBufer()).
 *         Purely diagnostic -- does not affect send behavior.
 */
uint32_t UTIL_ADV_TRACE_GetDropCount(void)
{
    return s_trace_drop_count;
}

/**
 * @brief  Lock the trace buffer.
 */
static void TRACE_Lock(void)
{
    UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION();
    ADV_TRACE_Ctx.TraceLock++;
    UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
}

/**
 * @brief  UnLock the trace buffer.
 */
static void TRACE_UnLock(void)
{
    UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION();
    ADV_TRACE_Ctx.TraceLock--;
    UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION();
}

/**
 * @brief  Check whether the trace buffer is locked.
 */
static uint32_t TRACE_IsLocked(void)
{
    return (ADV_TRACE_Ctx.TraceLock == 0u ? 0u : 1u);
}
