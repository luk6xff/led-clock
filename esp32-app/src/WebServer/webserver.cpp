/**
 * @brief LedClock WebServer Implementation
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-11-18
 */

#include "webserver.h"
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
#include "esp_wifi.h"

//------------------------------------------------------------------------------
#define WEBSERVER_TASK_STACK_SIZE (8192*4)
#define WEBSERVER_TASK_PRIORITY      (2)

#define MODULE_NAME "[WEBS]"
#define U_PART U_SPIFFS

//------------------------------------------------------------------------------
WebServer::WebServer(const char* hostname)
    : m_server(80)
    , k_apHostname(hostname)
    , m_wifiMode(WifiMode::AP)
{

}

//------------------------------------------------------------------------------
bool WebServer::start()
{
    int attempt = 0;
    bool success = false;
    int wifiConnectAttempts = 5;

    while (attempt < wifiConnectAttempts && !success)
    {
        attempt++;
        utils::inf((""));
        utils::inf("Wifi connection attempt num: %d", attempt);
        success = wifiConnect(AppCfg.getCurrent().wifi.ssid, AppCfg.getCurrent().wifi.pass);
    }

    if (success)
    {
        startApi();
    }
    else
    {
        utils::inf("");
        utils::inf("Wifi connection could not be established");
    }

    if (m_wifiMode != WifiMode::STATION)
    {
        utils::inf("WifiMode AP (Captive Portal) starting...");
        runCaptivePortal();
    };
    return success;
}

//------------------------------------------------------------------------------
void WebServer::process()
{
    if (m_wifiMode == WifiMode::AP)
    {
        m_dnsServer.processNextRequest();
    }
}

//------------------------------------------------------------------------------
void WebServer::registerHandlers(AsyncWebServer& server)
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
            AsyncWebParameter *p = request->getParam(0);
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


    // App set date and time
    server.on("/dev-app-set-dt", HTTP_POST, [this](AsyncWebServerRequest *request)
    {
        uint16_t errorCode = 400;
        std::string response = "{\"status\":\"error\"}";
        if(request->hasParam("dt", true))
        {
            AsyncWebParameter *p = request->getParam("dt", true);
            String val = p->value();
            const uint32_t t = (uint32_t)val.toInt();
            utils::dbg("Received DT value: %s, %d", val.c_str(), t);
            DateTime dt(t);
            if (AppSh.putDateTimeMsg(dt))
            {
                errorCode = 200;
                response = "{\"status\":\"success\"}";
            }
        }

        request->send(errorCode, "application/json", response.c_str());
        utils::inf("/dev-app-set-dt response: %s", response.c_str());
    });


    // App get date and time
    server.on("/dev-app-get-dt", HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
        DynamicJsonDocument doc(capacity);
        doc["dt"] = "0";
        uint16_t errorCode = 400;
        DateTime dt = AppSh.getAppDt();
        if (dt.isValid())
        {
            errorCode = 200;
            doc["dt"] = String(dt.unixtime());
        }

        String resp;
        serializeJson(doc, resp);
        request->send(errorCode, "application/json", resp.c_str());
        utils::inf("/dev-app-get-dt response: %s", resp.c_str());
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


    // App reset
    server.on("/dev-app-reset", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "success");
        utils::inf("App is being restarted...");
        ESP.restart();
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
        request->send(404, "text/plain", " <<< Not found >>> ");
    });

    // Set serve static files and cache responses for 10 minutes (600 seconds)
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html").setCacheControl("max-age=600");
}

//------------------------------------------------------------------------------
void WebServer::setCfgSaveHandlers()
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
void WebServer::setCfgReadHandlers()
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
void WebServer::handleDoOtaUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final)
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
void WebServer::printOtaUpdateProgress(size_t prg, size_t sz)
{
    utils::inf("Progress: %d%%\n", (prg*100) / m_otaFileContentLen);
}

//------------------------------------------------------------------------------
bool WebServer::runCaptivePortal()
{
    startAP();
    return startAPApi();
}

//------------------------------------------------------------------------------
bool WebServer::isAPModeActive()
{
    return wifiMode() == WifiMode::AP;
}

//------------------------------------------------------------------------------
bool WebServer::isStationModeActive()
{
    return wifiMode() == WifiMode::STATION;
}

//------------------------------------------------------------------------------
const WebServer::WifiMode WebServer::wifiMode()
{
    return m_wifiMode;
}

//------------------------------------------------------------------------------
bool WebServer::wifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

//------------------------------------------------------------------------------
bool WebServer::wifiConnect(const char* ssid, const char* password)
{
    utils::inf("%s Waiting for WiFi to connect", MODULE_NAME);

    bool connected = false;
    int retry = 0;
    int wifiConnectRetries = 5;
    WiFi.begin(ssid, password[0] == '\0' ? NULL : password);

    while (retry < wifiConnectRetries && !connected)
    {
        utils::inf(".");
        connected = wifiConnected();
        if (connected)
        {
            utils::inf("%s Connected to:%s, IPaddress:%s", MODULE_NAME,
                WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

            m_wifiMode = WifiMode::STATION;
            WiFi.mode(WIFI_STA);
        }
        else
        {
            int wifiConnectInterval = 500;
            delay(wifiConnectInterval);
        }

        retry++;
    }

    return connected;
}

//------------------------------------------------------------------------------
bool WebServer::createWebServer()
{
    // Server is started only once
    if (!m_webServerRunning)
    {
        registerHandlers(m_server);

        // Init filesystem
        if (!SPIFFS.begin(true))
        {
            utils::err("Error on SPIFFS begin!");
            return false;
        }

        using namespace std::placeholders;
        Update.onProgress(std::bind(&WebServer::printOtaUpdateProgress, this, _1, _2));

        // Run the server
        m_server.begin();

        // Run mdns service
        if (!MDNS.begin(k_apHostname))
        {
            utils::err("Error setting up MDNS responder!");
            return false;
        }
        else
        {
            MDNS.addService("http", "tcp", 80);
        }

        m_webServerRunning = true;
    }

    return m_webServerRunning;
}

//------------------------------------------------------------------------------
void WebServer::startAP()
{
    m_wifiMode = WifiMode::AP;

    utils::inf("Starting Access Point");
    WiFi.mode(WIFI_OFF);
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true);
    delay(2000);
    WiFi.mode(WIFI_AP);
    String apHostname = k_apHostname;
    String apPass = AppCfg.getCurrent().wifi.ap_pass;
    if (apHostname == "")
    {
        apHostname = "ledclock";
    }
    if (apPass == "" || apPass.length() < 8 || apPass.length() > 63)
    {
        utils::inf("AccessPoint set password is INVALID or <8 or >63 chars, default `admin123` set instead");
        apPass = "admin123";
    }

    WiFi.softAP(apHostname.c_str(), apPass.c_str());

    utils::inf("Setting softAP Hostname:", apHostname.c_str());
    bool res =  WiFi.softAPsetHostname(apHostname.c_str());
    if (!res)
    {
        utils::err("Hostname: AP set failed!");
    }
    utils::dbg("Hostname: AP: %s", WiFi.softAPgetHostname());

    IPAddress localIp(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnetMask(255,255,255,0);
    WiFi.softAPConfig(localIp, gateway, subnetMask);

    delay(500);  // Need to wait to get IP
    utils::inf("AP Name: %s", k_apHostname);
    utils::inf("AP IP address: %s", WiFi.softAPIP().toString().c_str());

    //m_dnsServer.reset(new DNSServer);
    m_dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    m_dnsServer.start(k_dnsPort, "*", localIp);
}

//------------------------------------------------------------------------------
bool WebServer::startAPApi()
{
    // LU_TODO - createApWebServer() ?
    return createWebServer();
}

//------------------------------------------------------------------------------
bool WebServer::startApi()
{
    // Stop dns server if running
    m_dnsServer.stop();
    return createWebServer();
}

//------------------------------------------------------------------------------