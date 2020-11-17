#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>
#include "App/utils.h"



#define WEATHER_CFG_KEY                 "dev-cfg-weather"
#define WEATHER_CFG_VAL_ENABLE          "weather-update-enable"
#define WEATHER_CFG_VAL_SYNC_INTERVAL   "weather-sync-interval"
#define WEATHER_CFG_VAL_OWM_APIKEY      "weather-owm-api-key"
#define WEATHER_CFG_VAL_OWM_LANG        "weather-owm-lang"
#define WEATHER_CFG_VAL_OWM_CITY        "weather-owm-city"


struct WeatherData
{
    uint8_t test;
};


struct WeatherSettings
{
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
            const String colon = ":";
            const String comma =", ";
            return String(WEATHER_CFG_VAL_OWM_CITY)+colon+comma + \
                    String("cityName")+colon+String(cityName)+comma + \
                    String("latitude")+colon+String(latitude)+comma + \
                    String("longitude")+colon+String(longitude);
        }
    };


    WeatherSettings()
        : enable(true)
        , updateInterval(3600)
    {
        memset(city.cityName, 0, CITY_NAME_MAXLEN);
        memset(owmAppid, 0, OWM_APPID_MAXLEN);
        memset(language, 0, LANG_MAXLEN);
        memcpy(owmAppid, "INVALID_KEY", OWM_APPID_MAXLEN);
        memcpy(language, "pl", LANG_MAXLEN);
        city = {
            49.692167, // Ubiad, PL
            20.708694
        };
        memcpy(city.cityName, "Ubiad", CITY_NAME_MAXLEN);
    }

    WeatherSettings(bool enableOnOff, uint32_t updateInterval, const char *apiKey,
                    const char *language,  const City& city)
        : enable(enableOnOff)
        , updateInterval(updateInterval)
        , city(city)
    {
        memcpy(this->owmAppid, apiKey, OWM_APPID_MAXLEN);
        memcpy(this->language, language, LANG_MAXLEN);
    }

    std::string toJson()
    {
        StaticJsonDocument<512> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(WEATHER_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_ENABLE] = enable;
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_SYNC_INTERVAL] = updateInterval / 60; //Convert from seconds to minutes on server.
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_OWM_APIKEY] = owmAppid;
        obj = arr.createNestedObject();
        obj[WEATHER_CFG_VAL_OWM_LANG] = language;
        obj = arr.createNestedObject();
        const String c = cityToServerStr(city);
        obj[WEATHER_CFG_VAL_OWM_CITY] = c.c_str();
        serializeJson(doc, json);
        return json;
    }

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[WEATHER_CFG_KEY].as<JsonArray>();
        for (const auto& v : arr)
        {
            if (v[WEATHER_CFG_VAL_ENABLE])
            {
                enable = v[WEATHER_CFG_VAL_ENABLE].as<uint32_t>();
            }
            else if (v[WEATHER_CFG_VAL_SYNC_INTERVAL])
            {
                updateInterval = v[WEATHER_CFG_VAL_SYNC_INTERVAL].as<uint32_t>();
                // Convert from minutes on server to seconds
                updateInterval = updateInterval * 60;
            }
            else if (v[WEATHER_CFG_VAL_OWM_APIKEY])
            {
                memset(owmAppid, 0, OWM_APPID_MAXLEN);
                memcpy(owmAppid, v[WEATHER_CFG_VAL_OWM_APIKEY].as<const char*>(), OWM_APPID_MAXLEN);
            }
            else if (v[WEATHER_CFG_VAL_OWM_LANG])
            {
                memset(language, 0, LANG_MAXLEN);
                memcpy(language, v[WEATHER_CFG_VAL_OWM_LANG].as<const char*>(), LANG_MAXLEN);
            }
            else if (v[WEATHER_CFG_VAL_OWM_CITY])
            {
                std::string str(v[WEATHER_CFG_VAL_OWM_CITY].as<const char*>());
                // Validate
                City c;
                if (validateAndParseCityStr(str, c))
                {
                    city = c;
                }
                else
                {
                    utils::err("Invalid settings for Weather City");
                }
            }
        }
    }

    bool validateAndParseCityStr(const std::string& s, City& c)
    {
        // Validate
        std::string delim = "|";
        if (s.size() == 0)
        {
            return false;
        }
        if (s[0] != '[' || s[s.size()-1] != ']')
        {
            return false;
        }

        auto start = 1U;
        auto end = s.find(delim);
        std::string str = s;
        str[0] = '|';
        str[str.size()-1] = '|';
        std::vector<std::string> data;
        while (end != std::string::npos)
        {
            data.push_back(str.substr(start, end-start));
            start = end + delim.length();
            end = str.find(delim, start);
        }

        if (data.size() != 3)
        {
            utils::err("Not enough data in Weather city cfg: %d", data.size());
            utils::dbg("City cfg:");
            for (auto& d : data)
            {
                utils::dbg("%s", d.c_str());
            }
            return false;
        }

        // Check values
        const float lat = String(data[1].c_str()).toFloat();
        if (lat < -90.f || lat > 90.f)
        {
            return false;
        }
        const float lon = String(data[2].c_str()).toFloat();
        if (lon < -180.f || lon > 180.f)
        {
            return false;
        }

        // Set values in memory
        memcpy(c.cityName, data[0].c_str(), CITY_NAME_MAXLEN);
        c.latitude = lat;
        c.longitude = lon;
        return true;
    }


    String cityToServerStr(const City& city)
    {
        String s;
        s += String('[');
        s += String(city.cityName) + '|';
        s += String(city.latitude) + '|';
        s += String(city.longitude) + ']';
        return s;
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return String(WEATHER_CFG_KEY)+colon+comma + \
                String(WEATHER_CFG_VAL_ENABLE)+colon+String(enable)+comma + \
                String(WEATHER_CFG_VAL_SYNC_INTERVAL)+colon+String(updateInterval)+comma + \
                String(WEATHER_CFG_VAL_OWM_APIKEY)+colon+String(owmAppid)+comma + \
                String(WEATHER_CFG_VAL_OWM_LANG)+colon+String(language)+comma + \
                city.toStr();
    }


    uint32_t enable;
    uint32_t updateInterval; // [s] in seconds
    char owmAppid[OWM_APPID_MAXLEN];
    char language[LANG_MAXLEN];
    City city;
};