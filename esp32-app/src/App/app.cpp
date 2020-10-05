#include "app.h"

#include <Arduino.h>

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <esp_task_wdt.h>
#include <esp_partition.h>


#include "nvs_flash.h"
#include "esp_event.h"

#include <driver/gpio.h>
#include <sdkconfig.h>


// Define the version number
#define VERSION     "0.0.1"



//------------------------------------------------------------------------------
App::App()
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
    Serial.begin(9600);
    Serial.printf( "\r\nStarting <LUK6XFF LED_CLOCK 2020> running on CPU %d at %d MHz.  Version %s.  Free memory %d\r\n",
                    xPortGetCoreID(),
                    ESP.getCpuFreqMHz(),
                    VERSION,
                    ESP.getFreeHeap() ) ;                       // Normally about 170 kB
}