#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

#include "hw_config.h"
#include "Display/display.h"
#include "Clock/system_rtc.h"
#include "Display/light_sensor.h"


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
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Turn on debug statements to the serial output
#define  DEBUG  0

void getTime(char *psz, bool f = true)
// Code for reading clock time
// Simulated clock runs 1 minute every seond
{
#if USE_DS1307
  RTC.readTime();
  sprintf(psz, "%02d%c%02d", RTC.h, (f ? ':' : ' '), RTC.m);
#else
  uint16_t  h, m;

  m = millis()/1000;
  h = (m/60) % 24;
  m %= 60;
  sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
#endif
}


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


void setup(void)
{
    Serial.begin(9600);
    // Create blink task
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello!");
    auto rtc = SystemRtc::instance();
}


void loop(void)
{
    static uint32_t	lastTime = 0; // millis() memory
    static bool	flasher = false;  // seconds passing flasher

    disp.update();
    if (disp.getDispObject()->getZoneStatus(DISPLAY_ZONE_0))
    {
        // Adjust the time string if we have to. It will be adjusted
        // every second at least for the flashing colon separator.
        if (millis() - lastTime >= 1000)
        {
            lastTime = millis();
            getTime((char*)disp.getDispTxtBuffer(), flasher);
            flasher = !flasher;
            disp.reset();
        }
    }
}