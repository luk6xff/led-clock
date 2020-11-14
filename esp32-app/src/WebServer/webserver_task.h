#pragma once

#include "App/task.h"
#include "webserver_settings.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include <functional>
#include <map>

class WebServerTask : public Task
{
public:
    //explicit WebServerTask(const WebServerSettings& webServerCfg);
    explicit WebServerTask();

private:
    virtual void run() override;

    void registerHandlers(AsyncWebServer& server);
    void setCfgSaveHandlers();

    /**
     * @brief OTA update handler
     * Based on: https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino
     */
    void handleDoOtaUpdate(AsyncWebServerRequest *request, const String& filename,
                           size_t index, uint8_t *data, size_t len, bool final);

    void printOtaUpdateProgress(size_t prg, size_t sz);

private:
    //const WebServerSettings& m_webServerCfg;

    std::map<String, std::function<bool(const JsonObject&)>> m_cfgSaveHandleMap;
    size_t m_otaFileContentLen;
};