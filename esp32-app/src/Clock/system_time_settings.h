#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>

#include <Timezone.h>


struct SystemTimeSettings
{
    #define TIME_CFG_KEY          "dev-cfg-time"
    #define TIME_CFG_VAL_DATE     "time-date"
    #define TIME_CFG_VAL_CLK      "time-clock"
    #define TIME_CFG_VAL_TZ_NUM   "time-timezone-num"
    #define TIME_CFG_VAL_TZ_1     "time-timezone-1"
    #define TIME_CFG_VAL_TZ_2     "time-timezone-2"
    #define TIME_CFG_VAL_NTP_ON   "time-ntp-enable"
    #define TIME_CFG_VAL_TZ_2     "time-ntp-sync-interval"


    std::string toJson()
    {
        StaticJsonDocument<256> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(TIME_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        // obj[TIME_CFG_VAL_DATE] = ssid;
        // obj = arr.createNestedObject();
        // obj[TIME_CFG_VAL_CLK] = pass;
        // obj = arr.createNestedObject();
        // obj[TIME_CFG_VAL_TZ_NUM] = ap_pass;
        serializeJson(doc, json);
        return json;
    }

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[TIME_CFG_KEY].as<JsonArray>();
        for (const auto& v : arr)
        {
            // if (v[WIFI_CFG_VAL_SSID])
            // {
            //     memset(ssid, 0, WIFI_SETTINGS_LEN);
            //     memcpy(ssid, v[WIFI_CFG_VAL_SSID].as<const char*>(), WIFI_SETTINGS_LEN);
            // }
            // else if (v[WIFI_CFG_VAL_PASS])
            // {
            //     memset(pass, 0, WIFI_SETTINGS_LEN);
            //     memcpy(pass, v[WIFI_CFG_VAL_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
            // }
            // else if (v[WIFI_CFG_VAL_AP_PASS])
            // {
            //     memset(ap_pass, 0, WIFI_SETTINGS_LEN);
            //     memcpy(ap_pass, v[WIFI_CFG_VAL_AP_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
            // }
        }
    }


    TimeChangeRule dstStart; // CEST
    TimeChangeRule stdStart; // CET
    String toStr()
    {
        return String("dstStart:" + dstStart.toStr() + " stdStart:" + stdStart.toStr());
    }
};