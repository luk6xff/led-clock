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


    City city;
    char owmAppid[OWM_APPID_MAXLEN];
    char language[4];
    uint32_t updateInterval; // [s] in seconds
};