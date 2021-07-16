#pragma once

#include <string>
#include <vector>
#include <array>
#include <utility>




//------------------------------------------------------------------------------
typedef enum
{
    // Common
    M_COMMON_EMPTY = 0,
    M_COMMON_DEG_CELS,
    M_COMMON_DEG_FAHR,
    M_COMMON_PRESSURE_PA,
    M_COMMON_PRESSURE_HPA,
    M_COMMON_PERCENT,
    M_COMMON_VOLTAGE,
    M_COMMON_SPACE,
    M_COMMON_LAST = M_COMMON_SPACE,
    // App language symbol
    M_LANG,
    // Days of week
    M_DOW_MON,
    M_DOW_TUE,
    M_DOW_WED,
    M_DOW_THU,
    M_DOW_FRI,
    M_DOW_SAT,
    M_DOW_SUN,
    // Months
    M_MONTH_JAN,
    M_MONTH_FEB,
    M_MONTH_MAR,
    M_MONTH_APR,
    M_MONTH_MAY,
    M_MONTH_JUN,
    M_MONTH_JUL,
    M_MONTH_AUG,
    M_MONTH_SEP,
    M_MONTH_OCT,
    M_MONTH_NOV,
    M_MONTH_DEC,
    // Weather
    M_WEATHER_NAME,
    M_WEATHER_TEMP,
    M_WEATHER_TEMP_SHORT,
    M_WEATHER_TODAY,
    M_WEATHER_TOMM,
    M_WEATHER_NOW,
    // External/Internal environment data sensor
    M_EXT_ENV_SENSOR_NAME,
    M_INT_ENV_SENSOR_NAME,
    M_SENSOR_TEMP,
    M_SENSOR_PRESS,
    M_SENSOR_HUMID,
    M_SENSOR_VBATT,
    // App Status
    M_ERROR,
    M_RADIO_ERROR,
    M_RTC_ERROR,
    M_DISP_ERROR,
    M_EXT_DATA_SENSOR_ERROR,
    M_OTA_UPDATE_RUNNING,
    // Day Info
    M_TODAY_IS,
    M_DAY,
    M_OF_THE_YEAR,
    M_HAPPY_NEW_YEAR,

    // Last msg_id keep it always here
    M_MSGID_LAST
} i18n_msg_id;

//------------------------------------------------------------------------------
typedef enum
{
    I18N_ENGLISH,
    I18N_POLISH,
    I18N_LANGUAGES_NUM
} i18n_lang;

//------------------------------------------------------------------------------
using i18n_map = std::vector<std::array<const std::string, I18N_LANGUAGES_NUM>>;

class I18N final
{

public:

    explicit I18N(i18n_lang language);

    /**
     * @brief get text form map
     */
    const std::string& translate(i18n_msg_id msg_id);

    /**
     * @brief Load language data for language file in a map
     */
    void loadTranslation(i18n_lang language);

private:

    i18n_lang m_lang;
    static const i18n_map k_i18n;

};

