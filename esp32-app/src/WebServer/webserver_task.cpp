#include "webserver_task.h"
#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#include <ESPmDNS.h>
#include "App/rtos_common.h"
#include "Wifi/wifi_task.h"
#include "App/utils.h"
#include "App/devinfo.h"
#include "AsyncJson.h"
#include <functional>
#include "App/app_config.h"

//------------------------------------------------------------------------------
#define WEBSERVER_TASK_STACK_SIZE (8192*4)
#define WEBSERVER_TASK_PRIORITY      (2)

#define MODULE_NAME "[WEBS]"
#define U_PART U_SPIFFS

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

    using namespace std::placeholders;
    m_server.begin();
    Update.onProgress(std::bind(&WebServerTask::printOtaUpdateProgress, this, _1, _2));

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

//------------------------------------------------------------------------------
void WebServerTask::registerHandlers(AsyncWebServer& server)
{
    // Update Device info tab
    server.on("/devinfo", HTTP_GET, [] (AsyncWebServerRequest *request)
    {
        DeviceInfo devInfo;
        request->send(200, "application/json", devInfo.createDevInfoTable());
    });

    // Configuration update handler
    setCfgSaveHandlers();
    AsyncCallbackJsonWebHandler *cfgSaveHandler = new AsyncCallbackJsonWebHandler("/dev-cfg-save",
                                                [this](AsyncWebServerRequest *request, JsonVariant &json)
    {
        JsonObject jsonObj = json.as<JsonObject>();
        String response = "{\"status\":\"failure\"}";
        for (const auto& h : m_cfgSaveHandleMap)
        {
            if (jsonObj.containsKey(h.first))
            {
                if (h.second(jsonObj))
                {
                    response = "{\"status\":\"success\"}";
                }
                break;
            }
        }
        request->send(200, "application/json", response);
        utils::inf("/dev-cfg-save response: %s", response.c_str());
    });
    server.addHandler(cfgSaveHandler);

    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });


    // OTA update
    server.on("/update", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        [this](AsyncWebServerRequest *request, const String& filename,
            size_t index, uint8_t *data, size_t len, bool final){
            handleDoOtaUpdate(request, filename, index, data, len, final);
        }
    );

    server.onNotFound([](AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "Not found");
    });


    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
}

//------------------------------------------------------------------------------
void WebServerTask::setCfgSaveHandlers()
{
    const char *cfgTimeKey = "dev-cfg-time";
    const char *cfgWeatherKey = "dev-cfg-weather";
    const char *cfgRadioKey = "dev-cfg-radio";
    const char *cfgDisplayKey = "dev-cfg-wifi";
    const char *cfgOtherKey = "dev-cfg-other";

    m_cfgSaveHandleMap = {

        {WIFI_CFG_KEY, [this](const JsonObject& json)
            {
                WifiSettings cfg = AppCfg.getCurrent().wifi;
                cfg.fromJson(json);
                utils::inf("%s", cfg.toJson().c_str());
                // LU_TODO save
                return true;
            }
        },

        {cfgTimeKey , [this, cfgTimeKey](const JsonObject& json)
            {
                JsonArray arr = json[cfgTimeKey].as<JsonArray>();
                for (const auto& v : arr)
                {
                    String id = v["id"];
                    String val = v["value"];
                    utils::inf("[%s]  id:%s, value:%s", cfgTimeKey, id.c_str(), val.c_str());
                }
                return true;
            }
        },

        {cfgWeatherKey, [this, cfgWeatherKey](const JsonObject& json)
            {
                JsonArray arr = json[cfgWeatherKey].as<JsonArray>();
                for (const auto& v : arr)
                {
                    String id = v["id"];
                    String val = v["value"];
                    utils::inf("[%s]  id:%s, value:%s", cfgWeatherKey, id.c_str(), val.c_str());
                }
                return true;
            }
        },

        {cfgRadioKey, [this, cfgRadioKey](const JsonObject& json)
            {
                JsonArray arr = json[cfgRadioKey].as<JsonArray>();
                for (const auto& v : arr)
                {
                    String id = v["id"];
                    String val = v["value"];
                    utils::inf("[%s]  id:%s, value:%s", cfgRadioKey, id.c_str(), val.c_str());
                }
                return true;
            }
        },

        {cfgDisplayKey, [this, cfgDisplayKey](const JsonObject& json)
            {
                JsonArray arr = json[cfgDisplayKey].as<JsonArray>();
                for (const auto& v : arr)
                {
                    String id = v["id"];
                    String val = v["value"];
                    utils::inf("[%s]  id:%s, value:%s", cfgDisplayKey, id.c_str(), val.c_str());
                }
                return true;
            }
        },

        {cfgOtherKey, [this, cfgOtherKey](const JsonObject& json)
            {
                JsonArray arr = json[cfgOtherKey].as<JsonArray>();
                for (const auto& v : arr)
                {
                    String id = v["id"];
                    String val = v["value"];
                    utils::inf("[%s]  id:%s, value:%s", cfgOtherKey,  id.c_str(), val.c_str());
                }
                return true;
            }
        },
    };
}

//------------------------------------------------------------------------------
void WebServerTask::handleDoOtaUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        utils::inf("Update starting...");
        m_otaFileContentLen = request->contentLength();
        // If filename includes spiffs, update the spiffs partition
        int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
        {
            Update.printError(Serial);
        }
    }

    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
    }

    if (final)
    {
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
        response->addHeader("Refresh", "20");
        response->addHeader("Location", "/");
        request->send(response);
        if (!Update.end(true))
        {
            Update.printError(Serial);
        }
        else
        {
            utils::inf("Update completed!");
            ESP.restart();
        }
    }
}

//------------------------------------------------------------------------------
void WebServerTask::printOtaUpdateProgress(size_t prg, size_t sz)
{
    utils::inf("Progress: %d%%\n", (prg*100) / m_otaFileContentLen);
}

//------------------------------------------------------------------------------
