#include "clock_task.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define CLOCK_TASK_STACK_SIZE (8192)
#define CLOCK_TASK_PRIORITY     (6)

//------------------------------------------------------------------------------
ClockTask::ClockTask(DisplayTask& disp,
                     SystemTimeSettings& timeCfg)
    : Task("ClockTask", CLOCK_TASK_STACK_SIZE, CLOCK_TASK_PRIORITY, 1)
    , m_disp(disp)
    , m_timeCfg(timeCfg)
    , m_extSrcTime(nullptr)
{
    m_extSrcTime = xQueueCreate(1, sizeof(DateTime));
    if (!m_extSrcTime)
    {
        err("m_extSrcTime has not been created!");
    }
}

//------------------------------------------------------------------------------
const QueueHandle_t& ClockTask::extSrcTimeQueue()
{
    return m_extSrcTime;
}

//------------------------------------------------------------------------------
void ClockTask::run()
{
    const TickType_t timeMeasDelay = (200 / portTICK_PERIOD_MS);

    SystemTime time(m_timeCfg);
    DateTime dt;
    for(;;)
    {
        dt = time.getTime();
        if (dt.isValid())
        {
            m_disp.addTimeMsg(dt);
            dbg("DT-s: %s", dt.timestamp().c_str());
        }
        else
        {
            err("Invalid DateTime read from RTC: %s", dt.timestamp().c_str());
        }

        // Check external clock source, shall be dt in UTC format
        const BaseType_t rc = xQueueReceive(m_extSrcTime, &dt, 0);
        if (rc == pdPASS)
        {
            dbg("[SYSTIME] UTC:%s", dt.timestamp().c_str());
            time.setUtcTime(dt);
        }

        vTaskDelay(timeMeasDelay);
    }
}


//------------------------------------------------------------------------------
