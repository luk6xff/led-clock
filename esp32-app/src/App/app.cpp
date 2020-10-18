#include "app.h"

#include <Arduino.h>

// APP
#include "utils.h"
#include "app_config.h"

// MISC
#include <esp_task_wdt.h>
#include <sdkconfig.h>



// Define the version number
#define VERSION     "0.0.1"


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
    m_dispTask = std::unique_ptr<DisplayTask>(new DisplayTask);
    m_clockTask = std::unique_ptr<ClockTask>(new ClockTask(*m_dispTask.get(),
                                                AppCfg.getCurrent().time));
    m_ntpTask = std::unique_ptr<NtpTask>(new NtpTask(AppCfg.getCurrent().ntp,
                                                m_wifiTask->getWifiEvtHandle(),
                                                m_clockTask->extSrcTimeQueue()));
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
}

//------------------------------------------------------------------------------
void App::printMotd()
{
    inf("\r\nStarting <LUK6XFF LED_CLOCK 2020>\r\nSystem running on CPU %d at %d MHz.\r\nApp Version %s.\r\nFree heap memory %d\r\n",
        xPortGetCoreID(),
        ESP.getCpuFreqMHz(),
        VERSION,
        ESP.getFreeHeap());
}

//------------------------------------------------------------------------------