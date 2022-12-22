#pragma once

#include "open_weather_map.h"
#include "Config/WeatherConfigParam.h"
#include "Rtos/task.h"

class WeatherTask : public Task
{
public:
    explicit WeatherTask(const WeatherConfigData& weatherCfg);

    const QueueHandle_t& getWeatherQ();

private:
    virtual void run() override;

private:
    const WeatherConfigData& m_weatherCfg;
    QueueHandle_t m_weatherQ;
};