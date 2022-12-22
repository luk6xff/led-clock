#pragma once

#include "ConfigParam.h"
#include "ConfigTypes.h"
#include "Config.h"


class Config;

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
    bool setConfigFromJson(const JsonObject& json) override;
    void getConfigAsStr(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(WeatherConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const WeatherConfigData& data) override;
};
