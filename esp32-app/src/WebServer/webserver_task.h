#pragma once

#include "App/task.h"
#include "webserver_settings.h"
#include <ESPAsyncWebServer.h>

class WebServerTask : public Task
{
public:
    //explicit WebServerTask(const WebServerSettings& webServerCfg);
    explicit WebServerTask();

private:
    virtual void run() override;

    void registerHandlers(AsyncWebServer& server);

private:
    //const WebServerSettings& m_webServerCfg;
};