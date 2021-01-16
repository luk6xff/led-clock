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

    Serial.println("Welcome to test LORA Radio app");

    // Enable radio
    radio_init();
    Serial.println("LORA init succeeded.");
}

//-----------------------------------------------------------------------------
void loop()
{
    const int sleep_minutes = 5;
    static int msgCount = 0;
    sendSensorMsg();
    Serial.printf("Sending msgCount:%d\r\n", msgCount++);
    for (int i = 0; i < sleep_minutes; i++)
    {
        delay(60000);
    }

}

//-----------------------------------------------------------------------------
void sendSensorMsg()
{
    static float temp = -40.0f;
    temp += 0.1f;

    radio_msg_sensor_frame msgf;
    msgf.temperature = temp;
    msgf.humidity = 11.99f;
    msgf.pressure = 109900;
    msgf.vbatt = 3300;
    radio_send(&msgf);
}

//-----------------------------------------------------------------------------
