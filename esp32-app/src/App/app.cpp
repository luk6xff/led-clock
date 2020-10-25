#include "app.h"

#include <Arduino.h>

// APP
#include "utils.h"
#include "app_config.h"

// MISC
#include <esp_task_wdt.h>
#include <sdkconfig.h>



// Define the app version number
#define APP_VERSION "0.0.1"


//------------------------------------------------------------------------------
App::App()
    : m_wifiTask(nullptr)
    , m_dispTask(nullptr)
    , m_clockTask(nullptr)
    , m_ntpTask(nullptr)
{

}

//------------------------------------------------------------------------------
App::~App()
{
    AppCfg.close();
}

//------------------------------------------------------------------------------
App& App::instance()
{
    static App app;
    return app;
}

//------------------------------------------------------------------------------
void App::setup()
{
    utils::init();
    printMotd();
    AppCfg.init();
    createTasks();
    runTasks();
}

//------------------------------------------------------------------------------
void App::run()
{

}

//------------------------------------------------------------------------------
void App::createTasks()
{
    // Create all the tasks
    m_wifiTask = std::unique_ptr<WifiTask>(new WifiTask(AppCfg.getCurrent().wifi));
    m_ntpTask = std::unique_ptr<NtpTask>(new NtpTask(AppCfg.getCurrent().ntp,
                                            m_wifiTask->getWifiEvtHandle()));
    m_clockTask = std::unique_ptr<ClockTask>(new ClockTask(AppCfg.getCurrent().time,
                                                m_ntpTask->getNtpTimeQ()));
    m_dispTask = std::unique_ptr<DisplayTask>(new DisplayTask(m_clockTask->getTimeQ()));
    m_weatherTask = std::unique_ptr<WeatherTask>(new WeatherTask(AppCfg.getCurrent().weather));
    m_radioSensorTask = std::unique_ptr<RadioSensorTask>(new RadioSensorTask(AppCfg.getCurrent().radioSensor));
}

//------------------------------------------------------------------------------
void App::runTasks()
{
    //Run all the tasks if created
    if (m_wifiTask)
    {
        m_wifiTask->start();
    }
    if (m_dispTask)
    {
        m_dispTask->start();
    }
    if (m_clockTask)
    {
        m_clockTask->start();
    }
    if (m_ntpTask)
    {
        m_ntpTask->start();
    }
    if (m_weatherTask)
    {
        m_weatherTask->start();
    }
    if (m_radioSensorTask)
    {
        m_radioSensorTask->start();
    }
}

//------------------------------------------------------------------------------
void App::printMotd()
{
    inf("\r\nStarting <LUK6XFF LED_CLOCK 2020>\r\nSystem running on CPU %d at %d MHz.\r\nApp Version %s.\r\nFree heap memory %d\r\n",
        xPortGetCoreID(),
        ESP.getCpuFreqMHz(),
        APP_VERSION,
        ESP.getFreeHeap());
}

//------------------------------------------------------------------------------