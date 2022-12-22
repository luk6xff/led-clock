#pragma once

#include "App/i18n.h"


#define WIFI_CFG_KEY        "dev-cfg-wifi"
#define TIME_CFG_KEY        "dev-cfg-time"
#define DISPLAY_CFG_KEY     "dev-cfg-display"
#define RADIO_CFG_KEY       "dev-cfg-radio"
#define INTENV_CFG_KEY      "dev-cfg-intenv"
#define WEATHER_CFG_KEY     "dev-cfg-weather"
#define APP_CFG_KEY         "dev-cfg-app"


struct WifiConfigData
{
    #define WIFI_CONFIG_LEN 32

    char ssid[WIFI_CONFIG_LEN];
    char passwd[WIFI_CONFIG_LEN];
    char apHostname[WIFI_CONFIG_LEN];
    char apPasswd[WIFI_CONFIG_LEN];
};


struct TimeConfigData
{
    #define NTP_SERVER_NAME_MAXLEN 32
    uint32_t timezoneNum;
    int32_t stdTimezone1UtcOffset;  // Standard time, difference in hours. eg. CET  - Central European Time (UTC+1)
    int32_t dstTimezone2UtcOffset;  // Summer time, difference in hours. eg. CEST - Central European daylight saving time (summer UTC+2)
    uint32_t ntpEnabled;
    int32_t ntpTimeOffset;          // in [min]
    uint32_t ntpUpdateInterval;     // in [ms]
    char ntpPoolServerNames[3][NTP_SERVER_NAME_MAXLEN];
};


struct DisplayConfigData
{
    DisplayConfigData()
        : enableAutoIntenisty(true)
        , intensityValue(0)
        , animationSpeed(70)
        , timeFormat(1)
    {
    }

    DisplayConfigData(bool enableOnOff, uint8_t intensityValue,
                    uint8_t animationSpeed,  uint8_t timeFormat)
        : enableAutoIntenisty(enableOnOff)
        , intensityValue(intensityValue)
        , animationSpeed(animationSpeed)
        , timeFormat(timeFormat)
    {
    }

    uint8_t enableAutoIntenisty;
    uint8_t intensityValue;
    uint8_t animationSpeed;
    uint8_t timeFormat; // [s] in seconds
};


struct RadioConfigData
{
    bool enabled;
    uint32_t dataUpdateInterval;        // [s] in seconds
    uint32_t critVbattLevel;            // [mV]
    uint32_t msgDispRepeatNum;          // How many times incoming message shall be displayed
    uint32_t lastMsgDispEveryMinutes;   // How often shall be last sensor msg displayed in minutes [min]
};


struct InternalEnvironmentDataConfigData
{
    bool enabled;                 // is functionality active
    uint32_t dataUpdateInterval; // [s] in seconds
    uint32_t msgDispRepeatNum;   // How many times incoming message shall be displayed
};


struct WeatherConfigData
{
    #define OWM_APPID_MAXLEN        48
    #define OWM_CITY_NAME_MAXLEN    32

    WeatherConfigData()
    : enabled(true)
    , updateInterval(3600)
    , owmAppid("INVALID_KEY")
    , owmCityName("Ubiad")
    , owmCityLatitude(49.692167) // Ubiad, PL
    , owmCityLongitude(20.708694)
    {
    }

    bool enabled;
    uint32_t updateInterval; // [s] in seconds
    char owmAppid[OWM_APPID_MAXLEN];
    char owmCityName[OWM_CITY_NAME_MAXLEN];
    float owmCityLatitude;
    float owmCityLongitude;
};


struct AppConfigData
{

    AppConfigData()
        : appLang(I18N_POLISH)
    {
    }
    i18n_lang appLang;
};
