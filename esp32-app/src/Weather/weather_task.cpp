#include "weather_task.h"
#include "App/rtos_common.h"
#include "App/wifi_task.h"
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
        err("%s m_weatherQ has not been created!.", MODULE_NAME);
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
    const TickType_t sleepTime = (30000 / portTICK_PERIOD_MS);
    OpenWeatherMapOneCallData openWeatherMapOneCallData;
    OpenWeatherMapOneCall oneCallClient;
    oneCallClient.setMetric(true);
    oneCallClient.setLanguage(m_weatherCfg.language);
    for(;;)
    {
        if (!WiFi.isConnected())
        {
            vTaskDelay(sleepTime);
            continue;
        }

        oneCallClient.update(&openWeatherMapOneCallData, m_weatherCfg.owmAppid, m_weatherCfg.city.latitude, m_weatherCfg.city.longitude);

        dbg("Current Weather: ");
        dbg("%s %s", String(openWeatherMapOneCallData.current.temp, 1).c_str(), (true ? "°C" : "°F") );
        dbg("%s", openWeatherMapOneCallData.current.weatherDescription.c_str());
        dbg("Forecasts: ");

        for (int i = 0; i < 2; i++)
        {
            if (openWeatherMapOneCallData.daily[i].dt > 0)
            {
                dbg("dt: %s", String(openWeatherMapOneCallData.daily[i].dt).c_str());
                dbg("temp: %s %s",String(openWeatherMapOneCallData.daily[i].tempDay, 1).c_str(), (true ? "°C" : "°F") );
                dbg("desc: %s",  openWeatherMapOneCallData.daily[i].weatherDescription.c_str());
            }
        }
        vTaskDelay(sleepTime);
    }
}

//------------------------------------------------------------------------------
