#include "radio_sensor_task.h"
#include "App/utils.h"
#include "App/app_shared.h"

//------------------------------------------------------------------------------
#define RADIO_SENSOR_TASK_STACK_SIZE (8192*2)
#define RADIO_SENSOR_TASK_PRIORITY        (3)

#define MODULE_NAME "[RSEN]"

//------------------------------------------------------------------------------
RadioSensorTask::RadioSensorTask(RadioSensorSettings& radioSensorCfg)
    : Task("RadioSensorTask", RADIO_SENSOR_TASK_STACK_SIZE, RADIO_SENSOR_TASK_PRIORITY, 0)
    , m_radioSensorCfg(radioSensorCfg)
    , m_radioSensorQ(nullptr)
{
    m_radioSensorQ = xQueueCreate(3, sizeof(RadioSensorData));
    if (!m_radioSensorQ)
    {
        utils::err("%s m_radioSensorQ has not been created!.", MODULE_NAME);
    }
}

//------------------------------------------------------------------------------
const QueueHandle_t& RadioSensorTask::getRadioSensorQ()
{
    return m_radioSensorQ;
}

//------------------------------------------------------------------------------
void RadioSensorTask::run()
{
    const TickType_t sleepTime = (100 / portTICK_PERIOD_MS);
    Radio radioSensor(m_radioSensorCfg);
    for(;;)
    {
        radio_msg_sensor_frame msg;
        const BaseType_t rc = xQueueReceive(Radio::msgSensorDataQ, &msg, portMAX_DELAY);
        if (rc == pdTRUE)
        {
            utils::dbg(">>>RadioSensorData received:");
            Radio::parse_incoming_msg_sensor((uint8_t*)&msg, sizeof(msg));
            // Send response to the sensor
            radioSensor.sendResponseToSensor();
            RadioSensorData data =
            {
                .vbatt = msg.vbatt,
                .temperature = msg.temperature,
                .pressure = msg.pressure,
                .humidity = msg.humidity,
            };
            pushRadioSensorMsg(data);
        }
        vTaskDelay(sleepTime);
    }
}

//------------------------------------------------------------------------------
bool RadioSensorTask::pushRadioSensorMsg(const RadioSensorData& data)
{
    //Send directly to display instead of radio queue
    // BaseType_t status = pdFAIL;
    // if (m_radioSensorQ)
    // {
    //     status = xQueueSendToBack(m_radioSensorQ, &data, 0);
    // }
    // return (status == pdPASS) ? true : false;
    String col(":");
    String spc(" ");
    String msg(tr(M_SENSOR_NAME) + col + spc + \
                tr(M_SENSOR_TEMP) + col + String(data.temperature) + tr(M_COMMON_DEG_CELS) + spc + \
                tr(M_SENSOR_PRESS) + col + String(uint32_t(data.pressure/100)) + tr(M_COMMON_PRESSURE_HPA) + spc + \
                tr(M_SENSOR_HUMID) + col + String(uint8_t(data.humidity)) + tr(M_COMMON_PERCENT) + spc + \
                tr(M_SENSOR_VBATT) + col + String(float(data.vbatt/1000.f)) + tr(M_COMMON_VOLTAGE));

    return AppSh.putDisplayMsg(msg.c_str(), msg.length());
}

//------------------------------------------------------------------------------
