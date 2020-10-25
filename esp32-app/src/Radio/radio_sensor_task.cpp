#include "radio_sensor_task.h"
#include "App/utils.h"

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
        err("%s m_radioSensorQ has not been created!.", MODULE_NAME);
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
    Radio radioSensor(m_radioSensorCfg);
    for(;;)
    {
        radio_msg_sensor_frame msg;
        const BaseType_t rc = xQueueReceive(Radio::msgSensorDataQ, &msg, portMAX_DELAY);
        if (rc == pdTRUE)
        {
            dbg(">>>RadioSensorData received:");
            Radio::parse_incoming_msg_sensor((uint8_t*)&msg, sizeof(msg));
            //radioSensor.send(&msg);
        }
    }
}

//------------------------------------------------------------------------------
bool RadioSensorTask::pushRadioSensorMsg(const RadioSensorData& dt)
{
    BaseType_t status = pdFAIL;
    if (m_radioSensorQ)
    {
        status = xQueueSendToBack(m_radioSensorQ, &dt, 0);
    }
    return (status == pdPASS) ? true : false;
}

//------------------------------------------------------------------------------
