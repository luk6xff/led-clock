#include "clock_task.h"
#include "hw_config.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define CLOCK_TASK_STACK_SIZE (8192)
#define CLOCK_TASK_PRIORITY      (9)

//------------------------------------------------------------------------------
ClockTask::ClockTask(DisplayTask& disp)
    : Task("ClockTask", CLOCK_TASK_STACK_SIZE, CLOCK_TASK_PRIORITY)
    , m_disp(disp)
{

}

//------------------------------------------------------------------------------
void ClockTask::run()
{
    const TickType_t timeMeasDelay = (100 / portTICK_PERIOD_MS);
    SystemRtc rtc;
    DateTime dt;
    for(;;)
    {
        {
            rtos::LockGuard<rtos::Mutex> lock(g_i2cMutex);
            dt = rtc.getTime();
            if (dt.isValid())
            {
                m_disp.addTimeMsg(dt);
                dbg("DT-s: %s", dt.timestamp().c_str());
            }
            else
            {
                err("Invalid DateTime read from RTC: %s", dt.timestamp().c_str());
            }
        }
        vTaskDelay(timeMeasDelay);
    }
}


//------------------------------------------------------------------------------
