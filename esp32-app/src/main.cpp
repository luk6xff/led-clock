

#include "App/app.h"
#include "App/utils.h"
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
    utils::init();
    // Create blink task
    //xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    App::instance().setup();
    //utils::util_i2c_scanner();
}

//------------------------------------------------------------------------------
void loop(void)
{
    test_modules();
}

//------------------------------------------------------------------------------
static void test_modules()
{

#define TEST_DISPLAY
#define TEST_LIGHT_SENSOR
#define TEST_RTC
//#define TEST_RADIO

///<! SETUP
#ifdef TEST_DISPLAY
    Display::MAX72xxConfig cfg =
    {
        DISPLAY_MAX72XX_HW_TYPE,
        DISPLAY_MAX72XX_MODULES_NUM,
        DISPLAY_DIN_PIN,
        DISPLAY_CLK_PIN,
        DISPLAY_CS_PIN,
    };
    Display disp(cfg);
#endif

#ifdef TEST_LIGHT_SENSOR
    LightSensor light;
#endif

#ifdef TEST_RTC
    SystemRtc rtc;
#endif

#ifdef TEST_RADIO
    Radio radio;
#endif


///<! LOOP
    while(1)
    {

#ifdef TEST_DISPLAY
        static bool	flasher = false;
        disp.update();
        disp.printTime(rtc.getTime(), Display::MHS, flasher);
        flasher ^= 1;
#endif // TEST_DISPLAY


#ifdef TEST_LIGHT_SENSOR
        Serial.printf("BH1750 Light: %3.2f [lx]\r\n", light.getIlluminance());
#endif // TEST_LIGHT_SENSOR


#ifdef TEST_RTC
        Serial.printf("SystemRTC temperature: %3.2f\r\n", rtc.getTemperature());
        Serial.printf("SystemRTC time: %s\r\n", SystemRtc::timeToStr(rtc.getTime()));
        Serial.printf("SystemRTC date: %s\r\n", SystemRtc::dateToStr(rtc.getTime()));
#endif // TEST_RTC

        delay(1000);
    }
}

//------------------------------------------------------------------------------
