#include "WeatherConfigParam.h"
#include "Rtos/logger.h"
#include "Config.h"

//------------------------------------------------------------------------------
WeatherConfigParam::WeatherConfigParam() : ConfigParam(CFG_KEY_RADIO, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool WeatherConfigParam::setConfigFromJson(const JsonObject& json)
{
    WeatherConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        logger::dbg("WeatherConfigParam::unpackFromJson - SUCCESS");
        if (std::memcmp(&m_cfgData, &dataFromServer, cfgDataSize()) != 0)
        {
            logger::dbg("Storing new WeatherConfigData settings");
            return m_cfgHndl.save(key(), &dataFromServer);
        }
        logger::dbg("Storing WeatherConfigData settings skipped, no change detected");
        return false;
    }

    logger::err("WeatherConfigParam::unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
String WeatherConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
            cfgParamKey(WeatherKeys::WEATHER_ENABLED)+colon+String(m_cfgData.enabled)+comma + \
            cfgParamKey(WeatherKeys::WEATHER_SYNC_INTERVAL)+colon+String(m_cfgData.updateInterval)+comma + \
            cfgParamKey(WeatherKeys::WEATHER_OWM_APIKEY)+colon+String(m_cfgData.owmAppid)+comma + \
            cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_NAME)+colon+String(m_cfgData.owmCityName)+comma + \
            cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LATITUDE)+colon+String(m_cfgData.owmCityLatitude)+comma + \
            cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LONGITUDE)+colon+String(m_cfgData.owmCityLongitude);
}

//------------------------------------------------------------------------------
void WeatherConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { WeatherKeys::WEATHER_ENABLED,             "weather-update-enable" },
        { WeatherKeys::WEATHER_SYNC_INTERVAL,       "weather-sync-interval" },
        { WeatherKeys::WEATHER_OWM_APIKEY,          "weather-owm-api-key" },
        { WeatherKeys::WEATHER_OWM_CITY_NAME,       "weather-owm-city-name" },
        { WeatherKeys::WEATHER_OWM_CITY_LATITUDE,   "weather-owm-city-latitude" },
        { WeatherKeys::WEATHER_OWM_CITY_LONGITUDE,  "weather-owm-city-longitude" },
    };
}

//------------------------------------------------------------------------------
bool WeatherConfigParam::unpackFromJson(WeatherConfigData& cfgData, const JsonObject& json)
{
    // Not used so far
    bool ret = true;

    JsonArray arr = json[key()].as<JsonArray>();
    for (const auto& v : arr)
    {
        if (v[cfgParamKey(WeatherKeys::WEATHER_ENABLED)])
        {
            cfgData.enabled = v[cfgParamKey(WeatherKeys::WEATHER_ENABLED)].as<bool>();
        }
        else if (v[cfgParamKey(WeatherKeys::WEATHER_SYNC_INTERVAL)])
        {
            cfgData.updateInterval = v[cfgParamKey(WeatherKeys::WEATHER_SYNC_INTERVAL)].as<uint32_t>();
            if (cfgData.updateInterval == 0)
            {
                cfgData.updateInterval = 1;
            }
            // Convert from minutes on server to seconds
            cfgData.updateInterval = cfgData.updateInterval * 60;
        }
        else if (v[cfgParamKey(WeatherKeys::WEATHER_OWM_APIKEY)])
        {
            memset(cfgData.owmAppid, 0, OWM_APPID_MAXLEN);
            memcpy(cfgData.owmAppid, v[cfgParamKey(WeatherKeys::WEATHER_OWM_APIKEY)].as<const char*>(), OWM_APPID_MAXLEN);
        }
        else if (v[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_NAME)])
        {
            memset(cfgData.owmCityName, 0, OWM_CITY_NAME_MAXLEN);
            memcpy(cfgData.owmCityName, v[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_NAME)].as<const char*>(), OWM_CITY_NAME_MAXLEN);
        }
        else if (v[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LATITUDE)])
        {
            cfgData.owmCityLatitude = v[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LATITUDE)].as<float>();
            if (cfgData.owmCityLatitude < -90.f || cfgData.owmCityLatitude > 90.f)
            {
                cfgData.owmCityLatitude = 49.692167;
            }
        }
        else if (v[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LONGITUDE)])
        {
            cfgData.owmCityLongitude = v[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LONGITUDE)].as<float>();
            if (cfgData.owmCityLongitude< -180.f || cfgData.owmCityLongitude > 180.f)
            {
                cfgData.owmCityLongitude = 20.708694;
            }
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
String WeatherConfigParam::packToJson(const WeatherConfigData& data)
{
    StaticJsonDocument<1024> doc;
    String json;
    JsonArray arr = doc.createNestedArray(key());
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(WeatherKeys::WEATHER_ENABLED)] = data.enabled;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WeatherKeys::WEATHER_SYNC_INTERVAL)] = data.updateInterval / 60;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WeatherKeys::WEATHER_OWM_APIKEY)] = data.owmAppid;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_NAME)] = data.owmCityName;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LATITUDE)] = data.owmCityLatitude;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WeatherKeys::WEATHER_OWM_CITY_LONGITUDE)] = data.owmCityLongitude;
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
