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
    : m_dispTask(nullptr)
    , m_clockTask(nullptr)
{

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
    connectToAp();
    createTasks();

    // m_dispTask = std::unique_ptr<DisplayTask>(new DisplayTask);
    // m_dispTask->start();
    m_clockTask = std::unique_ptr<ClockTask>(new ClockTask(*m_dispTask.get(),
                                                        AppCfg.getCurrent().time,
                                                        AppCfg.getCurrent().ntp));
    m_clockTask->start();
}

//------------------------------------------------------------------------------
void App::run()
{

}

//------------------------------------------------------------------------------
bool App::connectToAp()
{
    WiFi.setAutoConnect(0);
    int tries = 0;
    bool connected = true;
    // m_wifiMulti.addAP(AppCfg.getCurrent().wifi.ssid0,
    //                 AppCfg.getCurrent().wifi.pass0);
    // m_wifiMulti.addAP(AppCfg.getCurrent().wifi.ssid1,
    //                 AppCfg.getCurrent().wifi.pass1);
    // m_wifiMulti.addAP(AppCfg.getCurrent().wifi.ssid2,
    //                 AppCfg.getCurrent().wifi.pass2);

    inf("Connecting ...");
    WiFi.begin(AppCfg.getCurrent().wifi.ssid0, AppCfg.getCurrent().wifi.pass0);
    while (WiFi.status() != WL_CONNECTED)
    // while (m_wifiMulti.run() != WL_CONNECTED)
    {
        // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest network
        delay(100);
        tries++;
        inf(".");
        if (tries > 5)
        {
            err("Connecting to AP failed!!!");
            connected = false;
            break;
        }
    }

    if (connected)
    {
        inf("\nConnected to: %s\nIP address:\t %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }

    return connected;
}

//------------------------------------------------------------------------------
void App::createTasks()
{
    // Create all the tasks and run them
    // m_dispTask = std::unique_ptr<DisplayTask>(new DisplayTask);
    // m_dispTask->start();
    // m_clockTask = std::unique_ptr<ClockTask>(new ClockTask(*m_dispTask.get(),
    //                                                     AppCfg.getCurrent().time,
    //                                                     AppCfg.getCurrent().ntp));
    // m_clockTask->start();
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