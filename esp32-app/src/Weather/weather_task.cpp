#include "weather_task.h"
#include "App/rtos_common.h"
#include "Wifi/wifi_task.h"
#include "App/utils.h"

//------------------------------------------------------------------------------
#define WEATHER_TASK_STACK_SIZE (8192*2)
#define WEATHER_TASK_PRIORITY      (2)

#define MODULE_NAME "[WEAT]"

//------------------------------------------------------------------------------
WeatherTask::WeatherTask(const WeatherSettings& weatherCfg)
    : Task("WeatherTask", WEATHER_TASK_STACK_SIZE, WEATHER_TASK_PRIORITY, CONFIG_ARDUINO_RUNNING_CORE)
    , m_weatherCfg(weatherCfg)
{
    m_weatherQ = xQueueCreate(1, sizeof(WeatherData));
    if (!m_weatherQ)
    {
        utils::err("%s m_weatherQ has not been created!.", MODULE_NAME);
    }
}

//------------------------------------------------------------------------------
const QueueHandle_t& WeatherTask::getWeatherQ()
{
    return m_weatherQ;
}

//------------------------------------------------------------------------------
void WeatherTask::run()
{
    const TickType_t sleepTime = (1000 / portTICK_PERIOD_MS);
    uint32_t updateIntervalSeconds = m_weatherCfg.updateInterval;
    OpenWeatherMapOneCallData openWeatherMapOneCallData;
    OpenWeatherMapOneCall oneCallClient;
    oneCallClient.setMetric(true);
    oneCallClient.setLanguage(m_weatherCfg.language);
    for(;;)
    {
        if (!m_weatherCfg.enable)
        {
            vTaskDelay((10000 / portTICK_PERIOD_MS));
            continue;
        }

        if (!WiFi.isConnected())
        {
            vTaskDelay(sleepTime);
            continue;
        }

        if (updateIntervalSeconds++ < m_weatherCfg.updateInterval)
        {
            vTaskDelay(sleepTime);
            continue;
        }

        // Update interval passed, run next measurement
        oneCallClient.update(&openWeatherMapOneCallData, m_weatherCfg.owmAppid, m_weatherCfg.city.latitude, m_weatherCfg.city.longitude);
        utils::dbg("Current Weather: ");
        utils::dbg("%s %s", String(openWeatherMapOneCallData.current.temp, 1).c_str(), (true ? "°C" : "°F") );
        utils::dbg("%s", openWeatherMapOneCallData.current.weatherDescription.c_str());
        utils::dbg("Forecasts: ");

        for (int i = 0; i < 2; i++)
        {
            if (openWeatherMapOneCallData.daily[i].dt > 0)
            {
                utils::dbg("dt: %s", String(openWeatherMapOneCallData.daily[i].dt).c_str());
                utils::dbg("temp: %s %s",String(openWeatherMapOneCallData.daily[i].tempDay, 1).c_str(), (true ? "°C" : "°F") );
                utils::dbg("desc: %s",  openWeatherMapOneCallData.daily[i].weatherDescription.c_str());
            }
        }
        // Reset time to next measurement
        updateIntervalSeconds = 0;
    }
}

//------------------------------------------------------------------------------
