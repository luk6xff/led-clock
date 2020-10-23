#pragma once

#include "open_weather_map.h"
#include "weather_settings.h"
#include "App/task.h"

class WeatherTask : public Task
{
public:
    explicit WeatherTask(const WeatherSettings& weatherCfg);

    const QueueHandle_t& getWeatherQ();

private:
    virtual void run() override;

private:
    const WeatherSettings& m_weatherCfg;
    QueueHandle_t m_weatherQ;
};