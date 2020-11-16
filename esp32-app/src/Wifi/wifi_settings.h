#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>


#define WIFI_CFG_KEY                "dev-cfg-wifi"
#define WIFI_CFG_VAL_SSID           "wifi-ssid"
#define WIFI_CFG_VAL_PASS           "wifi-pass"
#define WIFI_CFG_VAL_AP_HOSTNAME    "wifi-ap-hostname"
#define WIFI_CFG_VAL_AP_PASS        "wifi-ap-pass"


struct WifiSettings
{

    #define WIFI_SETTINGS_LEN 32

    WifiSettings()
    {
        const char *admin = "admin";
        memset(this, 0, sizeof(*this));
        memcpy(ssid, admin, WIFI_SETTINGS_LEN);
        memcpy(pass, admin, WIFI_SETTINGS_LEN);
        memcpy(ap_hostname, "ledclock", WIFI_SETTINGS_LEN);
        memcpy(ap_pass, admin, WIFI_SETTINGS_LEN);
    }

    String toStr()
    {
        return "ssid:" + String(String(ssid) + " pass:" + String(pass) + \
               " ap-hostname:" + String(ap_hostname) + " ap-pass:" + String(ap_pass));
    }

    std::string toJson()
    {
        StaticJsonDocument<256> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(WIFI_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[WIFI_CFG_VAL_SSID] = ssid;
        obj = arr.createNestedObject();
        obj[WIFI_CFG_VAL_PASS] = pass;
        obj = arr.createNestedObject();
        obj[WIFI_CFG_VAL_AP_HOSTNAME] = ap_hostname;
        obj = arr.createNestedObject();
        obj[WIFI_CFG_VAL_AP_PASS] = ap_pass;
        serializeJson(doc, json);
        return json;
    }

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[WIFI_CFG_KEY].as<JsonArray>();
        for (const auto& v : arr)
        {
            if (v[WIFI_CFG_VAL_SSID])
            {
                memset(ssid, 0, WIFI_SETTINGS_LEN);
                memcpy(ssid, v[WIFI_CFG_VAL_SSID].as<const char*>(), WIFI_SETTINGS_LEN);
            }
            else if (v[WIFI_CFG_VAL_PASS])
            {
                memset(pass, 0, WIFI_SETTINGS_LEN);
                memcpy(pass, v[WIFI_CFG_VAL_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
            }
            else if (v[WIFI_CFG_VAL_AP_HOSTNAME])
            {
                memset(ap_pass, 0, WIFI_SETTINGS_LEN);
                memcpy(ap_pass, v[WIFI_CFG_VAL_AP_HOSTNAME].as<const char*>(), WIFI_SETTINGS_LEN);
            }
            else if (v[WIFI_CFG_VAL_AP_PASS])
            {
                memset(ap_pass, 0, WIFI_SETTINGS_LEN);
                memcpy(ap_pass, v[WIFI_CFG_VAL_AP_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
            }
        }
    }

    char ssid[WIFI_SETTINGS_LEN];
    char pass[WIFI_SETTINGS_LEN];
    char ap_hostname[WIFI_SETTINGS_LEN];
    char ap_pass[WIFI_SETTINGS_LEN];
};