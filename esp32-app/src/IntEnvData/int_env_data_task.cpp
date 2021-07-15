#include "int_env_data_task.h"
#include "App/utils.h"
#include "App/app_context.h"

//------------------------------------------------------------------------------
#define INT_ENV_DATA_TASK_STACK_SIZE (8192)
#define INT_ENV_DATA_TASK_PRIORITY      (2)

#define MODULE_NAME "[IENV]"

//------------------------------------------------------------------------------
IntEnvDataTask::IntEnvDataTask(InternalEnvDataSettings& intEnvSettings)
    : Task("IntEnvDataTask", INT_ENV_DATA_TASK_STACK_SIZE, INT_ENV_DATA_TASK_PRIORITY, 1)
    , m_intEnvCfg(intEnvSettings)
{
    m_intEnvDataComm.intEnvQ = xQueueCreate(1, sizeof(InternalEnvData));
    if (!m_intEnvDataComm.intEnvQ)
    {
        utils::err("%s m_intEnvQ has not been created!.", MODULE_NAME);
    }

    // Create Event Group
    m_intEnvDataComm.intEvtH = xEventGroupCreate();
    if (!m_intEnvDataComm.intEvtH)
    {
        utils::err("%s event group create failed!", MODULE_NAME);
    }
}

//------------------------------------------------------------------------------
const IntEnvDataComm& IntEnvDataTask::getComm()
{
    return m_intEnvDataComm;
}

//------------------------------------------------------------------------------
void IntEnvDataTask::run()
{
    const TickType_t k_waitForDataSecs = ((10 * 1000) / portTICK_PERIOD_MS);
    InternalEnvData msg;

    for(;;)
    {
        // Disable if OTA Update
        if (AppCtx.appStatus() & AppStatusType::OTA_UPDATE_RUNNING)
        {
            vTaskDelay(k_waitForDataSecs);
            continue;
        }

        if (m_intEnvCfg.update_data_interval == 0) // If internal env data sensor disabled
        {
            utils::inf("%s IntEnvData task not active! update_data_interval=0", MODULE_NAME);
            // Sleep longer
            vTaskDelay(k_waitForDataSecs);
        }
        else
        {
            // Set event group
            xEventGroupSetBits(m_intEnvDataComm.intEvtH, IntEnvDataEvent::INTERNAL_ENV_DATA_REQ);
            const BaseType_t rc = xQueueReceive(m_intEnvDataComm.intEnvQ, &msg, k_waitForDataSecs);
            if (rc == pdTRUE)
            {
                utils::err("%s IntEnvData received: temperature: %3.2f", MODULE_NAME, msg.temperature);
                pushIntEnvDataMsg(msg);
            }

            // Sleep for some time
            const uint32_t k_sleepTimeSecs = m_intEnvCfg.update_data_interval;
            const TickType_t k_sleepTime = ((k_sleepTimeSecs * 1000) / portTICK_PERIOD_MS);
            vTaskDelay(k_sleepTime);
        }
    }
}

//------------------------------------------------------------------------------
bool IntEnvDataTask::pushIntEnvDataMsg(const InternalEnvData& data)
{
    String col(":");
    String spc(" ");
    String com(",");
    String msg(tr(M_INT_ENV_SENSOR_NAME) + col + spc + \
                tr(M_SENSOR_TEMP) + col + String(data.temperature, 1) + tr(M_COMMON_DEG_CELS));

    return AppCtx.putDisplayMsg(msg.c_str(), msg.length(), m_intEnvCfg.msg_disp_repeat_num);
}

//------------------------------------------------------------------------------
