#include "display_task.h"
#include "../hw_config.h"

//------------------------------------------------------------------------------
DisplayTask::DisplayTask()
    : Task("DisplayTask", 4096, 10)
    , m_timeDispMode(Display::TMH)
    , m_timeQ(nullptr)
{
    m_timeQ = xQueueCreate(256, sizeof(DateTime));
    if (!m_timeQ)
    {
        kill();
    }
}

//------------------------------------------------------------------------------
bool DisplayTask::addTimeMsg(const DateTime& dt)
{
    const BaseType_t status = xQueueOverwrite(m_timeQ, &dt);
    return (status == pdPASS) ? true : false;
}

//------------------------------------------------------------------------------
void DisplayTask::run()
{
    bool timeFlasher = true;
    uint32_t timeLast = 0;
    uint32_t timeSecCnt = 0;
    DateTime dt;
    SystemRtc rtc;
    Display::MAX72xxConfig cfg =
    {
        DISPLAY_MAX72XX_HW_TYPE,
        DISPLAY_MAX72XX_MODULES_NUM,
        DISPLAY_DIN_PIN,
        DISPLAY_CLK_PIN,
        DISPLAY_CS_PIN,
    };
    Display disp(cfg);
    for(;;)
    {
        //const BaseType_t status = xQueuePeek(m_timeQ, &dt, 0);
        disp.update();
        //if (status == pdPASS) 
        {
            dt = rtc.getTime(); // LU_TODO
            if (millis() - timeLast >= 10000) // 10[s]
            {
                disp.printTime(dt, Display::DWYMD);
                timeLast = millis();
                if (m_timeDispMode == Display::TMH)
                {
                    m_timeDispMode = Display::TMHS;
                }
                else
                {
                    m_timeDispMode = Display::TMH;
                }
            }
            else
            {
                disp.printTime(dt, m_timeDispMode, timeFlasher);
            }

            if (millis() - timeSecCnt >= 1000) // 1[s]
            {
                timeFlasher = timeFlasher ^ 1;
                timeSecCnt = millis();
            }
        }
    }
}


//------------------------------------------------------------------------------
