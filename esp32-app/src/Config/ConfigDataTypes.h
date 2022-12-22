#pragma once

#include "App/i18n.h"
#include "ConfigTypes.h"

#define DEV_CFG
#undef DEV_CFG
#ifdef DEV_CFG
    #include "dev_cfg.h"
#endif

#define CFG_KEY_WIFI        "dev-cfg-wifi"
#define CFG_KEY_TIME        "dev-cfg-time"
#define CFG_KEY_DISPLAY     "dev-cfg-display"
#define CFG_KEY_RADIO       "dev-cfg-radio"
#define CFG_KEY_INTENV      "dev-cfg-intenv"
#define CFG_KEY_WEATHER     "dev-cfg-weather"
#define CFG_KEY_APP         "dev-cfg-app"


struct WifiConfigData : public IConfigData
{
    #define WIFI_CONFIG_LEN 32

    WifiConfigData()
    {
    #ifdef DEV_CFG
        memcpy(ssid, DEV_CFG_WIFI_SSID, sizeof(ssid));
        memcpy(pass, DEV_CFG_WIFI_PASS, sizeof(passwd));
    #else
        memcpy(ssid, "admin", sizeof(ssid));
        memcpy(passwd, "admin", sizeof(passwd));
    #endif
        memcpy(apHostname, "ledclock", sizeof(apHostname));
        memcpy(apPasswd, "admin123", sizeof(apPasswd));
    }

    char ssid[WIFI_CONFIG_LEN];
    char passwd[WIFI_CONFIG_LEN];
    char apHostname[WIFI_CONFIG_LEN];
    char apPasswd[WIFI_CONFIG_LEN];
};


struct TimeConfigData : IConfigData
{
    #define NTP_SERVER_NAME_MAXLEN 32
    TimeConfigData()
    : timezoneNum(2)
    , stdTimezone1UtcOffset(1) // {"CET", Last, Sun, Oct, 3, 60},  // Central European Standard Time
    , dstTimezone2UtcOffset(2) // {"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
    , ntpEnabled(true)
    , ntpTimeOffset(0)
    , ntpUpdateInterval((1000*3600))
    {
        memcpy(ntpPoolServerNames[0], "time.google.com", NTP_SERVER_NAME_MAXLEN);
        memcpy(ntpPoolServerNames[1], "pl.pool.ntp.org", NTP_SERVER_NAME_MAXLEN);
        memcpy(ntpPoolServerNames[2], "pool.ntp.org",    NTP_SERVER_NAME_MAXLEN);
    }

    uint32_t timezoneNum;
    int32_t stdTimezone1UtcOffset;  // Standard time, difference in hours. eg. CET  - Central European Time (UTC+1)
    int32_t dstTimezone2UtcOffset;  // Summer time, difference in hours. eg. CEST - Central European daylight saving time (summer UTC+2)
    uint32_t ntpEnabled;
    int32_t ntpTimeOffset;          // in [min]
    uint32_t ntpUpdateInterval;     // in [ms]
    char ntpPoolServerNames[3][NTP_SERVER_NAME_MAXLEN];
};


struct DisplayConfigData : IConfigData
{
    DisplayConfigData()
        : enableAutoIntenisty(true)
        , intensityValue(0)
        , animationSpeed(30)
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


struct RadioConfigData : IConfigData
{
    RadioConfigData()
    : enabled(false)
    , dataUpdateInterval(1800)
    , critVbattLevel(3000)
    , msgDispRepeatNum(1)
    , lastMsgDispEveryMinutes(5)
    {
    }

    bool enabled;
    uint32_t dataUpdateInterval;        // [s] in seconds
    uint32_t critVbattLevel;            // [mV]
    uint32_t msgDispRepeatNum;          // How many times incoming message shall be displayed
    uint32_t lastMsgDispEveryMinutes;   // How often shall be last sensor msg displayed in minutes [min]
};


struct InternalEnvironmentDataConfigData : IConfigData
{
    InternalEnvironmentDataConfigData()
    : enabled(false)
    , dataUpdateInterval(900)
    , msgDispRepeatNum(1)
    {
    }

    bool enabled;                 // is functionality active
    uint32_t dataUpdateInterval; // [s] in seconds
    uint32_t msgDispRepeatNum;   // How many times incoming message shall be displayed
};


struct WeatherConfigData : IConfigData
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
    #ifdef DEV_CFG
        memcpy(owmAppid, DEV_CFG_WEATHER_OWM_APPID, OWM_APPID_MAXLEN);
    #endif
    }

    bool enabled;
    uint32_t updateInterval; // [s] in seconds
    char owmAppid[OWM_APPID_MAXLEN];
    char owmCityName[OWM_CITY_NAME_MAXLEN];
    float owmCityLatitude;
    float owmCityLongitude;
};


struct AppConfigData : IConfigData
{

    AppConfigData()
    : appLang(I18N_POLISH)
    {
    }
    i18n_lang appLang;
};
