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

// Debug buffer size
#define DEBUG_BUFFER_SIZE 256

char* App::dbgPrint(const char* format, ... )
{
    static char sbuf[DEBUG_BUFFER_SIZE] ;                // For debug lines
    va_list varArgs ;                                    // For variable number of params

    va_start ( varArgs, format ) ;                       // Prepare parameters
    vsnprintf ( sbuf, sizeof(sbuf), format, varArgs ) ;  // Format the message
    va_end ( varArgs ) ;                                 // End of using parameters
    if ( DEBUG )                                         // DEBUG on?
    {
        Serial.print ( "D: " ) ;                           // Yes, print prefix
        Serial.println ( sbuf ) ;                          // and the info
    }
    return sbuf ;                                        // Return stored string
}



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