
#include "tests.h"

#include "../App/app.h"
#include "../App/utils.h"
#include "../hw_config.h"
#include "../Display/display.h"
#include "../Clock/system_rtc.h"
#include "../Display/light_sensor.h"
#include "../Radio/radio.h"


//------------------------------------------------------------------------------
/**
 * @brief Defines
 */
#define BLINK_GPIO (gpio_num_t)CONFIG_BLINK_GPIO
#ifndef LED_BUILTIN
    #define LED_BUILTIN 4
#endif

//------------------------------------------------------------------------------
/**
 * @brief FreeRtos demo blink task
 */
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
void tests_setup()
{
    // Utils init
    utils::init();
    //utils::util_i2c_scanner();

    // Create blink task
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}


//------------------------------------------------------------------------------
void tests_run()
{

    static bool	timeDots = true;
    static uint32_t timeLast = 0;
    static uint32_t timeSecCnt = 0;


    #define TEST_DISPLAY
    //#define TEST_LIGHT_SENSOR
    //#define TEST_RTC
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

    #if defined(TEST_RTC) || defined(TEST_DISPLAY)
        SystemRtc rtc;
    #endif

    #ifdef TEST_RADIO
        Radio radio;
    #endif

    ///<! LOOP
        timeLast = millis();
        timeSecCnt =  millis();

        Display::DateTimePrintMode timeDispMode = Display::THM;
        while(1)
        {

    #ifdef TEST_DISPLAY
            disp.update();
            DateTime dt = rtc.getTime();

            if (millis() - timeLast >= 10000) // 10[s]
            {
                disp.printTime(dt, Display::DWYMD);
                timeLast = millis();
                if (timeDispMode == Display::THM)
                {
                    timeDispMode = Display::THMS;
                }
                else
                {
                    timeDispMode = Display::THM;
                }

            }
            else
            {
                disp.printTime(dt, timeDispMode, timeDots);
            }

            if (millis() - timeSecCnt >= 1000) // 1[s]
            {
                timeDots = timeDots ^ 1;
                timeSecCnt = millis();
            }


    #endif // TEST_DISPLAY


    #ifdef TEST_LIGHT_SENSOR
            Serial.printf("BH1750 Light: %3.2f [lx]\r\n", light.getIlluminance());
    #endif // TEST_LIGHT_SENSOR


    #ifdef TEST_RTC
            Serial.printf("SystemRTC temperature: %3.2f\r\n", rtc.getTemperature());
            Serial.printf("SystemRTC time: %s\r\n", SystemRtc::timeToStr(rtc.getTime()));
            Serial.printf("SystemRTC date: %s\r\n", SystemRtc::dateToStr(rtc.getTime()));
    #endif // TEST_RTC

        }
}

//------------------------------------------------------------------------------
