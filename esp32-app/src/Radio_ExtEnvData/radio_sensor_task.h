#pragma once

#include "App/task.h"
#include "App/rtos_utils.h"
#include "radio.h"

struct RadioSensorData
{
	uint32_t vbatt;
	float temperature;
	float pressure;
	float humidity;
};


struct RadioHealthStateTask
{
    TaskHandle_t task;
    rtos::Mutex mtx;
    Radio *radioHandle;
    uint32_t resetTimeoutSecs;
    uint32_t lastRadioMsgReceivedTimeMs;
};


class RadioSensorTask : public Task
{
public:
    explicit RadioSensorTask(RadioSensorSettings& radioSensorCfg);
    const QueueHandle_t& getRadioSensorQ();

    static void healtStateCheckCb(void *arg);

private:
    virtual void run() override;
    bool pushRadioSensorMsg(const RadioSensorData& dt);

private:
    RadioSensorSettings& m_radioSensorCfg;
    Radio m_radioSensor;
    QueueHandle_t m_radioSensorQ;
    RadioHealthStateTask m_radioHealthStateTask;
    RadioSensorData m_lastRadioMsg;
};

