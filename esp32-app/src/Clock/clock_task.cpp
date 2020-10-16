#include "clock_task.h"
#include "hw_config.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
ClockTask::ClockTask(DisplayTask& disp)
    : Task("ClockTask", 8192, 1)
    , m_rtc()
    , m_disp(disp)
{

}

//------------------------------------------------------------------------------
void ClockTask::run()
{
    const TickType_t timeMeasDelay = (100 / portTICK_PERIOD_MS);
    DateTime dt;
    for(;;)
    {
        {
            rtos::LockGuard<rtos::Mutex> lock(i2cMutex);
            dt = m_rtc.getTime();
        }
        //m_disp.addTimeMsg(dt);
        dbg("ClockTask::run()");
        vTaskDelay(timeMeasDelay);
    }
}


//------------------------------------------------------------------------------
