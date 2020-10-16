#include "clock_task.h"
#include "../hw_config.h"

//------------------------------------------------------------------------------
ClockTask::ClockTask(DisplayTask& disp)
    : Task("ClockTask", 4096, 5)
    , m_rtc()
    , m_disp(disp)
{

}

//------------------------------------------------------------------------------
void ClockTask::run()
{
    const TickType_t timeMeasDelay = (100 / portTICK_PERIOD_MS);
    for(;;)
    {
        const DateTime dt = m_rtc.getTime();
        m_disp.addTimeMsg(dt);
        vTaskDelay(timeMeasDelay);
    }
}


//------------------------------------------------------------------------------
