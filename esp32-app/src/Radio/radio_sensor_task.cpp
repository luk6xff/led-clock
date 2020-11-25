#include "radio_sensor_task.h"
#include "App/utils.h"
#include "App/app_shared.h"

//------------------------------------------------------------------------------
#define RADIO_SENSOR_TASK_STACK_SIZE (8192*2)
#define RADIO_SENSOR_TASK_PRIORITY        (3)

#define MODULE_NAME "[RSEN]"

//------------------------------------------------------------------------------
RadioSensorTask::RadioSensorTask(RadioSensorSettings& radioSensorCfg)
    : Task("RadioSensorTask", RADIO_SENSOR_TASK_STACK_SIZE, RADIO_SENSOR_TASK_PRIORITY, 1)
    , m_radioSensorCfg(radioSensorCfg)
    , m_radioSensor(m_radioSensorCfg)
    , m_radioSensorQ(nullptr)
{
    m_radioSensorQ = xQueueCreate(3, sizeof(RadioSensorData));
    if (!m_radioSensorQ)
    {
        utils::err("%s m_radioSensorQ has not been created!.", MODULE_NAME);
    }

    // Create radio health state task
    m_radioHealthStateTask.radioHandle = &m_radioSensor;
    m_radioHealthStateTask.resetTimeoutSecs = m_radioSensorCfg.update_data_interval * 4; // When no radio msg received in 4x update interval, reset the radio
    m_radioHealthStateTask.lastRadioMsgReceivedTimeMs = 0;
    BaseType_t ret = xTaskCreate(healtStateCheckCb,
                                    "RHSTask",
                                    4096,
                                    &m_radioHealthStateTask,
                                    1,
                                    &m_radioHealthStateTask.task);
    if (ret != pdPASS)
    {
        utils::err("%s m_radioHealthStateTask start failed", MODULE_NAME);
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
    const TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    radio_msg_queue_data msg;
    for(;;)
    {
        const BaseType_t rc = xQueueReceive(Radio::msgSensorDataQ, &msg, portMAX_DELAY);
        if (rc == pdTRUE)
        {
            utils::dbg(">>>radio_msg_queue_data received:");
            utils::dbg("received from: 0x%x", msg.hdr.sender_id);
            utils::dbg("sent to: 0x%x", msg.hdr.receiver_id);
            utils::dbg("message ID: 0x%x", msg.hdr.msg_id);
            utils::dbg("payload length: %d", msg.hdr.payload_len);
            utils::dbg("RSSI: %d", msg.rssi);
            utils::dbg("SNR: %f", msg.snr);
            radio_msg_sensor_frame_status ret = Radio::parse_incoming_msg_sensor((uint8_t*)&(msg.frame), sizeof(msg.frame));
            // Send response to the sensor
            m_radioSensor.sendResponseToSensor();
            if (ret == MSG_NO_ERROR || ret == MSG_BATT_LOW)
            {
                RadioSensorData data =
                {
                    .vbatt = msg.frame.vbatt,
                    .temperature = msg.frame.temperature,
                    .pressure = msg.frame.pressure,
                    .humidity = msg.frame.humidity,
                };
                pushRadioSensorMsg(data);
            }
            {
                rtos::LockGuard<rtos::Mutex> lock(m_radioHealthStateTask.mtx);
                m_radioHealthStateTask.lastRadioMsgReceivedTimeMs = millis();
            }

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
    String com(",");
    String msg(tr(M_SENSOR_NAME) + col + spc + \
                tr(M_SENSOR_TEMP) + col + String(data.temperature) + tr(M_COMMON_DEG_CELS) + com + spc + \
                tr(M_SENSOR_PRESS) + col + String(uint32_t(data.pressure/100)) + tr(M_COMMON_PRESSURE_HPA) + com + spc + \
                tr(M_SENSOR_HUMID) + col + String(uint8_t(data.humidity)) + tr(M_COMMON_PERCENT) + com + spc + \
                tr(M_SENSOR_VBATT) + col + String(float(data.vbatt/1000.f)) + tr(M_COMMON_VOLTAGE));

    return AppSh.putDisplayMsg(msg.c_str(), msg.length());
}

//------------------------------------------------------------------------------
void RadioSensorTask::healtStateCheckCb(void *arg)
{
    RadioHealthStateTask *rhst = static_cast<RadioHealthStateTask*>(arg);
    const TickType_t timeoutMs = (10000 / portTICK_PERIOD_MS);
    for(;;)
    {
        {
            rtos::LockGuard<rtos::Mutex> lock(rhst->mtx);
            if (((millis() - rhst->lastRadioMsgReceivedTimeMs) / 1000) > rhst->resetTimeoutSecs)
            {
                // No Frame received for too long, restart radio
                utils::dbg("%s No Radio frame msg received for too long:[%d] seconds, restarting radio...", MODULE_NAME, rhst->resetTimeoutSecs);
                rhst->radioHandle->restart();
                rhst->lastRadioMsgReceivedTimeMs = 0;
            }
        }
        vTaskDelay(timeoutMs);
    }
}

//------------------------------------------------------------------------------
