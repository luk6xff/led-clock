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
WebServerTask::WebServerTask()
    : Task("WebServerTask", WEBSERVER_TASK_STACK_SIZE, WEBSERVER_TASK_PRIORITY,
            CONFIG_ARDUINO_RUNNING_CORE)
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

    // Configuration read handler
    server.on("/dev-cfg-read", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string cfg =
        request->send(200, "application/json", String(ESP.getFreeHeap()));
    });

    // Free Heap size
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
    m_cfgSaveHandleMap = {

        {WIFI_CFG_KEY, [this](const JsonObject& json)
            {
                WifiSettings cfg = AppCfg.getCurrent().wifi;
                cfg.fromJson(json);
                utils::inf("%s", cfg.toJson().c_str());
                return AppCfg.saveWifiSettings(cfg);
            }
        },

        {TIME_CFG_KEY , [this](const JsonObject& json)
            {
                SystemTimeSettings cfg = AppCfg.getCurrent().time;
                cfg.fromJson(json);
                utils::inf("%s", cfg.toJson().c_str());
                return AppCfg.saveSystemTimeSettings(cfg);
            }
        },

        {WEATHER_CFG_KEY, [this](const JsonObject& json)
            {
                WeatherSettings cfg = AppCfg.getCurrent().weather;
                cfg.fromJson(json);
                utils::inf("%s", cfg.toJson().c_str());
                return AppCfg.saveWeatherSettings(cfg);
            }
        },

        {RADIO_CFG_KEY, [this](const JsonObject& json)
            {
                RadioSensorSettings cfg = AppCfg.getCurrent().radioSensor;
                cfg.fromJson(json);
                utils::inf("%s", cfg.toJson().c_str());
                return AppCfg.saveRadioSensorSettings(cfg);
            }
        },

        {DISPLAY_CFG_KEY, [this](const JsonObject& json)
            {
                DisplaySettings cfg = AppCfg.getCurrent().display;
                cfg.fromJson(json);
                utils::inf("%s", cfg.toJson().c_str());
                return AppCfg.saveDisplaySettings(cfg);
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
