
#pragma once
#include <stdint.h>

/**
 * @brief App status
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2021-07-14
 */

typedef enum
{
    NO_ERROR                = 1<<0,
    RADIO_ERROR             = 1<<1,
    RTC_ERROR               = 1<<2,
    DISP_ERROR              = 1<<3,
    EXT_DATA_SENSOR_ERROR   = 1<<4,
    OTA_UPDATE_RUNNING      = 1<<5,
} AppStatusType;

typedef uint32_t AppStatusValue;
