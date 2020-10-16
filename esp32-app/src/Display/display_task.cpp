#include "display_task.h"
#include "hw_config.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
DisplayTask::DisplayTask()
    : Task("DisplayTask", (8192*2), 10)
    , m_timeDispMode(Display::TMH)
    , m_timeQ(nullptr)
{
    m_timeQ = xQueueCreate(32, sizeof(DateTime));
    if (!m_timeQ)
    {
        err("m_timeQ has not been created! DisplayTask will be killed...");
        kill();
    }
}

//------------------------------------------------------------------------------
bool DisplayTask::addTimeMsg(const DateTime& dt)
{
    dbg("BeforeAdd");
    const BaseType_t status = /*xQueueOverwrite*/xQueueSendToBack(m_timeQ, &dt, 0);
    dbg("AfterAdd");
    return (status == pdPASS) ? true : false;
}

//------------------------------------------------------------------------------
void DisplayTask::run()
{
    bool timeDots;
    BaseType_t status;
    DateTime dt;
    Display::MAX72xxConfig cfg =
    {
        DISPLAY_MAX72XX_HW_TYPE,
        DISPLAY_MAX72XX_MODULES_NUM,
        DISPLAY_DIN_PIN,
        DISPLAY_CLK_PIN,
        DISPLAY_CS_PIN,
    };
    Display disp(cfg);

    const TickType_t timeMeasDelay = (50 / portTICK_PERIOD_MS);
    for(;;)
    {
        //addTimeMsg(dt);
        dbg("BeforePeek");
        status = xQueueReceive(m_timeQ, &dt, portMAX_DELAY);

        dbg("AfterPeek");
        {
            rtos::LockGuard<rtos::Mutex> lock(i2cMutex);
            disp.update();
        }
        if (status == pdPASS) 
        {
            if (dt.second() % 2)
            {
                timeDots = true; 
            }
            else
            {
                timeDots = false;
            }
            disp.printTime(dt, m_timeDispMode, timeDots);
        }
        vTaskDelay(timeMeasDelay);
    }
}


//------------------------------------------------------------------------------
