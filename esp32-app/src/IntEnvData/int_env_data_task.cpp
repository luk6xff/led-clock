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

}

//------------------------------------------------------------------------------
void IntEnvDataTask::run()
{
    const TickType_t k_waitForDataSecs = ((10 * 1000) / portTICK_PERIOD_MS);
    m_intEnvDataSensor.init();

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
            InternalEnvData msg;
            if (m_intEnvDataSensor.getData(msg))
            {
                utils::inf("%s IntEnvData received: temperature:%3.2f, humidity:%3.2f, pressure:%3.2f", MODULE_NAME, msg.temperature, msg.humidity, msg.pressure);
                pushIntEnvDataMsg(msg);
            }

            // Sleep for some time
            const uint32_t k_sleepTimeSecs = m_intEnvCfg.update_data_interval;
            const TickType_t k_sleepTime = ((k_sleepTimeSecs * 1000) / portTICK_PERIOD_MS);
            vTaskDelay(k_sleepTime);
        }
    }

    m_intEnvDataSensor.deinit();
}

//------------------------------------------------------------------------------
bool IntEnvDataTask::pushIntEnvDataMsg(const InternalEnvData& data)
{

    String col(":");
    String spc(" ");
    String com(",");
    String msg(tr(M_INT_ENV_SENSOR_NAME) + col + spc + \
                tr(M_SENSOR_TEMP) + col + String(data.temperature, 1) + tr(M_COMMON_DEG_CELS) + com + spc + \
                tr(M_SENSOR_PRESS) + col + String(uint32_t(data.pressure/100)) + tr(M_COMMON_PRESSURE_HPA) + com + spc + \
                tr(M_SENSOR_HUMID) + col + String(uint8_t(data.humidity)) + tr(M_COMMON_PERCENT));

    return AppCtx.putDisplayMsg(msg.c_str(), msg.length(), m_intEnvCfg.msg_disp_repeat_num);
}

//------------------------------------------------------------------------------
