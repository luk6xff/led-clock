#include "app.h"
#include "app_vars.h"
#include "Config/Config.h"

#include <Arduino.h>

// APP
#include "Config/AppConfigParam.h"
#include "app_context.h"

// MISC
#include <esp_task_wdt.h>
#include <sdkconfig.h>


//------------------------------------------------------------------------------
App::App()
    : m_appStatusTask(nullptr)
    , m_wifiTask(nullptr)
    , m_dispTask(nullptr)
    , m_clockTask(nullptr)
    , m_ntpTask(nullptr)
    , m_weatherTask(nullptr)
    , m_radioSensorTask(nullptr)
    , m_intEnvDataTask(nullptr)
{

}

//------------------------------------------------------------------------------
App::~App()
{
    Cfg.close();
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
    logger::init();
    printMotd();
    Cfg.init();
    AppCtx.setAppLang((reinterpret_cast<AppConfigParam)(Cfg.getCfgParam(CFG_KEY_APP)).getData().appLang);
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
    m_appStatusTask = std::unique_ptr<AppStatusTask>(new AppStatusTask());
    m_wifiTask = std::unique_ptr<WifiTask>(new WifiTask(Cfg.getCurrent().wifi));
    m_ntpTask = std::unique_ptr<NtpTask>(new NtpTask(Cfg.getCurrent().time,
                                            m_wifiTask->getWifiEvtHandle()));
    m_intEnvDataTask = std::unique_ptr<IntEnvDataTask>(new IntEnvDataTask(Cfg.getCurrent().intEnv));
    m_clockTask = std::unique_ptr<ClockTask>(new ClockTask(Cfg.getCurrent().time,
                                                m_ntpTask->getNtpTimeQ()));
    m_dispTask = std::unique_ptr<DisplayTask>(new DisplayTask(Cfg.getCurrent().display, m_clockTask->getTimeQ()));
    m_weatherTask = std::unique_ptr<WeatherTask>(new WeatherTask(Cfg.getCurrent().weather));
    m_radioSensorTask = std::unique_ptr<RadioSensorTask>(new RadioSensorTask(Cfg.getCurrent().radio));
}

//------------------------------------------------------------------------------
void App::runTasks()
{
    // Run all the tasks if created
    if (m_appStatusTask)
    {
        m_appStatusTask->start();
    }

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

    if (m_intEnvDataTask)
    {
        m_intEnvDataTask->start();
    }
}

//------------------------------------------------------------------------------
void App::printMotd()
{
    logger::inf("\r\nStarting <LUK6XFF LED_CLOCK 2020>\r\nSystem running on CPU %d at %d MHz.\r\nApp Version %s.\r\nFree heap memory %d\r\n",
        xPortGetCoreID(),
        ESP.getCpuFreqMHz(),
        APP_VERSION,
        ESP.getFreeHeap());
}

//------------------------------------------------------------------------------