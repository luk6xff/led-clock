#pragma once

/**
 * @brief WifiManager declaration
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2022-03-30
 */

#include <ArduinoJson.h>
#include <DNSServer.h>
#include "wifi_manager.h"
#include "WebServer/webserver.h"


class WifiManager
{

public:
    explicit WifiManager(const char* hostname);

    bool start();
    void process();

    bool isConnected();
    bool isAPModeActive();
    bool isStationModeActive();

private:
    bool wifiConnect(const char *ssid, const char *password);
    void reset();

    bool startCaptivePortal();
    bool startWebPortal();

private:
    const char *k_apHostname;
    const uint8_t k_dnsPort = 53;

    WebServer m_webServer;
    IPAddress m_apIP;
    DNSServer m_dnsServer;

};