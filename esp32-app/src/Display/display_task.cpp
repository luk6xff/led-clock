#include "display_task.h"
#include "hw_config.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define DISPLAY_TASK_STACK_SIZE (8192)
#define DISPLAY_TASK_PRIORITY     (10)

//------------------------------------------------------------------------------
DisplayTask::DisplayTask()
    : Task("DisplayTask", DISPLAY_TASK_STACK_SIZE, DISPLAY_TASK_PRIORITY, 0)
    , m_dispCfg {   DISPLAY_MAX72XX_HW_TYPE,
                    DISPLAY_MAX72XX_MODULES_NUM,
                    DISPLAY_DIN_PIN,
                    DISPLAY_CLK_PIN,
                    DISPLAY_CS_PIN }
    , m_timeDispMode(Display::THM)
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
    BaseType_t status = pdFAIL;
    if (m_timeQ)
    {
        status = xQueueSendToBack(m_timeQ, &dt, 0);
    }
    return (status == pdPASS) ? true : false;
}

//------------------------------------------------------------------------------
void DisplayTask::run()
{
    bool timeDots;
    BaseType_t status;
    DateTime dt;
    const TickType_t dispRefreshTime = (50 / portTICK_PERIOD_MS);
    Display m_disp(m_dispCfg);
    m_disp.setup();
    for(;;)
    {
        status = xQueueReceive(m_timeQ, &dt, portMAX_DELAY);
        {
            rtos::LockGuard<rtos::Mutex> lock(g_i2cMutex);
            m_disp.update();
        }
        dbg("DT-r: %s", dt.timestamp().c_str());
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
            m_disp.printTime(dt, m_timeDispMode, timeDots);
        }
        vTaskDelay(dispRefreshTime);
    }
}


//------------------------------------------------------------------------------
