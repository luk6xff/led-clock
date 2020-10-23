#pragma once
#include "App/task.h"
#include <WiFiMulti.h>
#include <WiFi.h>

struct WifiSettings
{
    #define WIFI_SETTINGS_LEN 32
    #define WIFI_SETTINGS_CH_NUM 3

    WifiSettings()
    {
        const char *admin = "admin";
        memset(this, 0, sizeof(*this));
        memcpy(ssid0, admin, WIFI_SETTINGS_LEN);
        memcpy(pass0, admin, WIFI_SETTINGS_LEN);
        memcpy(ssid1, admin, WIFI_SETTINGS_LEN);
        memcpy(pass1, admin, WIFI_SETTINGS_LEN);
        memcpy(ssid2, admin, WIFI_SETTINGS_LEN);
        memcpy(pass2, admin, WIFI_SETTINGS_LEN);
    }

    char ssid0[WIFI_SETTINGS_LEN];
    char pass0[WIFI_SETTINGS_LEN];
    char ssid1[WIFI_SETTINGS_LEN];
    char pass1[WIFI_SETTINGS_LEN];
    char ssid2[WIFI_SETTINGS_LEN];
    char pass2[WIFI_SETTINGS_LEN];
};

class WifiTask : public Task
{

public:
    enum WifiEvent
    {
        WIFI_INVALID        = 1<<0,
        WIFI_CONNECTED      = 1<<1,
        WIFI_DISCONNECTED   = 1<<2,
    };

public:
    explicit WifiTask(const WifiSettings& wifiCfg);

    const EventGroupHandle_t& getWifiEvtHandle();

private:
    virtual void run() override;

private:
    const WifiSettings& m_wifiCfg;
    WiFiMulti m_wifiMulti;
    EventGroupHandle_t m_wifiEvt;

};