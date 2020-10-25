#pragma once
#include "App/task.h"
#include "radio.h"

struct RadioSensorData
{
	uint32_t vbatt;
	float temperature;
	float pressure;
	float humidity;
};

class RadioSensorTask : public Task
{
public:
    explicit RadioSensorTask(RadioSensorSettings& radioSensorCfg);

    const QueueHandle_t& getRadioSensorQ();

private:
    virtual void run() override;
    bool pushRadioSensorMsg(const RadioSensorData& dt);

private:
    RadioSensorSettings& m_radioSensorCfg;
    QueueHandle_t m_radioSensorQ;
};