#include "clock_task.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define CLOCK_TASK_STACK_SIZE (8192)
#define CLOCK_TASK_PRIORITY     (6)

#define MODULE_NAME "[CLCK]"

//------------------------------------------------------------------------------
ClockTask::ClockTask(SystemTimeSettings& timeCfg, const QueueHandle_t& ntpTimeQ)
    : Task("ClockTask", CLOCK_TASK_STACK_SIZE, CLOCK_TASK_PRIORITY, 1)
    , m_timeCfg(timeCfg)
    , m_ntpTimeQ(ntpTimeQ)
    , m_timeQ(nullptr)
{
    m_timeQ = xQueueCreate(16, sizeof(DateTime));
    if (!m_timeQ)
    {
        err("%s m_timeQ has not been created!.", MODULE_NAME);
    }
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
        dt = time.getTime();
        if (dt.isValid())
        {
            if (lastDt != dt)
            {
                if (pushTimeMsg(dt))
                {
                    dbg("%s DT:%s", MODULE_NAME, dt.timestamp().c_str());
                }
            }
            lastDt = dt;
        }
        else
        {
            err("%s Invalid DateTime read from RTC: %s", MODULE_NAME, dt.timestamp().c_str());
        }

        if (m_ntpTimeQ)
        {
            // Check ntp clock source, shall be dt in UTC format
            const BaseType_t rc = xQueueReceive(m_ntpTimeQ, &dt, 0);
            if (rc == pdPASS)
            {
                dbg("%s UTC:%s", MODULE_NAME, dt.timestamp().c_str());
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
