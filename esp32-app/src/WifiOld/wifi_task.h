#pragma once
#include "Rtos/task.h"
#include "wifi_settings.h"
#include <WiFiMulti.h>
#include <WiFi.h>

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