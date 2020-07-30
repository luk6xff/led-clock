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


#include <MD_MAX72xx.h>
#include <SPI.h>
// We always wait a bit between updates of the display
#define  DELAYTIME  300  // in milliseconds
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
//LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);


    // GENERIC_HW,   ///< Use 'generic' style hardware modules commonly available.
    // FC16_HW,      ///< Use FC-16 style hardware module.
    // PAROLA_HW,    ///< Use the Parola style hardware modules.
    // ICSTATION_HW, ///< Use ICStation style hardware module.
// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary pins
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Global variables
uint32_t  lastTime = 0;

typedef struct
{
  uint8_t startDev; // start of zone
  uint8_t endDev;   // end of zone
  uint8_t ch;       // character to show
  MD_MAX72XX::transformType_t tt;
} zoneDef_t;

zoneDef_t Z[] =
{
#if MAX_DEVICES == 4
  {0, 0, 49, MD_MAX72XX::TRC  },
  {1, 1, 50, MD_MAX72XX::TRC  },
  {2, 2, 51, MD_MAX72XX::TRC  },
  {3, 3, 52, MD_MAX72XX::TRC  },
#endif // MAX_DEVICES 4
};

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

void runTransformation(void)
{
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  for (uint8_t i = 0; i < ARRAY_SIZE(Z); i++)
  {
    mx.transform(Z[i].startDev, Z[i].endDev, Z[i].tt);
  }

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}



void setup()
{
    Serial.begin(9600);
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello!");
    mx.begin();
    mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::ON);

    // set up the display characters
    for (uint8_t i = 0; i < ARRAY_SIZE(Z); i ++)
    {
        mx.clear(Z[i].startDev, Z[i].endDev);
        mx.setChar((Z[i].startDev*COL_SIZE)+COL_SIZE/2, Z[i].ch);
    }
    lastTime = millis();
    // Enable the display
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    mx.control(MD_MAX72XX::INTENSITY, 0);
    runTransformation();
  // // init the display
	// lmd.setEnabled(true);
	// lmd.setIntensity(0);   // 0 = low, 10 = high
}


const int ANIM_DELAY = 200;
int x = 0, s = 1, dir = 3, c = 0;

void loop()
{
  if (millis() - lastTime >= DELAYTIME)
  {
    //runTransformation();
    lastTime = millis();
  }


//   switch( dir )
//   {
//     case 0:
//       lmd.scroll(LEDMatrixDriver::scrollDirection::scrollDown);
//       lmd.setPixel(x,0,true);
//       break;
//     case 1:
//       lmd.scroll(LEDMatrixDriver::scrollDirection::scrollUp);
//       lmd.setPixel(x,7,true);
//       break;
//     case 2:
//       lmd.scroll(LEDMatrixDriver::scrollDirection::scrollLeft);
//       lmd.setPixel(7,x,true);
//       break;
//     case 3:
//       lmd.scroll(LEDMatrixDriver::scrollDirection::scrollRight);
//       lmd.setPixel(0,x,true);
//       break;
//   };

//   // Show the content of the frame buffer
//   lmd.display();

//   // Increase / Reset the effect counter
//   if( c++ > 50 )  // after 50 loos
//   {
//     //if( ++dir > 3 ) // we switch to the next effect
//     //dir = 0;
//     // And reset counter and pixel values
//     //c=0;
//     //x=0;
//     //s=1;

//     // Also clearing the buffer gives a better look to this example.
//     //lmd.clear();
//   }

//   // Move the pixel up and down
//   x += s;
//   if( x == 7 )
//     s = -1;
//   else if( x == 0 )
//     s = +1;

//   delay(ANIM_DELAY);
}