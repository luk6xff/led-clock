#include "display_task.h"
#include "hw_config.h"
#include "App/rtos_utils.h"
#include "App/utils.h"
#include "App/app_context.h"

//------------------------------------------------------------------------------
#define DISPLAY_TASK_STACK_SIZE (8192)
#define DISPLAY_TASK_PRIORITY     (6)

#define MODULE_NAME "[DISP]"

//------------------------------------------------------------------------------
DisplayTask::DisplayTask(const DisplaySettings& displayCfg,
                            const QueueHandle_t& timeQ)
    : Task("DisplayTask", DISPLAY_TASK_STACK_SIZE, DISPLAY_TASK_PRIORITY, 0)
    , m_dispCfg {   DISPLAY_MAX72XX_HW_TYPE,
                    DISPLAY_MAX72XX_MODULES_NUM,
                    DISPLAY_DIN_PIN,
                    DISPLAY_CLK_PIN,
                    DISPLAY_CS_PIN }
    , m_timeDispMode(Display::THMS)
    , m_displayCfg(displayCfg)
    , m_timeQ(timeQ)
    , m_disp(m_dispCfg)
{

}

//------------------------------------------------------------------------------
void DisplayTask::run()
{
    const TickType_t k_dispRefreshTime = (50 / portTICK_PERIOD_MS);
    bool timeDots;
    bool timeMsgLock = false;
    const uint8_t k_timeMsgUnlockTime = 2; // [secs]
    uint8_t timeMsgUnlockTimer = 0; // [secs]
    DateTime dt;
    AppDisplayMsg dispMsg;
    m_disp.enableAutoIntensityLevelControl(m_displayCfg.enableAutoIntenisty);
    if (!m_disp.isAutoIntensityEnabled())
    {
        m_disp.setIntensityLevel(m_displayCfg.intensityValue);
    }
    else
    {
        // Create display auto intensity timer if needed
        m_lightSensorTimerTask.dispHandle = &m_disp;
        m_lightSensorTimerTask.timeoutMs = 2000;
        BaseType_t ret = xTaskCreate(runLightSensorCb,
                                    "DispSensTask",
                                    4096,
                                    &m_lightSensorTimerTask,
                                    2,
                                    &m_lightSensorTimerTask.timerTask);
        if (ret != pdPASS)
        {
            utils::err("%s DT:%s", MODULE_NAME, "m_lightSensorTimerTask start failed");
        }
    }

    m_disp.setDisplaySpeedValue(m_displayCfg.animSpeed);

    for(;;)
    {
        m_disp.update();
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

                if (!m_disp.printTime(dt, (Display::DateTimePrintMode)m_displayCfg.timeFormat, timeDots))
                {
                    timeMsgLock = true;
                }
                else
                {
                    if (timeMsgUnlockTimer++ > k_timeMsgUnlockTime)
                    {
                        timeMsgUnlockTimer = 0;
                        timeMsgLock = false;
                    }
                }
                utils::dbg("%s DT:%s", MODULE_NAME, dt.timestamp().c_str());
            }
        }

        if (!timeMsgLock && AppCtx.getDisplayQHandle())
        {
            const BaseType_t rc = xQueuePeek(AppCtx.getDisplayQHandle(), &dispMsg, 0);
            if (rc == pdPASS)
            {
                if (m_disp.printMsg(dispMsg.msg, dispMsg.msgLen))
                {
                    utils::dbg("%s MSG removed from DISP queue:%s", MODULE_NAME, dispMsg.msg);
                    // Remove the message from the queue if handled properly
                    xQueueReceive(AppCtx.getDisplayQHandle(), &dispMsg, 0);
                    free(dispMsg.msg);
                }
            }
        }
        vTaskDelay(k_dispRefreshTime);
    }
}


//------------------------------------------------------------------------------
void DisplayTask::runLightSensorCb(void *arg)
{
    DisplayTimerTask *d = static_cast<DisplayTimerTask*>(arg);
    const TickType_t timeoutMs = (d->timeoutMs / portTICK_PERIOD_MS);
    for(;;)
    {
        d->dispHandle->processAutoIntensityLevelControl();
        vTaskDelay(timeoutMs);
    }
}