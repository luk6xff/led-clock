#include "clock_task.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define CLOCK_TASK_STACK_SIZE (8192*2)
#define CLOCK_TASK_PRIORITY      (9)

//------------------------------------------------------------------------------
ClockTask::ClockTask(DisplayTask& disp,
                     SystemTimeSettings& timeCfg,
                     NtpSettings& ntpCfg)
    : Task("ClockTask", CLOCK_TASK_STACK_SIZE, CLOCK_TASK_PRIORITY)
    , m_disp(disp)
    , m_timeCfg(timeCfg)
    , m_ntpCfg(m_ntpCfg)
{

}

//------------------------------------------------------------------------------
void ClockTask::run()
{
    const TickType_t timeMeasDelay = (100 / portTICK_PERIOD_MS);

    SystemTime time(m_timeCfg);
    // NtpSettings ntpCfg(0, 1000*3600, "time.google.com");//"pl.pool.ntp.org");
    Ntp ntp(m_ntpCfg);
    DateTime dt;
    for(;;)
    {
        {
            rtos::LockGuard<rtos::Mutex> lock(g_i2cMutex);
            dt = time.getTime();
        }

        if (dt.isValid())
        {
            //m_disp.addTimeMsg(dt);
            dbg("DT-s: %s", dt.timestamp().c_str());
        }
        else
        {
            err("Invalid DateTime read from RTC: %s", dt.timestamp().c_str());
        }
        // if (ntp.updateTime())
        // {
        //     DateTime dt(ntp.getCurrentTime());
        //     dbg("NTP UTC:%s", dt.timestamp().c_str());
        //     time.setUtcTime(dt);
        // }
        vTaskDelay(timeMeasDelay);
    }
}


//------------------------------------------------------------------------------
