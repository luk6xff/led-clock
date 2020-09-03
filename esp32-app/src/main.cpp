

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


// //------------------------------------------------------------------------------
// /**
//  * @brief Test function for each device module - used during development
//  */
// static void test_modules();
// static void get_time(char *psz, bool f = true);


// //------------------------------------------------------------------------------
// #define BLINK_GPIO (gpio_num_t)CONFIG_BLINK_GPIO
// #ifndef LED_BUILTIN
//     #define LED_BUILTIN 4
// #endif
// void blink_task(void *pvParameter)
// {
//     gpio_pad_select_gpio(BLINK_GPIO);
//     /* Set the GPIO as a push/pull output */
//     gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
//     while(1)
//     {
//         /* Blink off (output low) */
//         gpio_set_level(BLINK_GPIO, 0);
//         vTaskDelay(500 / portTICK_PERIOD_MS);
//         /* Blink on (output high) */
//         gpio_set_level(BLINK_GPIO, 1);
//         vTaskDelay(500 / portTICK_PERIOD_MS);
//     }
// }


// //------------------------------------------------------------------------------
// void setup(void)
// {

//     // Create blink task
//     //xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
//     pinMode(LED_BUILTIN, OUTPUT);

//     App::instance().setup();
//     util_i2c_scanner();
// }

// //------------------------------------------------------------------------------
// void loop(void)
// {
//     test_modules();
// }

// //------------------------------------------------------------------------------
// static void test_modules()
// {
//     // Setup
//     Display::MAX72xxConfig cfg =
//     {
//         DISPLAY_MAX72XX_HW_TYPE,
//         DISPLAY_MAX72XX_MODULES_NUM,
//         DISPLAY_DIN_PIN,
//         DISPLAY_CLK_PIN,
//         DISPLAY_CS_PIN,
//     };
//     Display disp(cfg);
//     LightSensor light;
//     Radio radio;

//     // Loop
//     while(1)
//     {
//         static uint32_t	last_time = 0; // millis() memory
//         static bool	flasher = false;   // seconds passing flasher
//         disp.update();
//         if (disp.getDispObject()->getZoneStatus(DISPLAY_ZONE_0))
//         {
//             if (millis() - last_time >= 1000)
//             {
//                 last_time = millis();
//                 get_time((char*)disp.getDispTxtBuffer(), flasher);
//                 flasher = !flasher;
//                 disp.reset();
//             }
//         }
//         static int c = 0;
//         if (c++ % 100000 == 0)
//         {
//             //Serial.printf("Test: %d\r\n", c);
//             Serial.printf("SystemRTC temperature: %3.2f\r\n", SystemRtc::instance().get_temperature());

//         }
//     }
// }

// //------------------------------------------------------------------------------
// static void get_time(char *psz, bool f)
// {
//     uint16_t  h, m;
//     m = millis()/1000;
//     h = (m / 60) % 24;
//     m %= 60;
//     sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
// }

// //------------------------------------------------------------------------------
#include <Wire.h>
#include <RTClib.h>
#include <MAX44009.h>

MAX44009 light;

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup ()
{
  Serial.begin(9600);
  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");

	// Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }

    if(light.begin())
    {
        Serial.println("Could not find a valid MAX44009 sensor, check wiring!");
        while(1);
    }

}

void loop ()
{
    DateTime now = rtc.now();

    Serial.println("Current Date & Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.println("Unix Time: ");
    Serial.print("elapsed ");
    Serial.print(now.unixtime());
    Serial.print(" seconds/");
    Serial.print(now.unixtime() / 86400L);
    Serial.println(" days since 1/1/1970");

    // calculate a date which is 7 days & 30 seconds into the future
    DateTime future (now + TimeSpan(7,0,0,30));

    Serial.println("Future Date & Time (Now + 7days & 30s): ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();

    Serial.println();
    delay(1000);

    Serial.print("Light (lux):    ");
    Serial.println(light.get_lux());
    delay(1000);
}