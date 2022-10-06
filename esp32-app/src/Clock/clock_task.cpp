#include "clock_task.h"
#include "App/utils.h"
#include "App/app_context.h"

//------------------------------------------------------------------------------
#define CLOCK_TASK_STACK_SIZE (8192)
#define CLOCK_TASK_PRIORITY     (6)

#define MODULE_NAME "[CLCK]"

//------------------------------------------------------------------------------
ClockTask::ClockTask(SystemTimeSettings& timeCfg,
                    const QueueHandle_t& ntpTimeQ)
    : Task("ClockTask", CLOCK_TASK_STACK_SIZE, CLOCK_TASK_PRIORITY, 0)
    , m_timeCfg(timeCfg)
    , m_ntpTimeQ(ntpTimeQ)
    , m_extTimeQ(AppCtx.getTimeQHandle())
    , m_timeQ(nullptr)
{
    m_timeQ = xQueueCreate(16, sizeof(DateTime));
    if (!m_timeQ)
    {
        log::err("%s m_timeQ has not been created!.", MODULE_NAME);
    }
    log::dbg("%s ClockTask::ClockTask created!", MODULE_NAME);
}

//------------------------------------------------------------------------------
const QueueHandle_t& ClockTask::getTimeQ()
{
    return m_timeQ;
}

//------------------------------------------------------------------------------
void ClockTask::run()
{
    const TickType_t timeMeasDelay = (200 / portTICK_PERIOD_MS);
    SystemTime time(m_timeCfg);
    DateTime dt;
    DateTime lastDt;
    for(;;)
    {

        // Disable if OTA Update
        if (AppCtx.appStatus() & AppStatusType::OTA_UPDATE_RUNNING)
        {
            vTaskDelay(timeMeasDelay);
            continue;
        }

        dt = time.getTime();
        if (dt.isValid())
        {
            AppCtx.clearAppStatus(AppStatusType::RTC_ERROR);
            if (lastDt != dt)
            {
                if (pushTimeMsg(dt))
                {
                    log::dbg("%s DT:%s", MODULE_NAME, dt.timestamp().c_str());
                    // Update System time
                    AppCtx.setAppDt(dt);
                }
            }
            lastDt = dt;
        }
        else
        {
            AppCtx.setAppStatus(AppStatusType::RTC_ERROR);
            log::err("%s Invalid DateTime read from RTC: %s", MODULE_NAME, dt.timestamp().c_str());
        }

        if (m_ntpTimeQ)
        {
            // Check ntp clock source, shall be dt in UTC format
            const BaseType_t rc = xQueueReceive(m_ntpTimeQ, &dt, 0);
            if (rc == pdPASS)
            {
                log::dbg("%s UTC:%s", MODULE_NAME, dt.timestamp().c_str());
                time.setUtcTime(dt);
            }
        }

        if (m_extTimeQ)
        {
            // Check if any external source wants to set time
            const BaseType_t rc = xQueueReceive(m_extTimeQ, &dt, 0);
            if (rc == pdPASS)
            {
                log::dbg("%s EXT UTC time src: %s", MODULE_NAME, dt.timestamp().c_str());
                time.setUtcTime(dt);
            }
        }

        vTaskDelay(timeMeasDelay);
    }
}

//------------------------------------------------------------------------------
bool ClockTask::pushTimeMsg(const DateTime& dt)
{
    BaseType_t status = pdFAIL;
    if (m_timeQ)
    {
        status = xQueueSendToBack(m_timeQ, &dt, 0);
    }
    return (status == pdPASS) ? true : false;
}


//------------------------------------------------------------------------------
