#include "App/app_status_task.h"
#include "App/rtos_utils.h"
#include "Rtos/log.h"
#include "App/app_context.h"

//------------------------------------------------------------------------------
#define APP_STATUS_TASK_STACK_SIZE (8192)
#define APP_STATUS_TASK_PRIORITY      (5)

#define MODULE_NAME "[APPSTAT]"

namespace
{
    const String col(":");
    const String spc(" ");
}

//------------------------------------------------------------------------------
const AppStateActionsMap AppStatusTask::k_stateActions =
{
    {
        NO_ERROR, [](){} // Do nothing
    },
    {
        RADIO_ERROR, []()
        {
            const String msg(tr(M_ERROR) + col + spc + tr(M_RADIO_ERROR));
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            log::err("APP_STATUS: RADIO_ERROR-%s",tr(M_RADIO_ERROR));
        }
    },
    {
        RTC_ERROR, []()
        {
            const String msg(tr(M_ERROR) + col + spc + tr(M_RTC_ERROR));
            static int error_cnt = 0;
            if ((error_cnt++ % 10) == 0)
            {
                AppCtx.clearDisplayMsgQueue();
                AppCtx.putDisplayCmd(APP_DISPLAY_CLEAR_CMD);
                AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            }
            log::err("APP_STATUS: RTC_ERROR-%s",tr(M_RTC_ERROR));
        }
    },
    {
        DISP_ERROR, []()
        {
            const String msg(tr(M_ERROR) + col + spc + tr(M_DISP_ERROR));
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            log::err("APP_STATUS: DISP_ERROR-%s",tr(M_DISP_ERROR));
        }
    },
    {
        EXT_DATA_SENSOR_ERROR, []()
        {
            const String msg(tr(M_ERROR) + col + spc + tr(M_EXT_DATA_SENSOR_ERROR));
            static int error_cnt = 0;
            // Print msg every 10 min
            if ((error_cnt++ % 600) == 0)
            {
                AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            }
            log::err("APP_STATUS: EXT_DATA_SENSOR_ERROR-%s",tr(M_EXT_DATA_SENSOR_ERROR));
        }
    },
    {
        OTA_UPDATE_RUNNING, []()
        {
            const String msg(tr(M_OTA_UPDATE_RUNNING));
            static bool once = false;
            if (!once)
            {
                AppCtx.clearDisplayMsgQueue();
                // Clear display on OTA update once
                AppCtx.putDisplayCmd(APP_DISPLAY_CLEAR_CMD);
                // Send OTA msg
                once = true;
            }
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            log::inf("APP_STATUS: OTA_UPDATE_RUNNING-%s",tr(M_OTA_UPDATE_RUNNING));
        }
    },

};

//------------------------------------------------------------------------------
AppStatusTask::AppStatusTask()
    : Task("AppStatusTask", APP_STATUS_TASK_STACK_SIZE, APP_STATUS_TASK_PRIORITY)
{

}

//------------------------------------------------------------------------------
void AppStatusTask::run()
{
    const TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    AppStatusValue lastAppStatus = AppCtx.appStatus();

    for(;;)
    {
        const AppStatusValue currentAppStatus = AppCtx.appStatus();
        if (lastAppStatus != currentAppStatus)
        {
            log::inf("%s AppStatus changed from:[0x%x] to:[0x%x]", MODULE_NAME, lastAppStatus, currentAppStatus);
            lastAppStatus = currentAppStatus;
        }

        for (auto statusAction : k_stateActions)
        {
            if (currentAppStatus & statusAction.first)
            {
                statusAction.second();
            }
        }
        vTaskDelay(sleepTime);
    }
}

//------------------------------------------------------------------------------
