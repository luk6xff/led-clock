#include "webserver_task.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include "App/rtos_common.h"
#include "App/wifi_task.h"
#include "App/utils.h"
#include "App/devinfo.h"

//------------------------------------------------------------------------------
#define WEBSERVER_TASK_STACK_SIZE (8192*2)
#define WEBSERVER_TASK_PRIORITY      (2)

#define MODULE_NAME "[WEBS]"

//------------------------------------------------------------------------------
//WebServerTask::WebServerTask(const WebServerSettings& webServerCfg)
WebServerTask::WebServerTask()
    : Task("WebServerTask", WEBSERVER_TASK_STACK_SIZE, WEBSERVER_TASK_PRIORITY, CONFIG_ARDUINO_RUNNING_CORE)
    //, m_webServerCfg(webServerCfg)
{

}


//------------------------------------------------------------------------------
void WebServerTask::run()
{
    const TickType_t sleepTime = (30000 / portTICK_PERIOD_MS);
    AsyncWebServer m_server(80);
    registerHandlers(m_server);

    // Init filesystem
    if (!SPIFFS.begin(true))
    {
        utils::err("Error on SPIFFS begin!");
    }


    m_server.begin();

    if (!MDNS.begin("ledclock"))
    {
        utils::err("Error setting up MDNS responder!");
    }
    else
    {
        MDNS.addService("http", "tcp", 80);
    }

    for(;;)
    {
        if (!WiFi.isConnected())
        {
            vTaskDelay(sleepTime);
            continue;
        }
        vTaskDelay(sleepTime);
    }
}

const char* PARAM_MESSAGE = "message";
//------------------------------------------------------------------------------
void WebServerTask::registerHandlers(AsyncWebServer& server)
{
    // Update Device info tab
    server.on("/devinfotable.json", HTTP_GET, [] (AsyncWebServerRequest *request)
    {
        DeviceInfo devInfo;
        request->send(200, "application/json", devInfo.createDevInfoTable());
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    server.onNotFound([](AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "Not found");
    });

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
}

//------------------------------------------------------------------------------
