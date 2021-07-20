#include <Arduino.h>
#include "hw-config.h"
#include "radio.h"

//-----------------------------------------------------------------------------
static void sendSensorMsg();

//-----------------------------------------------------------------------------
void setup()
{
    Serial.begin(9600);                   // initialize serial
    while (!Serial);

    int delayTime = 30;
    while(delayTime--)
    {
        delay(100);
        Serial.printf(".");
    }
    Serial.println("\nWelcome to test LORA Radio app");


    // Enable radio
    radio_init();
    Serial.println("LORA init succeeded.");
}

//-----------------------------------------------------------------------------
void loop()
{
    static int msgCount = 0;
    sendSensorMsg();
    Serial.printf("Sending msgCount:%d\r\n", msgCount++);
    for (int i = 0; i < radio_get_last_clock_response()->update_data_interval; i++)
    {
        delay(1000);
    }

}

//-----------------------------------------------------------------------------
void sendSensorMsg()
{
    static float temp = 22.3f;
    temp += 0.1f;

    radio_msg_sensor_frame msgf;
    msgf.temperature = temp;
    msgf.humidity = 46.f;
    msgf.pressure = 96900; // [Pa]
    msgf.vbatt = 3300;
    radio_send(&msgf);
}

//-----------------------------------------------------------------------------
