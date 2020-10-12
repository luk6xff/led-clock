#if 1

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
static bool	flasher = true;

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
        disp.update();
        DateTime dt = rtc.getTime();
        disp.printTime(dt, Display::DWYMD, !flasher);
        flasher = flasher ^ 1;

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






#else // TEST


#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Parola_Fonts_data.h"
#include "hw_config.h"

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW
#define MAX_DEVICES 4
#define CLK_PIN   DISPLAY_CLK_PIN
#define DATA_PIN  DISPLAY_DIN_PIN
#define CS_PIN    DISPLAY_CS_PIN

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const uint16_t PAUSE_TIME = 2000;

// Turn on debug statements to the serial output
#define  DEBUG  1

#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(s, x)  { Serial.print(F(s)); Serial.print(x, HEX); }
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(s, x)
#endif

// Global variables
char	pc[][40] =
{
  "Ąą",
  "Ćć",
  "Ęę",
  "Łł",
  "Ńń",
  "Óó",
  "Śś",
  "Źź",
  "Żż",
  "°C",
};

uint8_t utf8Ascii(uint8_t ascii)
// Convert a single Character from UTF8 to Extended ASCII according to ISO 8859-1,
// also called ISO Latin-1. Codes 128-159 contain the Microsoft Windows Latin-1
// extended characters:
// - codes 0..127 are identical in ASCII and UTF-8
// - codes 160..191 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC2 then second byte identical to the extended ASCII code.
// - codes 192..255 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC3 then second byte differs only in the first two bits to extended ASCII code.
// - codes 128..159 in Windows-1252 are different, but usually only the €-symbol will be needed from this range.
//                 + The euro symbol is 0x80 in Windows-1252, 0xa4 in ISO-8859-15, and 0xe2 0x82 0xac in UTF-8.
//
// Modified from original code at http://playground.arduino.cc/Main/Utf8ascii
// Extended ASCII encoding should match the characters at http://www.ascii-code.com/
//
// Return "0" if a byte has to be ignored.
{
  static uint8_t cPrev;
  uint8_t c = '\0';

  if (ascii < 0x7f)   // Standard ASCII-set 0..0x7F, no conversion
  {
    cPrev = '\0';
    c = ascii;
  }
  else
  {
    switch (cPrev)  // Conversion depending on preceding UTF8-character
    {
        case 0xC2: c = ascii;  break;
        case 0xC3: c = ascii | 0xC0;  break;
        case 0xC4: c = ascii; break;
        case 0xC5: c = ascii | 0xD0;  break;
        case 0x82: if (ascii==0xAC) c = 0x80; // Euro symbol special case
    }
    cPrev = ascii;   // save last char
  }

  PRINTX("\nConverted 0x", ascii);
  PRINT(" ", ascii);
  PRINTX(" to 0x", c);
  PRINT(" dec:", c);

  return(c);
}


/**
 * @brief In place conversion UTF-8 string to Extended ASCII (+ some polish chars from Latin Extended-A)
 *        The extended ASCII string is always shorter.

 */
char* utf8Ascii(char *s)
{

    uint8_t prev_c = '\0';
    char *c_start = &s[0];
    char *cp = s;

    while (*s != '\0')
    {
        const uint8_t tmp_c = *s++;
        uint8_t c = '\0';

        if (tmp_c < 0x7f)   // Standard ASCII-set 0..0x7F, no conversion
        {
            prev_c = '\0';
            c = tmp_c;
        }
        else
        {
            switch (prev_c)  // Conversion depending on preceding UTF8-character
            {
                case 0xC2: c = tmp_c;  break;
                case 0xC3: c = tmp_c | 0xC0;  break;
                case 0xC4: c = tmp_c; break;
                case 0xC5: c = tmp_c | 0xD0;  break;
                case 0x82: if (tmp_c == 0xAC) c = 0x80; // Euro symbol special case
            }
            prev_c = tmp_c;   // save last char
        }

        if (c != '\0')
        {
            *cp++ = c;
        }
    }
    *cp = '\0';   // terminate the new string

    return c_start;
}

void setup(void)
{
#if DEBUG
  Serial.begin(9600);
#endif
  PRINTS("\n[Parola UTF-8 display Test]");

  // Do one time in-place conversion of the strings to be displayed
// for (uint8_t i=0; i<ARRAY_SIZE(pc); i++)
//     utf8Ascii(pc[i]);

  // Initialise the Parola library
  P.begin();
  P.setInvert(false);
  P.setPause(PAUSE_TIME);
  P.setIntensity(0);
  //P.setFont(ExtASCII);
  P.setFont(_5bite_rus);
}

void loop(void)
{
  static uint32_t timeLast = PAUSE_TIME;
  static uint16_t  idx = 0;
  static bool showMessage = true;

  if (millis() - timeLast <= PAUSE_TIME)
    return;

  timeLast = millis();

  if (showMessage)
  {
    // show the strings with the UTF-8 characters
    PRINT("\n\nS:", idx);
    PRINT(" - ", utf8Ascii(pc[idx]));
    P.print(utf8Ascii(pc[idx]));

    // Set the display for the next string.
    idx++;
    if (idx == ARRAY_SIZE(pc))
    {
      showMessage = false;
      idx = 255;
    }
  }
//   else
//   {
//     // show the list of characters in order
//     char szMsg[20];
//     sprintf(szMsg, "%3d %c", idx, idx);
//     PRINT("\nC:", idx);
//     PRINT(" - ", szMsg);
//     P.print(szMsg);

//     // set up for next character
//     idx++;
//     // if (idx == 201)
//     // {
//     //   idx = 224;
//     // }
//     if (idx == 256)
//     {
//       idx = 0;
//       showMessage = true;
//     }
//   }
}

#endif
