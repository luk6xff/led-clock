#pragma once

/**
 * @brief LedClock WebServer Implementation
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-11-18
 */

#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

#include <functional>
#include <map>

class WebServer
{
    enum class WifiMode : uint8_t
    {
        AP,
        STATION
    };

public:
    explicit WebServer(const char* hostname);

    bool start();
    void process();

private:

    void registerHandlers(AsyncWebServer& server);
    void setCfgSaveHandlers();
    void setCfgReadHandlers();

    /**
     * @brief OTA update handler
     * Based on: https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino
     */
    void handleDoOtaUpdate(AsyncWebServerRequest *request, const String& filename,
                           size_t index, uint8_t *data, size_t len, bool final);

    void printOtaUpdateProgress(size_t prg, size_t sz);

    void startAP();
    bool startAPApi();
    bool startApi();

    bool wifiConnected();
    bool wifiConnect(const char* ssid, const char* password);

private:

    AsyncWebServer m_server;
    std::map<String, std::function<bool(const JsonObject&)>> m_cfgSaveHandleMap;
    std::map<String, std::function<std::string(void)>> m_cfgReadHandleMap;
    size_t m_otaFileContentLen;
    const char *k_apHostname;

    const uint8_t k_dnsPort = 53;
    IPAddress m_apIP;
    DNSServer m_dnsServer;

    WifiMode m_wifiMode;
};