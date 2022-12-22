#include "wifi_task.h"
#include "Rtos/RtosUtils.h"
#include "Rtos/logger.h"
#include "App/app_context.h"

#include "wifi_manager.h"

//------------------------------------------------------------------------------
#define WIFI_TASK_STACK_SIZE (8192*2)
#define WIFI_TASK_PRIORITY      (4)

#define WIFI_TIMEOUT_MS      10000 // 10 second WiFi connection timeout

#define MODULE_NAME "[WIFI]"

//------------------------------------------------------------------------------
WifiTask::WifiTask(const WifiConfigData& wifiCfg)
    : Task("WifiTask", WIFI_TASK_STACK_SIZE, WIFI_TASK_PRIORITY, CONFIG_ARDUINO_RUNNING_CORE)
    , m_wifiCfg(wifiCfg)
{
    // Create Event Group
    m_wifiEvt = xEventGroupCreate();
    if (!m_wifiEvt)
    {
        logger::err("%s event group create failed!", MODULE_NAME);
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
    const size_t captivePortalTimeout = 600; //seconds
    size_t captivePortalTimeoutCnt = 0; //seconds

    WifiManager wifiMgr(m_wifiCfg.ap_hostname);
    if (wifiMgr.start())
    {
        xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_CONNECTED);
    }

    for(;;)
    {
        if (wifiMgr.isConnected())
        {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }

        // If OTA update active, do not make any reconnections in both AP and STATION modes
        if (AppCtx.appStatus() & AppStatusType::OTA_UPDATE_RUNNING)
        {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_DISCONNECTED);

        // Try to reconnect to station mode after timeout
        if (wifiMgr.isAPModeActive())
        {
            if (captivePortalTimeoutCnt++ > captivePortalTimeout)
            {
                captivePortalTimeoutCnt = 0;
                if (wifiMgr.start())
                {
                    logger::inf("%s Succesfully connected to wifi, server is running", MODULE_NAME);
                    xEventGroupSetBits(m_wifiEvt, WifiEvent::WIFI_CONNECTED);
                }
            }
        }

        // Lost wifi connection, try to reconnect
        if (wifiMgr.isStationModeActive() && !wifiMgr.isConnected())
        {
            logger::err("%s Connection failed, waiting for %d seconds...",
                        MODULE_NAME, WIFI_TIMEOUT_MS/1000);
            if (wifiMgr.start())
            {
                logger::inf("%s Wifi reconnected!", MODULE_NAME);
            }
        }

        wifiMgr.process();
        vTaskDelay(sleepTime);
    }
}

//------------------------------------------------------------------------------
