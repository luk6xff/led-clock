#include "weather_task.h"
#include "App/rtos_utils.h"
#include "Wifi/wifi_task.h"
#include "App/utils.h"
#include "App/app_context.h"

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
        log::err("%s m_weatherQ has not been created!.", MODULE_NAME);
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
    const TickType_t k_sleepTime = (1000 / portTICK_PERIOD_MS);
    uint32_t updateIntervalSeconds = m_weatherCfg.updateInterval;
    OpenWeatherMapOneCallData openWeatherMapOneCallData;
    OpenWeatherMapOneCall oneCallClient;
    oneCallClient.setMetric(true);
    oneCallClient.setLanguage(String(tr(M_LANG)));
    for(;;)
    {

        // Disable if OTA Update
        if (AppCtx.appStatus() & AppStatusType::OTA_UPDATE_RUNNING)
        {
            vTaskDelay(k_sleepTime);
            continue;
        }

        if (!m_weatherCfg.enable)
        {
            vTaskDelay((10000 / portTICK_PERIOD_MS));
            continue;
        }

        if (!WiFi.isConnected())
        {
            vTaskDelay(k_sleepTime);
            continue;
        }

        if (updateIntervalSeconds++ < m_weatherCfg.updateInterval)
        {
            vTaskDelay(k_sleepTime);
            continue;
        }

        // Update interval passed, run next measurement
        if (oneCallClient.update(&openWeatherMapOneCallData, m_weatherCfg.owmAppid, m_weatherCfg.city.latitude, m_weatherCfg.city.longitude))
        {
            String msg;

            log::dbg("Current Weather: ");
            log::dbg("%s %s", String(openWeatherMapOneCallData.current.temp, 1).c_str(), (true ? "°C" : "°F") );
            log::dbg("%s", openWeatherMapOneCallData.current.weatherDescription.c_str());
            log::dbg("Forecasts: ");

            for (int i = 0; i < 2; i++)
            {
                if (openWeatherMapOneCallData.daily[i].dt > 0)
                {
                    log::dbg("dt: %s", String(openWeatherMapOneCallData.daily[i].dt).c_str());
                    log::dbg("temp: %s %s",String(openWeatherMapOneCallData.daily[i].tempDay, 1).c_str(), (true ? "°C" : "°F") );
                    log::dbg("desc: %s",  openWeatherMapOneCallData.daily[i].weatherDescription.c_str());
                }
            }


            msg += tr(M_WEATHER_NAME) + String(": ");
            // Now
            // msg += tr(M_WEATHER_NOW) + String("-") + tr(M_WEATHER_TEMP_SHORT) + ":" +\
            //         String(openWeatherMapOneCallData.current.temp, 1).c_str() + tr(M_COMMON_DEG_CELS) + "," + \
            //         openWeatherMapOneCallData.current.weatherDescription.c_str();
            // Today
            if (openWeatherMapOneCallData.daily[0].dt > 0)
            {
                msg += /*String(", ") + */tr(M_WEATHER_TODAY) + String(": ") + tr(M_WEATHER_TEMP_SHORT) + ":" + \
                        String(openWeatherMapOneCallData.daily[0].tempDay, 1).c_str() + tr(M_COMMON_DEG_CELS) + "," + \
                        openWeatherMapOneCallData.daily[0].weatherDescription.c_str();
            }
            if (openWeatherMapOneCallData.daily[1].dt > 0)
            {
                msg += String(", ") + tr(M_WEATHER_TOMM) + String(": ") + tr(M_WEATHER_TEMP_SHORT) + ":" + \
                        String(openWeatherMapOneCallData.daily[1].tempDay, 1).c_str() + tr(M_COMMON_DEG_CELS) + "," + \
                        openWeatherMapOneCallData.daily[1].weatherDescription.c_str();
            }
            AppCtx.putDisplayMsg(msg.c_str(), msg.length());
        }
        else
        {
            log::err("%s Weather forecasts read failed!", MODULE_NAME);
        }


        // Reset time to next measurement
        updateIntervalSeconds = 0;
    }
}

//------------------------------------------------------------------------------
