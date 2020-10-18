#include "wifi_task.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define WIFI_TASK_STACK_SIZE (8192)
#define WIFI_TASK_PRIORITY      (4)

#define WIFI_TIMEOUT_MS      20000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 30000 // Wait 30 seconds after a failed connection attempt


//------------------------------------------------------------------------------
WifiTask::WifiTask(const WifiSettings& wifiCfg)
    : Task("WifiTask", WIFI_TASK_STACK_SIZE, WIFI_TASK_PRIORITY, CONFIG_ARDUINO_RUNNING_CORE)
    , m_wifiCfg(wifiCfg)
{
    // Create Event Group
    m_wifiEvt = xEventGroupCreate();
    if (!m_wifiEvt)
    {
        err("WIFI event group create failed!");
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
    const TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    WiFi.setAutoConnect(0);
    WiFi.softAPdisconnect();
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    for(;;)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_DISCONNECTED);
        WiFi.mode(WIFI_STA);
        WiFi.begin(m_wifiCfg.ssid0, m_wifiCfg.pass0);

        // Wait until connected or timeou exceeded
        inf("[WIFI] Connecting...");
        const uint32_t startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED &&
                millis() - startAttemptTime < WIFI_TIMEOUT_MS)
        {

        }

        if (WiFi.status() != WL_CONNECTED)
        {
            err("[WIFI] Connection failed, waiting for %d seconds...", WIFI_RECOVER_TIME_MS/1000);
            vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);
            continue;
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            inf("[WIFI] Connected to:%s, IPaddress:%s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
            xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_CONNECTED);
        }
        vTaskDelay(sleepTime);
    }
}


//------------------------------------------------------------------------------
