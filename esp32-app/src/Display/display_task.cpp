#include "display_task.h"
#include "hw_config.h"
#include "App/rtos_common.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define DISPLAY_TASK_STACK_SIZE (8192)
#define DISPLAY_TASK_PRIORITY     (6)

#define MODULE_NAME "[DISP]"

//------------------------------------------------------------------------------
DisplayTask::DisplayTask(const QueueHandle_t& timeQ)
    : Task("DisplayTask", DISPLAY_TASK_STACK_SIZE, DISPLAY_TASK_PRIORITY, 0)
    , m_dispCfg {   DISPLAY_MAX72XX_HW_TYPE,
                    DISPLAY_MAX72XX_MODULES_NUM,
                    DISPLAY_DIN_PIN,
                    DISPLAY_CLK_PIN,
                    DISPLAY_CS_PIN }
    , m_timeDispMode(Display::THMS)
    , m_timeQ(timeQ)
{

}

//------------------------------------------------------------------------------
void DisplayTask::run()
{
    const TickType_t dispRefreshTime = (50 / portTICK_PERIOD_MS);
    bool timeDots;
    DateTime dt;
    Display m_disp(m_dispCfg);
    for(;;)
    {
        m_disp.update();
        m_disp.processAutoIntensityLevelControl();
        if (m_timeQ)
        {
            const BaseType_t rc = xQueueReceive(m_timeQ, &dt, 0);
            if (rc == pdPASS)
            {
                if (dt.second() % 2)
                {
                    timeDots = false;
                }
                else
                {
                    timeDots = true;
                }
                m_disp.printTime(dt, m_timeDispMode, timeDots);
                utils::dbg("%s DT:%s", MODULE_NAME, dt.timestamp().c_str());
            }
        }
        vTaskDelay(dispRefreshTime);
    }
}


//------------------------------------------------------------------------------
