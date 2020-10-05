

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

    #include <RTClib.h>

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
    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    RTC_DS3231 rtc;
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower())
    {
        Serial.println("RTC lost power, lets set the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
#endif

#ifdef TEST_RADIO
    Radio radio;
#endif


///<! LOOP
    while(1)
    {

#ifdef TEST_DISPLAY
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
#endif // TEST_DISPLAY


#ifdef TEST_LIGHT_SENSOR
        Serial.printf("BH1750 Light: %3.2f [lx]\r\n", light.getIlluminance());
#endif // TEST_LIGHT_SENSOR


#ifdef TEST_RTC
        Serial.printf("SystemRTC temperature: %3.2f\r\n", SystemRtc::instance().get_temperature());
        // DateTime now = rtc.now();
        // Serial.println("Current Date & Time: ");
        // Serial.print(now.year(), DEC);
        // Serial.print('/');
        // Serial.print(now.month(), DEC);
        // Serial.print('/');
        // Serial.print(now.day(), DEC);
        // Serial.print(" (");
        // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
        // Serial.print(") ");
        // Serial.print(now.hour(), DEC);
        // Serial.print(':');
        // Serial.print(now.minute(), DEC);
        // Serial.print(':');
        // Serial.print(now.second(), DEC);
        // Serial.println();

        // Serial.println("Unix Time: ");
        // Serial.print("elapsed ");
        // Serial.print(now.unixtime());
        // Serial.print(" seconds/");
        // Serial.print(now.unixtime() / 86400L);
        // Serial.println(" days since 1/1/1970");

        // // calculate a date which is 7 days & 30 seconds into the future
        // DateTime future (now + TimeSpan(7,0,0,30));

        // Serial.println("Future Date & Time (Now + 7days & 30s): ");
        // Serial.print(future.year(), DEC);
        // Serial.print('/');
        // Serial.print(future.month(), DEC);
        // Serial.print('/');
        // Serial.print(future.day(), DEC);
        // Serial.print(' ');
        // Serial.print(future.hour(), DEC);
        // Serial.print(':');
        // Serial.print(future.minute(), DEC);
        // Serial.print(':');
        // Serial.print(future.second(), DEC);
        // Serial.println();
        // Serial.println();
#endif // TEST_RTC

        delay(1000);
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
