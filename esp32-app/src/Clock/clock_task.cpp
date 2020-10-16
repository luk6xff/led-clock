#include "clock_task.h"
#include "hw_config.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define CLOCK_TASK_STACK_SIZE (8192*2)
#define CLOCK_TASK_PRIORITY      (9)

//------------------------------------------------------------------------------
ClockTask::ClockTask(DisplayTask& disp)
    : Task("ClockTask", CLOCK_TASK_STACK_SIZE, CLOCK_TASK_PRIORITY)
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
            rtos::LockGuard<rtos::Mutex> lock(g_i2cMutex);
            dt = m_rtc.getTime();
        }
        //m_disp.addTimeMsg(dt);
        dbg("ClockTask::run()");
        vTaskDelay(timeMeasDelay);
    }
}


//------------------------------------------------------------------------------
