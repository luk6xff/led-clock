#include "App/app_status_task.h"
#include "App/rtos_utils.h"
#include "App/utils.h"
#include "App/app_context.h"

//------------------------------------------------------------------------------
#define APP_STATUS_TASK_STACK_SIZE (1024)
#define APP_STATUS_TASK_PRIORITY      (2)

#define MODULE_NAME "[APPSTAT]"

//------------------------------------------------------------------------------
AppStatusTask::AppStatusTask()
    : Task("AppStatusTask", APP_STATUS_TASK_STACK_SIZE, APP_STATUS_TASK_PRIORITY)
{

}

//------------------------------------------------------------------------------
void AppStatusTask::run()
{
    TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    for(;;)
    {
        // TODO Check and update status!
        vTaskDelay(sleepTime);
    }
}

//------------------------------------------------------------------------------
