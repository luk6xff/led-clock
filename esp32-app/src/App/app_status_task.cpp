#include "App/app_status_task.h"
#include "App/rtos_utils.h"
#include "App/utils.h"
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
            AppCtx.clearDisplayMsgQueue();
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            utils::err("APP_STATUS: RADIO_ERROR-%s",tr(M_RADIO_ERROR));
        }
    },
    {
        RTC_ERROR, []()
        {
            const String msg(tr(M_ERROR) + col + spc + tr(M_RTC_ERROR));
            AppCtx.clearDisplayMsgQueue();
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            utils::err("APP_STATUS: RTC_ERROR-%s",tr(M_RTC_ERROR));
        }
    },
    {
        DISP_ERROR, []()
        {
            const String msg(tr(M_ERROR) + col + spc + tr(M_DISP_ERROR));
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            utils::err("APP_STATUS: DISP_ERROR-%s",tr(M_DISP_ERROR));
        }
    },
    {
        EXT_DATA_SENSOR_ERROR, []()
        {
            const String msg(tr(M_ERROR) + col + spc + tr(M_EXT_DATA_SENSOR_ERROR));
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 0);
            utils::err("APP_STATUS: EXT_DATA_SENSOR_ERROR-%s",tr(M_EXT_DATA_SENSOR_ERROR));
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
            utils::inf("APP_STATUS: OTA_UPDATE_RUNNING-%s",tr(M_OTA_UPDATE_RUNNING));
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
    TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    AppStatusValue lastAppStatus = AppCtx.appStatus();

    for(;;)
    {
        const AppStatusValue currentAppStatus = AppCtx.appStatus();
        if (lastAppStatus != currentAppStatus)
        {
            utils::inf("%s AppStatus changed from:[0x%x] to:[0x%x]", MODULE_NAME, lastAppStatus, currentAppStatus);
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
