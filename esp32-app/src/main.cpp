#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

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
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

#if !CONFIG_AUTOSTART_ARDUINO
void arduinoTask(void *pvParameter) {
    pinMode(LED_BUILTIN, OUTPUT);
    while(1) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(1000);
    }
}

void app_main()
{
    // initialize arduino library before we start the tasks
    initArduino();

    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&arduinoTask, "arduino_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
#else

#include <MD_MAX72xx.h>
#include <SPI.h>

#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 4

#define CLK_PIN   35//18  // or SCK
#define DATA_PIN  //23  // or MOSI
#define CS_PIN    5// or SS


#include <LEDMatrixDriver.hpp>
// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are Arduino specific SPI pins (MOSI=DIN, SCK=CLK of the LEDMatrix) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 5;

// Number of 8x8 segments you are connecting
const int LEDMATRIX_SEGMENTS = 4;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);


    // GENERIC_HW,   ///< Use 'generic' style hardware modules commonly available.
    // FC16_HW,      ///< Use FC-16 style hardware module.
    // PAROLA_HW,    ///< Use the Parola style hardware modules.
    // ICSTATION_HW, ///< Use ICStation style hardware module.
// SPI hardware interface
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary pins
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Text parameters
#define CHAR_SPACING  1 // pixels between characters

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  5
char message[BUF_SIZE] = "Helo";
bool newMessageAvailable = true;

void readSerial(void)
{
  static uint8_t putIndex = 0;

  while (Serial.available())
  {
    message[putIndex] = (char)Serial.read();
    if ((message[putIndex] == '\n') || (putIndex >= BUF_SIZE-3))  // end of message character or full buffer
    {
      // put in a message separator and end the string
      message[putIndex] = '\0';
      // restart the index for next filling spree and flag we have a message waiting
      putIndex = 0;
      newMessageAvailable = true;
    }
    else
      // Just save the next char in next location
      message[putIndex++];
  }
}

// void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// // Print the text string to the LED matrix modules specified.
// // Message area is padded with blank columns after printing.
// {
//   uint8_t   state = 0;
//   uint8_t   curLen;
//   uint16_t  showLen;
//   uint8_t   cBuf[8];
//   int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

//   mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

//   do     // finite state machine to print the characters in the space available
//   {
//     switch(state)
//     {
//       case 0: // Load the next character from the font table
//         // if we reached end of message, reset the message pointer
//         if (*pMsg == '\0')
//         {
//           showLen = col - (modEnd * COL_SIZE);  // padding characters
//           state = 2;
//           break;
//         }

//         // retrieve the next character form the font file
//         showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
//         curLen = 0;
//         state++;
//         // !! deliberately fall through to next state to start displaying

//       case 1: // display the next part of the character
//         mx.setColumn(col--, cBuf[curLen++]);

//         // done with font character, now display the space between chars
//         if (curLen == showLen)
//         {
//           showLen = CHAR_SPACING;
//           state = 2;
//         }
//         break;

//       case 2: // initialize state for displaying empty columns
//         curLen = 0;
//         state++;
//         // fall through

//       case 3:	// display inter-character spacing or end of message padding (blank columns)
//         mx.setColumn(col--, 0);
//         curLen++;
//         if (curLen == showLen)
//           state = 0;
//         break;

//       default:
//         col = -1;   // this definitely ends the do loop
//     }
//   } while (col >= (modStart * COL_SIZE));

//   mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
// }



void setup()
{
    Serial.begin(9600);
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello!");
    Serial.print("\n[MD_MAX72XX Message Display]\nType a message for the display\nEnd message line with a newline");
    //mx.begin();

	// init the display
	lmd.setEnabled(true);
	lmd.setIntensity(0);   // 0 = low, 10 = high
}


const int ANIM_DELAY = 200;
int x = 0, s = 1, dir = 3, c = 0;

void loop()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    //delay(1000);
    //printText(0, MAX_DEVICES-1, "test");
    // readSerial();
    // if (newMessageAvailable)
    // {
    //     PRINT("\nProcessing new message: ", message);
    //     printText(0, MAX_DEVICES-1, message);
    //     newMessageAvailable = false;
    // }


  switch( dir )
  {
    case 0:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollDown);
      lmd.setPixel(x,0,true);
      break;
    case 1:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollUp);
      lmd.setPixel(x,7,true);
      break;
    case 2:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollLeft);
      lmd.setPixel(7,x,true);
      break;
    case 3:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollRight);
      lmd.setPixel(0,x,true);
      break;
  };

  // Show the content of the frame buffer
  lmd.display();

  // Increase / Reset the effect counter
  if( c++ > 50 )  // after 50 loos
  {
    //if( ++dir > 3 ) // we switch to the next effect
    //dir = 0;
    // And reset counter and pixel values
    //c=0;
    //x=0;
    //s=1;

    // Also clearing the buffer gives a better look to this example.
    //lmd.clear();
  }

  // Move the pixel up and down
  x += s;
  if( x == 7 )
    s = -1;
  else if( x == 0 )
    s = +1;

  delay(ANIM_DELAY);
}
#endif