#include "ntp_task.h"
#include "system_time.h"
#include "App/rtos_common.h"
#include "App/wifi_task.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define NTP_TASK_STACK_SIZE (4096)
#define NTP_TASK_PRIORITY      (3)

#define MODULE_NAME "[NTP]"

//------------------------------------------------------------------------------
NtpTask::NtpTask(NtpSettings& ntpCfg, const EventGroupHandle_t& wifiEvtHandle)
    : Task("NtpTask", NTP_TASK_STACK_SIZE, NTP_TASK_PRIORITY, CONFIG_ARDUINO_RUNNING_CORE)
    , m_ntpCfg(m_ntpCfg)
    , m_wifiEvtHandle(wifiEvtHandle)
{
    m_ntpTimeQ = xQueueCreate(1, sizeof(DateTime));
    if (!m_ntpTimeQ)
    {
        err("%s m_ntpTimeQ has not been created!.", MODULE_NAME);
    }
}

//------------------------------------------------------------------------------
const QueueHandle_t& NtpTask::getNtpTimeQ()
{
    return m_ntpTimeQ;
}

//------------------------------------------------------------------------------
void NtpTask::run()
{
    const TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    WifiTask::WifiEvent lastWifiState = WifiTask::WIFI_INVALID;
    //Ntp ntp(m_ntpCfg); // LU_TODO
    Ntp ntp(NtpSettings(0, (1000*3600), "time.google.com", "pl.pool.ntp.org", "pool.ntp.org"));
    DateTime dt;

    if (!m_wifiEvtHandle)
    {
        kill();
    }

    for(;;)
    {   
        const EventBits_t wifiEv = xEventGroupWaitBits(
                                    m_wifiEvtHandle,// WIFI Event group
                                    WifiTask::WIFI_CONNECTED |
                                    WifiTask::WIFI_DISCONNECTED, // bits to wait for
                                    pdFALSE,        // no clear
                                    pdFALSE,        // wait for all bits
                                    portMAX_DELAY); // timeout

        if ((wifiEv & WifiTask::WIFI_CONNECTED) != 0)
        {
            lastWifiState = WifiTask::WIFI_CONNECTED;
        }
        else if ((wifiEv & WifiTask::WIFI_DISCONNECTED) != 0)
        {
            lastWifiState = WifiTask::WIFI_DISCONNECTED;
        }

        // Or just this :D
        // if(!WiFi.isConnected())
        // {
        //     vTaskDelay(sleepTime);
        //     continue;
        // }

        if (lastWifiState == WifiTask::WIFI_CONNECTED)
        {
            if (ntp.updateTime())
            {
                DateTime dt(ntp.getCurrentTime());
                dbg("%s UTC:%s", MODULE_NAME, dt.timestamp().c_str());
                // Update in Systime clock task
                if (m_ntpTimeQ)
                {
                    xQueueOverwrite(m_ntpTimeQ, &dt);
                }
            }
        }

        vTaskDelay(sleepTime);
    }
}

//------------------------------------------------------------------------------