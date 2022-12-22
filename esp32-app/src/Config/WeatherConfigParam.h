#pragma once

#include "config_param.h"
#include "config_types.h"
#include "config.h"


struct WeatherConfigData
{
    #define OWM_APPID_MAXLEN        48
    #define OWM_CITY_NAME_MAXLEN    32


// WeatherConfigData()
//     : enabled(true)
//     , updateInterval(3600)
// {
//     memset(owmCityName, 0, OWM_CITY_NAME_MAXLEN);
//     memset(owmAppid, 0, OWM_APPID_MAXLEN);
//     memcpy(owmAppid, "INVALID_KEY", OWM_APPID_MAXLEN);
//     memcpy(owmCityName, "Ubiad", OWM_CITY_NAME_MAXLEN);
//     owmCityLatitude(49.692167), // Ubiad, PL
//     owmCityLongitude(20.708694)
// }


    bool enabled;
    uint32_t updateInterval; // [s] in seconds
    char owmAppid[OWM_APPID_MAXLEN];
    char owmCityName[OWM_CITY_NAME_MAXLEN];
    float owmCityLatitude;
    float owmCityLongitude;
};


class WeatherConfigParam : public ConfigParam<WeatherConfigData, Config>
{
public:

    enum WeatherKeys : ParamsKey
    {
        WEATHER_ENABLED,
        WEATHER_SYNC_INTERVAL,
        WEATHER_OWM_APIKEY,
        WEATHER_OWM_CITY_NAME,
        WEATHER_OWM_CITY_LATITUDE,
        WEATHER_OWM_CITY_LONGITUDE,
    };

    WeatherConfigParam();
    bool setConfig(const JsonObject& json) override;
    void getConfig(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(WeatherConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const WeatherConfigData& data) override;
};
