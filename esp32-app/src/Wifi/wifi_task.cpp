#include "wifi_task.h"
#include "App/rtos_common.h"
#include "App/utils.h"
//#include <WiFiManager.h>

#include "WebServer/webserver.h"

//------------------------------------------------------------------------------
#define WIFI_TASK_STACK_SIZE (8192*2)
#define WIFI_TASK_PRIORITY      (4)

#define WIFI_TIMEOUT_MS      10000 // 10 second WiFi connection timeout

#define MODULE_NAME "[WIFI]"

//------------------------------------------------------------------------------
WifiTask::WifiTask(const WifiSettings& wifiCfg)
    : Task("WifiTask", WIFI_TASK_STACK_SIZE, WIFI_TASK_PRIORITY, CONFIG_ARDUINO_RUNNING_CORE)
    , m_wifiCfg(wifiCfg)
{
    // Create Event Group
    m_wifiEvt = xEventGroupCreate();
    if (!m_wifiEvt)
    {
        utils::err("%s event group create failed!", MODULE_NAME);
    }
}

//------------------------------------------------------------------------------
const EventGroupHandle_t& WifiTask::getWifiEvtHandle()
{
    return m_wifiEvt;
}

//------------------------------------------------------------------------------
void WifiTask::run()
{
    const TickType_t sleepTime = (100 / portTICK_PERIOD_MS);
    uint8_t wifiConnectionFailureCnt = 0;
    // WiFi.setAutoConnect(0);
    // WiFi.softAPdisconnect();
    // WiFi.setAutoConnect(true);
    // WiFi.setAutoReconnect(true);
    // WiFiManager wm;
    // wm.setSaveConfigCallback([](String ssid, String pass)
    // {
    //     // LU_TODO Store new wifi settings
    // });
    // //wm.resetSettings();
    WebServer server(m_wifiCfg.ap_hostname);
    server.start();
    for(;;)
    {
        // if (WiFi.status() == WL_CONNECTED)
        // {
        //     vTaskDelay(10000 / portTICK_PERIOD_MS);
        //     continue;
        // }

        xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_DISCONNECTED);
        // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
        // WiFi.begin(m_wifiCfg.ssid, m_wifiCfg.pass);

        // // Wait until connected or timeou exceeded
        // utils::inf("%s Connecting...", MODULE_NAME);
        // const uint32_t startAttemptTime = millis();
        // while (WiFi.status() != WL_CONNECTED &&
        //         millis() - startAttemptTime < WIFI_TIMEOUT_MS)
        // {

        // }

        // if (WiFi.status() != WL_CONNECTED)
        // {
        //     utils::err("%s Connection failed, waiting for %d seconds...",
        //         MODULE_NAME, WIFI_TIMEOUT_MS/1000);
        //     vTaskDelay(WIFI_TIMEOUT_MS / portTICK_PERIOD_MS);
        //     wifiConnectionFailureCnt++;
        //     if (wifiConnectionFailureCnt >= 3)
        //     {
        //         // If no connection run Captive portal
        //         if (!wm.autoConnect("LedClock", m_wifiCfg.pass))
        //         {
        //             utils::dbg("%s Starting CaptivePortal", MODULE_NAME);
        //         }
        //         wifiConnectionFailureCnt = 0;
        //     }
        //     else
        //     {
        //         continue;
        //     }
        // }
        server.process();

        // if (WiFi.status() == WL_CONNECTED)
        // {
        //     utils::inf("%s Connected to:%s, IPaddress:%s", MODULE_NAME,
        //         WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
        //     xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_CONNECTED);
        // }
        vTaskDelay(sleepTime);
    }
}


//------------------------------------------------------------------------------
