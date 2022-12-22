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
#include "AsyncJson.h"
#include "Rtos/log.h"
#include "App/devinfo.h"
#include "App/app_context.h"
#include "App/app_vars.h"
#include <functional>

//------------------------------------------------------------------------------
#define MODULE_NAME "[WEBS]"

//------------------------------------------------------------------------------
WebServer::WebServer(const char *hostname)
    : k_apHostname(hostname)
    , m_server(80)
    , m_Config(AppCfg)
    , m_otaFileContentLen(1)
    , m_webServerRunning(false)
{

}

//------------------------------------------------------------------------------
void WebServer::registerHandlers(AsyncWebServer& server)
{
    // Update Device info tab
    server.on("/sysinfo", HTTP_GET, [this] (AsyncWebServerRequest *request)
    {
        DeviceInfo devInfo;
        WebServer::RequestResponseData resp = prepareJsonResponse(WebServer::RequestResponseType::SUCCESS, devInfo.createDevInfoTable());
        request->send(resp.errorCode, resp.type, resp.payload);
    });


    // Configuration read handler
    server.on("/dev-cfg-read", HTTP_GET, [this] (AsyncWebServerRequest *request)
    {
        String payload;
        m_Config.getConfig(payload);
        WebServer::RequestResponseData resp = prepareJsonResponse(WebServer::RequestResponseType::SUCCESS, payload);
        request->send(resp.errorCode, resp.type, resp.payload);
        log::dbg("/dev-cfg-read response: %s", resp.payload.c_str());
    });

    // Configuration save handler
    AsyncCallbackJsonWebHandler *cfgSetHandler = new AsyncCallbackJsonWebHandler("/dev-cfg-save",
                                                [this](AsyncWebServerRequest *request, JsonVariant &json)
    {
        JsonObject jsonObj = json.as<JsonObject>();
        String emptyPayload;
        WebServer::RequestResponseData resp;

        if (m_Config.setConfig(jsonObj))
        {
            resp = prepareJsonResponse(WebServer::RequestResponseType::SUCCESS, emptyPayload);
            log::inf("/dev-cfg-save SUCCESS");
        }
        else
        {
            resp = prepareJsonResponse(WebServer::RequestResponseType::ERROR, emptyPayload);
            log::inf("/dev-cfg-save ERROR");
        }
        request->send(resp.errorCode, resp.type, resp.payload);
        log::dbg("/dev-cfg-save response: %s", resp.payload.c_str());
    });
    server.addHandler(cfgSetHandler);


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
            log::dbg("Received DT value: %s, %d", val.c_str(), t);
            DateTime dt(t);
            if (AppCtx.putDateTimeMsg(dt))
            {
                errorCode = 200;
                response = "{\"status\":\"success\"}";
            }
        }

        request->send(errorCode, "application/json", response.c_str());
        log::inf("/dev-app-set-dt response: %s", response.c_str());
    });


    // App get date and time
    server.on("/dev-app-get-dt", HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
        DynamicJsonDocument doc(capacity);
        doc["dt"] = "0";
        uint16_t errorCode = 400;
        DateTime dt = AppCtx.getAppDt();
        if (dt.isValid())
        {
            errorCode = 200;
            doc["dt"] = String(dt.unixtime());
        }

        String resp;
        serializeJson(doc, resp);
        request->send(errorCode, "application/json", resp.c_str());
        log::inf("/dev-app-get-dt response: %s", resp.c_str());
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
            AppCtx.putDisplayMsg(txt.c_str(), txt.length());
            log::inf("/dev-app-print-text TXT: %s", txt.c_str());
        }
        request->send(errorCode, "application/json", response);
        log::inf("/dev-app-print-text response: %s", response.c_str());
    });
    server.addHandler(appPrintTextHandler);


    // App reset
    server.on("/dev-app-reset", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "success");
        log::inf("App is being restarted...");
        ESP.restart();
    });


    // Free Heap size
    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });


    // OTA update
    server.on("/update/identity", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"id\": \"ID\", \"hardware\": \"ESP32\"}");
    });

    server.on("/update", HTTP_POST, [&](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
            response->addHeader("Connection", "close");
            request->send(response);
        },
        [this](AsyncWebServerRequest *request, const String& filename,
            size_t index, uint8_t *data, size_t len, bool final)
        {
            m_ota.handleOtaRequest(request, filename, index, data, len, final);
        });


    server.onNotFound([](AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", " <<< ledclock 2020 - Not found >>> ");
    });

    // Set serve static files and cache responses for 10 minutes (600 seconds)
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html") \
                                        .setCacheControl("max-age=600") \
                                        .setLastModified(APP_DATE_LAST_MODIFIED);
}

//------------------------------------------------------------------------------
WebServer::RequestResponseData WebServer::prepareJsonResponse(WebServer::RequestResponseType rt, const String& payload)
{
    WebServer::RequestResponseData resp;
    resp.type = "application/json";

    switch(rt)
    {
        case WebServer::RequestResponseType::SUCCESS:
        {
            resp.errorCode = 200;
            // Add response status to payload
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            doc["status"] = "success";
            serializeJson(doc, resp.payload);
            break;
        }

        case WebServer::RequestResponseType::ERROR:
        default:
        {
            resp.errorCode = 400;
            // Add response status to payload
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            doc["status"] = "error";
            serializeJson(doc, resp.payload);
            break;
        }
    }

    return resp;
}

//------------------------------------------------------------------------------
void WebServer::printOtaUpdateProgress(size_t prg, size_t sz)
{
    log::inf("Progress: %d%%\n", (prg*100) / m_otaFileContentLen);
}

//------------------------------------------------------------------------------
bool WebServer::startWebServer()
{
    // Server is started only once
    if (!m_webServerRunning)
    {
        registerHandlers(m_server);

        // Init filesystem
        if (!SPIFFS.begin(true))
        {
            log::err("Error on SPIFFS begin!");
            return false;
        }

        using namespace std::placeholders;
        Update.onProgress(std::bind(&WebServer::printOtaUpdateProgress, this, _1, _2));

        // Run the server
        m_server.begin();

        // Run mdns service
        if (!MDNS.begin(k_apHostname))
        {
            log::err("Error setting up MDNS responder!");
            return false;
        }
        else
        {
            MDNS.addService("http", "tcp", 80);
        }

        m_webServerRunning = true;
        log::inf("--> WebServer is running...");
    }

    return m_webServerRunning;
}

//------------------------------------------------------------------------------
