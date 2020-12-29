#include "wifi_task.h"
#include "App/rtos_common.h"
#include "App/utils.h"
#include "App/app_context.h"

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
    const TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    uint8_t wifiConnectionFailureCnt = 0;
    const size_t captivePortalTimeout = 600; //seconds
    size_t captivePortalTimeoutCnt = 0; //seconds

    WebServer server(m_wifiCfg.ap_hostname);
    if (server.start())
    {
        xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_CONNECTED);
    }
    for(;;)
    {
        if (server.wifiConnected())
        {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }

        // If OTA update active, do not make any reconnections in both AP and STATION modes
        if (AppCtx.getAppOtaUpdateStatus())
        {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_DISCONNECTED);

        // Try to recconnect after timeout to AP
        if (server.isAPModeActive())
        {
            if (captivePortalTimeoutCnt++ > captivePortalTimeout)
            {
                captivePortalTimeoutCnt = 0;
                if (server.start())
                {
                    utils::inf("%s Succesfully connected to wifi, server is running", MODULE_NAME);
                    xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_CONNECTED);
                }
            }
        }

        // Lost connection, try to reconnect
        if (server.isStationModeActive() && !server.wifiConnected())
        {
            utils::err("%s Connection failed, waiting for %d seconds...",
                        MODULE_NAME, WIFI_TIMEOUT_MS/1000);
            vTaskDelay(WIFI_TIMEOUT_MS / portTICK_PERIOD_MS);
            wifiConnectionFailureCnt++;
            if (wifiConnectionFailureCnt >= 3)
            {
                utils::inf("%s Starting CaptivePortal...", MODULE_NAME);
                if (server.runCaptivePortal())
                {
                    utils::inf("%s CaptivePortal is running", MODULE_NAME);
                }
                else
                {
                    utils::dbg("%s CaptivePortal start failure!", MODULE_NAME);
                }

                wifiConnectionFailureCnt = 0;
            }
            else
            {
                continue;
            }
        }

        server.process();

        vTaskDelay(sleepTime);
    }
}


//------------------------------------------------------------------------------
