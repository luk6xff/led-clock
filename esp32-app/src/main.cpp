#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include "nvs_flash.h"
#include "esp_event.h"

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

#include "hw_config.h"
#include "Display/display.h"
#include "Clock/system_rtc.h"
#include "Display/light_sensor.h"
#include "Radio/radio.h"


//------------------------------------------------------------------------------
/**
 * @brief Test function for each device module - used during development
 */
static void test_modules();
static void get_time(char *psz, bool f = true);


//------------------------------------------------------------------------------
#define BLINK_GPIO (gpio_num_t)CONFIG_BLINK_GPIO
#ifndef LED_BUILTIN
    #define LED_BUILTIN 4
#endif
void blink_task(void *pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1)
    {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}


//------------------------------------------------------------------------------
void setup(void)
{
    Serial.begin(9600);
    // Create blink task
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello!");
}

//------------------------------------------------------------------------------
void loop(void)
{
    test_modules();
}

//------------------------------------------------------------------------------
static void test_modules()
{
    // Setup
    Display::MAX72xxConfig cfg =
    {
        DISPLAY_MAX72XX_HW_TYPE,
        DISPLAY_MAX72XX_MODULES_NUM,
        DISPLAY_DIN_PIN,
        DISPLAY_CLK_PIN,
        DISPLAY_CS_PIN,
    };
    Display disp(cfg);
    LightSensor light;
    Radio radio;
    SystemRtc::instance();

    // Loop
    while(1)
    {
        static uint32_t	last_time = 0; // millis() memory
        static bool	flasher = false;   // seconds passing flasher
        disp.update();
        if (disp.getDispObject()->getZoneStatus(DISPLAY_ZONE_0))
        {
            if (millis() - last_time >= 1000)
            {
                last_time = millis();
                get_time((char*)disp.getDispTxtBuffer(), flasher);
                flasher = !flasher;
                disp.reset();
            }
        }
        static int c = 0;
        if (c++ % 10000 == 0)
        {
            Serial.printf("Test: %d\r\n", c);
        }
    }
}

//------------------------------------------------------------------------------
static void get_time(char *psz, bool f)
{
    uint16_t  h, m;
    m = millis()/1000;
    h = (m / 60) % 24;
    m %= 60;
    sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
}

//------------------------------------------------------------------------------
