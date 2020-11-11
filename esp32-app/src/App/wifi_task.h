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
        memcpy(ssid, admin, WIFI_SETTINGS_LEN);
        memcpy(pass, admin, WIFI_SETTINGS_LEN);
    }

    char ssid[WIFI_SETTINGS_LEN];
    char pass[WIFI_SETTINGS_LEN];
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