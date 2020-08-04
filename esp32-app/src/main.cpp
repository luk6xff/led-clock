#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
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

#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW//FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   35// SPI SCK
#define CS_PIN    5 // SPI SS

#define MAX_CLK_ZONES 1
#define ZONE_SIZE 4
#define MAX_DEVICES (MAX_CLK_ZONES * ZONE_SIZE)
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define ZONE_UPPER  1
#define ZONE_LOWER  0

#define SPEED_TIME  75
#define PAUSE_TIME  0

#define TIME_MSG_LEN  10

// Hardware adaptation parameters for scrolling
bool invertUpperZone = false;

// Turn on debug statements to the serial output
#define  DEBUG  0

// Global variables
char  time_msg_buf[TIME_MSG_LEN];    // hh:mm:ss\0

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

// void createHString(char *pH, char *pL)
// {
//   for (; *pL != '\0'; pL++)
//     *pH++ = *pL | 0x80;   // offset character

//   *pH = '\0'; // terminate the string
// }

void setup(void)
{

    Serial.begin(9600);
    // Create blink task
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello!");
    //P.begin();
//    P.getGraphicObject()->transform(MD_MAX72XX::transformType_t::TRC);
    //P.displayText("HeLLo!", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    //P.getGraphicObject()->transform(MD_MAX72XX::transformType_t::TRC);

//   invertUpperZone = (HARDWARE_TYPE == MD_MAX72XX::GENERIC_HW || HARDWARE_TYPE == MD_MAX72XX::PAROLA_HW);

//   // initialise the LED display
  P.begin(MAX_CLK_ZONES);

//   // Set up zones for 2 halves of the display
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE);
  //P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES - 1);
//   P.setFont(numeric7SegDouble);

//   P.setCharSpacing(P.getCharSpacing() * 2); // double height --> double spacing

//   if (invertUpperZone)
//   {
//     P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_UD);
//     P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_LR);

//     P.displayZoneText(ZONE_LOWER, time_msg_buf, PA_RIGHT, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
//     P.displayZoneText(ZONE_UPPER, szTimeH, PA_LEFT, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
//   }
//   else
//   {
     P.displayZoneText(ZONE_LOWER, time_msg_buf, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
//     P.displayZoneText(ZONE_UPPER, szTimeH, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
//   }

// #if USE_DS1307
//   RTC.control(DS1307_CLOCK_HALT, DS1307_OFF);
//   RTC.control(DS1307_12H, DS1307_OFF);
// #endif
}

void loop(void)
{
  static uint32_t	lastTime = 0; // millis() memory
  static bool	flasher = false;  // seconds passing flasher

  P.displayAnimate();
  if (P.getZoneStatus(ZONE_LOWER))// && P.getZoneStatus(ZONE_UPPER))
  {
    // Adjust the time string if we have to. It will be adjusted
    // every second at least for the flashing colon separator.
    if (millis() - lastTime >= 1000)
    {
      lastTime = millis();
      getTime(time_msg_buf, flasher);
      flasher = !flasher;

      P.displayReset();

      // synchronise the start
      P.synchZoneStart();
    }
  }
}