#include "display_task.h"
#include "hw_config.h"
#include "Rtos/RtosUtils.h"
#include "Rtos/logger.h"
#include "App/app_context.h"
#include <cstring>

//------------------------------------------------------------------------------
#define DISPLAY_TASK_STACK_SIZE (8192)
#define DISPLAY_TASK_PRIORITY     (6)

#define MODULE_NAME "[DISP]"

//------------------------------------------------------------------------------
DisplayTask::DisplayTask(const DisplayConfigData& displayCfg,
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
    const uint8_t k_timeMsgUnlockTime = 3; // [secs]
    uint8_t timeMsgUnlockTimer = 0; // [secs] Print time not a msg as first for a few seconds
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
            logger::err("%s DT:%s", MODULE_NAME, "m_lightSensorTimerTask start failed");
        }
    }

    m_disp.setDisplaySpeedValue(m_displayCfg.animationSpeed);

    for(;;)
    {
        m_disp.update();
        // Time queue
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

                if (m_disp.printTime(dt, (Display::DateTimePrintMode)m_displayCfg.timeFormat, timeDots))
                {
                    timeMsgUnlockTimer++;
                }
                logger::dbg("%s DT:%s", MODULE_NAME, dt.timestamp().c_str());
            }
        }

        // AppDisplay cmd queue
        AppDisplayCmd cmd;
        const BaseType_t rc = xQueueReceive(AppCtx.getDisplayCmdQHandle(), &cmd, 0);
        if (rc == pdPASS)
        {
            switch (cmd)
            {
                logger::dbg("%s APP_DISPLAY_CMD received: 0x%x", MODULE_NAME, cmd);
                case APP_DISPLAY_CLEAR_CMD:
                {
                    m_disp.clear();
                    // Unlock msg queue immediately
                    timeMsgUnlockTimer = k_timeMsgUnlockTime + 1;
                    break;
                }

                case APP_DISPLAY_RESET_CMD:
                {
                    m_disp.reset();
                    // Unlock msg queue immediately
                    timeMsgUnlockTimer = k_timeMsgUnlockTime + 1;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        // AppDisplay msg queue
        if ((timeMsgUnlockTimer > k_timeMsgUnlockTime) && AppCtx.getDisplayMsgQHandle())
        {
            const BaseType_t rc = xQueuePeek(AppCtx.getDisplayMsgQHandle(), &dispMsg, 0);
            if (rc == pdPASS)
            {
                if (m_disp.printMsg(dispMsg.msg, dispMsg.msgLen))
                {
                    logger::dbg("%s MSG removed from DISP queue:%s", MODULE_NAME, dispMsg.msg);
                    // Clear the print time counter
                    timeMsgUnlockTimer = 0;

                    // Remove the message from the queue if handled properly
                    xQueueReceive(AppCtx.getDisplayMsgQHandle(), &dispMsg, 0);
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