#pragma once

#include "ConfigTypes.h"
#include "ConfigParam.h"
#include "ConfigDataTypes.h"

class WeatherConfigParam : public ConfigParam<WeatherConfigData>
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
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(WeatherConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const WeatherConfigData& data) override;
};
