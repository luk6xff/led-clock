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
#include "App/app_shared.h"

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

    if (!MDNS.begin(AppCfg.getCurrent().wifi.ap_hostname))
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
    server.on("/dev-app-sysinfo", HTTP_GET, [] (AsyncWebServerRequest *request)
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
        String response = "{\"status\":\"error\"}";
        uint16_t errorCode = 400;
        for (const auto& h : m_cfgSaveHandleMap)
        {
            if (jsonObj.containsKey(h.first))
            {
                if (h.second(jsonObj))
                {
                    response = "{\"status\":\"success\"}";
                    errorCode = 200;
                }
                break;
            }
        }
        request->send(errorCode, "application/json", response);
        utils::inf("/dev-cfg-save response: %s", response.c_str());
    });
    server.addHandler(cfgSaveHandler);


    // Configuration read handler
    setCfgReadHandlers();
    server.on("/dev-cfg-read", HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        const int paramsNum = request->params();
        uint16_t errorCode = 400;
        std::string response = "{\"status\":\"error\"}";
        if (paramsNum == 1)
        {
            AsyncWebParameter* p = request->getParam(0);
            for (const auto& h : m_cfgReadHandleMap)
            {
                if (p->value() == (String(h.first)))
                {
                    response = h.second();
                    errorCode = 200;
                    break;
                }
            }

        }

        request->send(errorCode, "application/json", response.c_str());
        utils::inf("/dev-cfg-read response: %s", response.c_str());
    });


    // App reset
    server.on("/dev-app-reset", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "success");
        utils::inf("App is being restarted...");
        ESP.restart();
    });


    // App print some text
    AsyncCallbackJsonWebHandler *appPrintTextHandler = new AsyncCallbackJsonWebHandler("/dev-app-print-text",
                                                [this](AsyncWebServerRequest *request, JsonVariant &json)
    {
        JsonObject jsonObj = json.as<JsonObject>();
        String response = "{\"status\":\"error\"}";
        uint16_t errorCode = 400;
        if (jsonObj.containsKey("txt"))
        {
            String txt = jsonObj["txt"];

            response = "{\"status\":\"success\"}";
            errorCode = 200;
            AppSh.putDisplayMsg(txt.c_str(), txt.length());
            utils::inf("/dev-app-print-text TXT: %s", txt.c_str());
        }
        request->send(errorCode, "application/json", response);
        utils::inf("/dev-app-print-text response: %s", response.c_str());
    });
    server.addHandler(appPrintTextHandler);


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
        request->send(404, "text/plain", " <<< Not found >>> ");
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
                // LU_TODO add return value from parser
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
void WebServerTask::setCfgReadHandlers()
{
    m_cfgReadHandleMap = {

        {WIFI_CFG_KEY, [this]()
            {
                return AppCfg.getCurrent().wifi.toJson();
            }
        },

        {TIME_CFG_KEY , [this]()
            {
                return AppCfg.getCurrent().time.toJson();
            }
        },

        {WEATHER_CFG_KEY, [this]()
            {
                return AppCfg.getCurrent().weather.toJson();
            }
        },

        {RADIO_CFG_KEY, [this]()
            {
                return AppCfg.getCurrent().radioSensor.toJson();
            }
        },

        {DISPLAY_CFG_KEY, [this]()
            {
                return AppCfg.getCurrent().display.toJson();
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
