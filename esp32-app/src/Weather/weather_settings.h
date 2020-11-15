#pragma once

#include <Arduino.h>
#include <cstdint>
#include <cstring>


struct WeatherData
{

    uint8_t test;
};


struct WeatherSettings
{
    #define WEATHER_CFG_KEY                    "dev-cfg-weather"
    #define WEATHER_CFG_VAL_ENABLE             "weather-update-enable"
    #define WEATHER_CFG_VAL_SYNC_INTERVAL     "weather-sync-interval"
    #define WEATHER_CFG_VAL_TZ_NUM             "weather-owm-api-key
    #define WEATHER_CFG_VAL_TZ_1               "weather-owm-city"
    #define WEATHER_CFG_VAL_TZ_2               "weather-owm-lang"


    #define CITY_NAME_MAXLEN    16
    #define OWM_APPID_MAXLEN    48
    #define LANG_MAXLEN          4

    struct City
    {
        float latitude;
        float longitude;
        char cityName[CITY_NAME_MAXLEN];
        String toStr()
        {
            return "cityName:" + String(String(cityName) + " latitude:" + String(latitude) + " longitude:" + String(longitude));
        }
    };


    WeatherSettings()
        : updateInterval(3600)
    {
        city = {
                49.692167, // Ubiad, PL
                20.708694
                };
        memset(city.cityName, 0, CITY_NAME_MAXLEN);
        memset(owmAppid, 0, OWM_APPID_MAXLEN);
        memset(language, 0, LANG_MAXLEN);
        memcpy(city.cityName, "Ubiad", CITY_NAME_MAXLEN);
        memcpy(owmAppid, "INVALID_KEY", OWM_APPID_MAXLEN);
        memcpy(language, "pl", LANG_MAXLEN);
    }

    WeatherSettings(const City& city, const char *apiKey,
                const char *language, uint32_t updateInterval)
        : city(city)
        , updateInterval(updateInterval)
    {
        memcpy(this->owmAppid, apiKey, OWM_APPID_MAXLEN);
        memcpy(this->language, language, LANG_MAXLEN);
    }

    String toStr()
    {
        return "city:" + String(city.toStr() + " owmAppid:" + String(owmAppid) + " language:" + String(language) + \
                      " updateInterval:" + String(updateInterval));
    }

    std::string toJson()
    {
        StaticJsonDocument<256> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(WEATHER_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_DATE] = "2020-11-20";
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_CLK] = "11:20";
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_TZ_NUM] = timezoneNum;
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_TZ_1] = timeChangeRuleToServerStr(stdStart);
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_TZ_2] = timeChangeRuleToServerStr(dstStart);
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_NTP_ON] = ntp.ntpEnabled;
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_NTP_WEATHER_OFFSET] = ntp.timeOffset;
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_NTP_SYNC_INT] = ntp.updateInterval;
        serializeJson(doc, json);
        return json;
    }

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[WEATHER_CFG_KEY].as<JsonArray>();
        for (const auto& v : arr)
        {
            if (v[WEATHER_CFG_VAL_TZ_NUM])
            {
                timezoneNum =  v[WEATHER_CFG_VAL_TZ_NUM].as<uint32_t>();
            }
            else if (v[WEATHER_CFG_VAL_TZ_1])
            {
                std::string tz(v[WEATHER_CFG_VAL_TZ_1].as<const char*>());
                // Validate
                TimeChangeRule tzRule;
                if (validateAndParseTzCfgStr(tz, tzRule))
                {
                    stdStart = tzRule;
                }
                else
                {
                    utils::err("Invalid settings for Timezone1");
                }
            }
            else if (v[WEATHER_CFG_VAL_TZ_2])
            {
                std::string tz(v[WEATHER_CFG_VAL_TZ_2].as<const char*>());
                // Validate
                TimeChangeRule tzRule;
                if (validateAndParseTzCfgStr(tz, tzRule))
                {
                    dstStart = tzRule;
                }
                else
                {
                    utils::err("Invalid settings for Timezone1");
                }
            }
            else if (v[WEATHER_CFG_VAL_NTP_ON])
            {
                ntp.ntpEnabled = v[WEATHER_CFG_VAL_NTP_ON].as<uint32_t>();
            }
            else if (v[WEATHER_CFG_VAL_NTP_WEATHER_OFFSET])
            {
                ntp.timeOffset = v[WEATHER_CFG_VAL_NTP_WEATHER_OFFSET].as<int32_t>();
            }
            else if (v[WEATHER_CFG_VAL_NTP_SYNC_INT])
            {
                ntp.updateInterval = v[WEATHER_CFG_VAL_NTP_SYNC_INT].as<uint32_t>();
            }
        }
    }


    City city;
    char owmAppid[OWM_APPID_MAXLEN];
    char language[4];
    uint32_t updateInterval; // [s] in seconds
};