/**
 * @file n32_systime.h
 * @brief System time (Unix epoch) functions for N32WB031.
 *
 * Calendar-math API contract. Hardware access is fully decoupled via the
 * UTIL_SYSTIM_Driver_s function-pointer table (extern UTIL_SYSTIMDriver),
 * populated by the project-local systime_if.c (see
 * BEACON/APP/src/systime_if.c).
 */

#ifndef N32_SYSTIME_H
#define N32_SYSTIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "time.h"

/* Exported constants --------------------------------------------------------*/

/* Days, Hours, Minutes and Seconds */
#define TM_DAYS_IN_LEAP_YEAR                        ( ( uint32_t )  366U )
#define TM_DAYS_IN_YEAR                             ( ( uint32_t )  365U )
#define TM_SECONDS_IN_1DAY                          ( ( uint32_t )86400U )
#define TM_SECONDS_IN_1HOUR                         ( ( uint32_t ) 3600U )
#define TM_SECONDS_IN_1MINUTE                       ( ( uint32_t )   60U )
#define TM_MINUTES_IN_1HOUR                         ( ( uint32_t )   60U )
#define TM_HOURS_IN_1DAY                            ( ( uint32_t )   24U )

/* Months */
#define TM_MONTH_JANUARY                            ( ( uint8_t ) 0U )
#define TM_MONTH_FEBRUARY                           ( ( uint8_t ) 1U )
#define TM_MONTH_MARCH                              ( ( uint8_t ) 2U )
#define TM_MONTH_APRIL                              ( ( uint8_t ) 3U )
#define TM_MONTH_MAY                                ( ( uint8_t ) 4U )
#define TM_MONTH_JUNE                               ( ( uint8_t ) 5U )
#define TM_MONTH_JULY                               ( ( uint8_t ) 6U )
#define TM_MONTH_AUGUST                             ( ( uint8_t ) 7U )
#define TM_MONTH_SEPTEMBER                          ( ( uint8_t ) 8U )
#define TM_MONTH_OCTOBER                            ( ( uint8_t ) 9U )
#define TM_MONTH_NOVEMBER                           ( ( uint8_t )10U )
#define TM_MONTH_DECEMBER                           ( ( uint8_t )11U )

/* Week days */
#define TM_WEEKDAY_SUNDAY                           ( ( uint8_t )0U )
#define TM_WEEKDAY_MONDAY                           ( ( uint8_t )1U )
#define TM_WEEKDAY_TUESDAY                          ( ( uint8_t )2U )
#define TM_WEEKDAY_WEDNESDAY                        ( ( uint8_t )3U )
#define TM_WEEKDAY_THURSDAY                         ( ( uint8_t )4U )
#define TM_WEEKDAY_FRIDAY                           ( ( uint8_t )5U )
#define TM_WEEKDAY_SATURDAY                         ( ( uint8_t )6U )

/** Number of seconds elapsed between Unix epoch and GPS epoch. */
#define UNIX_GPS_EPOCH_OFFSET                       315964800

/* Exported types --------------------------------------------------------*/

/** @brief Structure holding the system time in seconds and milliseconds. */
typedef struct SysTime_s
{
    uint32_t Seconds;
    int16_t SubSeconds;
} SysTime_t;

/** @brief SysTime driver definition -- populated by systime_if.c */
typedef struct
{
    void     (*BKUPWrite_Seconds)( uint32_t Seconds );      /*!< Store the delta between real time and calendar time. */
    uint32_t (*BKUPRead_Seconds)( void );                   /*!< Read the delta between real time and calendar time.  */
    void     (*BKUPWrite_SubSeconds)( uint32_t SubSeconds ); /*!< Store the sub-second part of the delta.             */
    uint32_t (*BKUPRead_SubSeconds)( void );                /*!< Read the sub-second part of the delta.              */
    uint32_t (*GetCalendarTime)( uint16_t* SubSeconds );    /*!< Read the current calendar time (Unix seconds).      */
} UTIL_SYSTIM_Driver_s;

/* Exported variables --------------------------------------------------------*/

/**
 * @brief Low-level hardware interface to the calendar/backup-store.
 * @remark Defined and initialized in the project-local systime_if.c.
 */
extern const UTIL_SYSTIM_Driver_s UTIL_SYSTIMDriver;

/* Exported functions ------------------------------------------------------- */

/** @brief Adds 2 SysTime_t values. */
SysTime_t SysTimeAdd( SysTime_t a, SysTime_t b );

/** @brief Subtracts 2 SysTime_t values (a - b). */
SysTime_t SysTimeSub( SysTime_t a, SysTime_t b );

/** @brief Sets the new system time (seconds/sub-seconds since Unix epoch). */
void SysTimeSet( SysTime_t sysTime );

/** @brief Gets the current system time (seconds/sub-seconds since Unix epoch). */
SysTime_t SysTimeGet( void );

/** @brief Gets the current MCU/calendar time (seconds/sub-seconds since MCU started/calendar epoch). */
SysTime_t SysTimeGetMcuTime( void );

/** @brief Converts the given SysTime to the equivalent value in milliseconds. */
uint32_t SysTimeToMs( SysTime_t sysTime );

/** @brief Converts the given value in milliseconds to the equivalent SysTime. */
SysTime_t SysTimeFromMs( uint32_t timeMs );

/** @brief Converts a calendar time into seconds since Unix epoch. */
uint32_t SysTimeMkTime( const struct tm* localtime );

/** @brief Converts a given time in seconds since Unix epoch into calendar time. */
void SysTimeLocalTime( const uint32_t timestamp, struct tm *localtime );

#ifdef __cplusplus
}
#endif

#endif /* N32_SYSTIME_H */
