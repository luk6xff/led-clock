#pragma once

#include <cstdint>
#include <cstring>

#undef USE_SECRET_DATA

#ifdef USE_SECRET_DATA
    #include "Common/secret_data.h"
#endif

struct WifiSettings
{

    #define WIFI_SETTINGS_LEN 32

    WifiSettings()
    {
        const char *admin = "admin";
        memset(this, 0, sizeof(*this));
#ifdef USE_SECRET_DATA
        memcpy(ssid, SECRET_WIFI_SSID, WIFI_SETTINGS_LEN);
        memcpy(pass, SECRET_WIFI_PASSWD, WIFI_SETTINGS_LEN);
#else
        memcpy(ssid, admin, WIFI_SETTINGS_LEN);
        memcpy(pass, admin, WIFI_SETTINGS_LEN);
#endif
        memcpy(ap_hostname, "emp-gateway", WIFI_SETTINGS_LEN);
        memcpy(ap_pass, "admin123", WIFI_SETTINGS_LEN);
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return "WifiSettings"+colon+comma + \
                "ssid"+colon+String(ssid)+comma + \
                "pass"+colon+String(pass)+comma  + \
                "apHostname"+colon+String(ap_hostname)+comma  + \
                "apPass"+colon+String(ap_pass);
    }

    char ssid[WIFI_SETTINGS_LEN];
    char pass[WIFI_SETTINGS_LEN];
    char ap_hostname[WIFI_SETTINGS_LEN];
    char ap_pass[WIFI_SETTINGS_LEN];
};