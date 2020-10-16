#include "app.h"

#include <Arduino.h>

// APP
#include "utils.h"

// MISC
#include <esp_task_wdt.h>
#include <esp_partition.h>
#include "nvs_flash.h"
#include "esp_event.h"
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
    inf("\r\nStarting <LUK6XFF LED_CLOCK 2020>\r\nSystem running on CPU %d at %d MHz.\r\nApp Version %s.\r\nFree heap memory %d\r\n",
        xPortGetCoreID(),
        ESP.getCpuFreqMHz(),
        VERSION,
        ESP.getFreeHeap()); // Normally about 170 kB

    // Create all the tasks and run them
    //m_dispTask = std::unique_ptr<DisplayTask>(new DisplayTask);
    m_dispTask = new DisplayTask;
    //m_clockTask = new ClockTask(*m_dispTask);
}

//------------------------------------------------------------------------------
void App::run()
{

}

//------------------------------------------------------------------------------