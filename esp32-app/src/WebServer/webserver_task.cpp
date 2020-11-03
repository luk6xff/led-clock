#include "webserver_task.h"
#include "App/rtos_common.h"
#include "App/wifi_task.h"
#include "App/utils.h"

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
    m_server.begin();
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
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request)
    {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
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

    server.onNotFound([](AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "Not found");
    });
}

//------------------------------------------------------------------------------
