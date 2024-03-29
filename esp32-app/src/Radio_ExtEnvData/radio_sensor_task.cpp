#include "radio_sensor_task.h"
#include "Rtos/logger.h"
#include "App/app_context.h"

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
        logger::err("%s m_radioSensorQ has not been created!.", MODULE_NAME);
    }

    // Create radio health state task
    m_radioHealthStateTask.radioHandle = &m_radioSensor;
    m_radioHealthStateTask.resetTimeoutSecs = m_radioSensorCfg.update_data_interval * 4; // When no radio msg received in 4x update interval, reset the radio
    m_radioHealthStateTask.lastRadioMsgReceivedTimeMs = millis();
    BaseType_t ret = xTaskCreate(healtStateCheckCb,
                                    "RHSTask",
                                    4096,
                                    &m_radioHealthStateTask,
                                    1,
                                    &m_radioHealthStateTask.task);
    if (ret != pdPASS)
    {
        logger::err("%s m_radioHealthStateTask start failed", MODULE_NAME);
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
    const uint32_t k_sendLastMsgNumber = 10;
    const uint32_t k_sleepTimeSecs = 10;
    const TickType_t k_sleepTime = ((k_sleepTimeSecs * 1000)/ portTICK_PERIOD_MS);

    uint32_t sendLastMsgCounterSecs = 0;
    uint32_t sendLastMsgNumCounter = 0;
    bool firstValidMsgReceived = false;
    radio_msg_queue_data msg;

    for(;;)
    {
        // Disable if OTA Update
        if (AppCtx.appStatus() & AppStatusType::OTA_UPDATE_RUNNING)
        {
            vTaskDelay(k_sleepTime);
            continue;
        }

        const BaseType_t rc = xQueueReceive(Radio::msgSensorDataQ, &msg, 0);
        if (rc == pdTRUE)
        {
            logger::dbg(">>>radio_msg_queue_data received:");
            logger::dbg("received from: 0x%x", msg.hdr.sender_id);
            logger::dbg("sent to: 0x%x", msg.hdr.receiver_id);
            logger::dbg("message ID: 0x%x", msg.hdr.msg_id);
            logger::dbg("payload length: %d", msg.hdr.payload_len);
            logger::dbg("RSSI: %d", msg.rssi);
            logger::dbg("SNR: %f", msg.snr);
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

                // Send a radio message to the display
                pushRadioSensorMsg(data);

                firstValidMsgReceived = true;
                // Store last message
                m_lastRadioMsg = data;

                // Clear send last message counters
                sendLastMsgNumCounter = 0;
                sendLastMsgCounterSecs = 0;

                // Update status
                AppCtx.clearAppStatus(AppStatusType::EXT_DATA_SENSOR_ERROR);
            }
            {
                rtos::LockGuard<rtos::Mutex> lock(m_radioHealthStateTask.mtx);
                m_radioHealthStateTask.lastRadioMsgReceivedTimeMs = millis();
            }
        }

        // Sleep for some time
        vTaskDelay(k_sleepTime);

        // Display last message
        if (m_radioSensorCfg.last_msg_disp_every_minutes != 0)
        {
            if (firstValidMsgReceived && (sendLastMsgNumCounter < k_sendLastMsgNumber))
            {
                sendLastMsgCounterSecs += k_sleepTimeSecs;
                if ((sendLastMsgCounterSecs / 60) >= m_radioSensorCfg.last_msg_disp_every_minutes)
                {
                    pushRadioSensorMsg(m_lastRadioMsg);
                    sendLastMsgNumCounter++;
                    sendLastMsgCounterSecs = 0;
                }
            }
        }
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
    String msg(tr(M_EXT_ENV_SENSOR_NAME) + col + spc + \
                tr(M_SENSOR_TEMP) + col + String(data.temperature, 1) + tr(M_COMMON_DEG_CELS) + com + spc + \
                tr(M_SENSOR_PRESS) + col + String(uint32_t(data.pressure/100)) + tr(M_COMMON_PRESSURE_HPA) + com + spc + \
                tr(M_SENSOR_HUMID) + col + String(uint8_t(data.humidity)) + tr(M_COMMON_PERCENT));// + com + spc +
                //tr(M_SENSOR_VBATT) + col + String(float(data.vbatt/1000.f)) + tr(M_COMMON_VOLTAGE));

    return AppCtx.putDisplayMsg(msg.c_str(), msg.length(), m_radioSensorCfg.msg_disp_repeat_num);
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
                logger::dbg("%s No Radio frame msg received for too long:[%d] seconds, restarting radio...", MODULE_NAME, rhst->resetTimeoutSecs);
                // Update status
                AppCtx.setAppStatus(AppStatusType::EXT_DATA_SENSOR_ERROR);
                rhst->radioHandle->restart();
                rhst->lastRadioMsgReceivedTimeMs = millis();
            }
        }
        vTaskDelay(timeoutMs);
    }
}

//------------------------------------------------------------------------------
