#pragma once

/**
 * @brief WebServer Implementation
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-11-18
 */

#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <OtaUpdate/ota.h>
#include "webconfig.h"

#include <functional>
#include <map>

class WebServer
{
    enum class RequestResponseType : uint8_t
    {
        SUCCESS = 0,
        ERROR,
    };

    struct RequestResponseData
    {
        int errorCode;
        String type;
        String payload;
    };

public:
    explicit WebServer(const char* hostname);
    bool startWebServer();

private:
    void registerHandlers(AsyncWebServer& server);
    void printOtaUpdateProgress(size_t prg, size_t sz);
    RequestResponseData prepareJsonResponse(RequestResponseType rt, const String& payload);

private:
    const char *k_apHostname;
    AsyncWebServer m_server;
    WebConfig m_webConfig;
    size_t m_otaFileContentLen;
    bool m_webServerRunning;
    OtaUpdate m_ota;
};