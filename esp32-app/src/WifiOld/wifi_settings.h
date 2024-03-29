#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>


#define CFG_KEY_WIFI                "dev-cfg-wifi"
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
        memcpy(ap_pass, "admin123", WIFI_SETTINGS_LEN);
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return String(CFG_KEY_WIFI)+colon+comma + \
                String(WIFI_CFG_VAL_SSID)+colon+String(ssid)+comma + \
                String(WIFI_CFG_VAL_PASS)+colon+String(pass)+comma  + \
                String(WIFI_CFG_VAL_AP_HOSTNAME)+colon+String(ap_hostname)+comma  + \
                String(WIFI_CFG_VAL_AP_PASS)+colon+String(ap_pass);
    }

    std::string toJson()
    {
        StaticJsonDocument<512> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(CFG_KEY_WIFI);
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
        JsonArray arr = json[CFG_KEY_WIFI].as<JsonArray>();
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
                memset(ap_hostname, 0, WIFI_SETTINGS_LEN);
                memcpy(ap_hostname, v[WIFI_CFG_VAL_AP_HOSTNAME].as<const char*>(), WIFI_SETTINGS_LEN);
            }
            else if (v[WIFI_CFG_VAL_AP_PASS])
            {
                String pass = v[WIFI_CFG_VAL_AP_PASS].as<const char*>();
                if (pass.length() < 8 || pass.length() > 63)
                {
                    pass = "admin123";
                }
                memset(ap_pass, 0, WIFI_SETTINGS_LEN);
                memcpy(ap_pass, pass.c_str(), WIFI_SETTINGS_LEN);
            }
        }
    }

    char ssid[WIFI_SETTINGS_LEN];
    char pass[WIFI_SETTINGS_LEN];
    char ap_hostname[WIFI_SETTINGS_LEN];
    char ap_pass[WIFI_SETTINGS_LEN];
};