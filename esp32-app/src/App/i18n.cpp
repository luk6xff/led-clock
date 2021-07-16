#include "i18n.h"
#include "utils.h"
#include <assert.h>


//------------------------------------------------------------------------------
const i18n_map I18N::k_i18n =
{
    // Common
    { "" },                                 // M_COMMON_EMPTY
    { "°C" },                               // M_COMMON_DEG_CELS
    { "°F" },                               // M_COMMON_DEG_FAHR
    { "Pa" },                               // M_COMMON_PRESSURE_PA
    { "hPa" },                              // M_COMMON_PRESSURE_HPA
    { "%" },                                // M_COMMON_PERCENT
    { "V" },                                // M_COMMON_VOLTAGE
    { " " },                                // M_COMMON_SPACE
    // App language symbol
    { "en", "pl"},                          // M_LANG
    // Days of week
    { "Monday", "Poniedziałek" },           // M_DOW_MON
    { "Tuesday", "Wtorek" },                // M_DOW_TUE
    { "Wednesday", "Środa" },               // M_DOW_WED
    { "Thursday", "Czwartek" },             // M_DOW_THU
    { "Friday", "Piątek" },                 // M_DOW_FRI
    { "Saturday", "Sobota" },               // M_DOW_SAT
    { "Sunday", "Niedziela" },              // M_DOW_SUN
    // Months
    { "January", "Styczeń" },               // M_MONTH_JAN,
    { "February", "Luty" },                 // M_MONTH_FEB,
    { "March", "Marzec" },                  // M_MONTH_MAR,
    { "April", "Kwiecień" },                // M_MONTH_APR,
    { "May", "Maj" },                       // M_MONTH_MAY,
    { "June", "Czerwiec" },                 // M_MONTH_JUN,
    { "July", "Lipiec" },                   // M_MONTH_JUL,
    { "August", "Sierpień" },               // M_MONTH_AUG,
    { "September", "Wrzesień" },            // M_MONTH_SEP,
    { "October", "Październik" },           // M_MONTH_OCT,
    { "November", "Listopad" },             // M_MONTH_NOV,
    { "December", "Grudzień" },             // M_MONTH_DEC,
    // Weather
    { "Weather", "Pogoda" },                // M_WEATHER_NAME
    { "Temperature", "Temperatura" },       // M_WEATHER_TEMP
    { "Temp", "T" },                        // M_WEATHER_TEMP_SHORT
    { "Today", "Dziś" },                    // M_WEATHER_TODAY
    { "Tommorow", "Jutro" },                // M_WEATHER_TOMM
    { "Now", "Teraz" },                     // M_WEATHER_NOW
    // External/Internal environment data sensor
    { "ExternalSensor", "PomiarZewn" },     // M_EXT_ENV_SENSOR_NAME
    { "InternalSensor", "PomiarWewn" },     // M_INT_ENV_SENSOR_NAME
    { "Temp", "Temp" },                     // M_SENSOR_TEMP
    { "Pressure", "Ciśn" },                 // M_SENSOR_PRESS
    { "Humidity", "Wilg" },                 // M_SENSOR_HUMID
    { "VBatt", "Bat" },                     // M_SENSOR_VBATT
    // App Status
    { "Error", "Błąd" },                                                // M_ERROR
    { "No Radio comm", "Brak komunikacji z radiem" },                   // M_RADIO_ERROR,
    { "No RTC comm", "Brak komunikacji z RTC" },                        // M_RTC_ERROR,
    { "Display", "Wyświetlacz" },                                       // M_DISP_ERROR,
    { "No radio frame", "Brak komunikacji z zewn. sensorem" },          // M_EXT_DATA_SENSOR_ERROR,
    { "OTA update is ongoing", "OTA aktualizacja..." },                 // M_OTA_UPDATE_RUNNING,
    // Day Info
    { "Today is", "Dzisiaj jest" },                                                // M_TODAY,
    { "day", "dzień" },                                                 // M_DAY,
    { "of the year", "roku" },                                          // M_OF_THE_YEAR,
    { "<< Happy new year! >>", "<< Szczęśliwego nowego roku! >>" },     // M_HAPPY_NEW_YEAR,
};

//------------------------------------------------------------------------------
I18N::I18N(i18n_lang language)
    : m_lang(language)
{

}

//------------------------------------------------------------------------------
const std::string& I18N::translate(i18n_msg_id msg_id)
{
    if (k_i18n.size() <= msg_id)
    {
        utils::err("Error can't find: %d msgid in language file!", msg_id);
        return k_i18n[M_COMMON_EMPTY][0];
    }
    if (msg_id <= M_COMMON_LAST)
    {
        return k_i18n[msg_id][0];
    }
    return  k_i18n[msg_id][m_lang];
}


//------------------------------------------------------------------------------
void I18N::loadTranslation(i18n_lang language)
{
    if (language < I18N_LANGUAGES_NUM)
    {
        m_lang = language;
    }
}

//------------------------------------------------------------------------------
