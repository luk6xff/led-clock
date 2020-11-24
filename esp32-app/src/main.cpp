
// /**
//  * @brief Led-Clock app
//  *
//  * @author Lukasz Uszko - luk6xff
//  * @date   2020-10-18
//  */


// #define TESTS_OFF

// #ifdef TESTS_ON
//     #include "Tests/tests.h"
// #else
//     #include "App/app.h"
// #endif

// //------------------------------------------------------------------------------
// void setup(void)
// {
// #ifdef TESTS_ON
//     tests_setup();
// #else
//     App::instance().setup();
// #endif
//   //enableLoopWDT();
//   enableCore1WDT();
//   enableCore0WDT();
// }

// //------------------------------------------------------------------------------
// void loop(void)
// {
// #ifdef TESTS_ON
//     tests_run();
// #else
//     //App::instance().run();
//     vTaskDelete(nullptr);
// #endif
// }




#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "hw_config.h"



/**
 * @brief Radio Msg frame status field description
 */
typedef enum
{
	MSG_NO_ERROR        = 1<<0,
	MSG_READ_ERROR      = 1<<1,
	MSG_INIT_ERROR      = 1<<2,
	MSG_BATT_LOW        = 1<<3,
	MSG_CHECKSUM_ERROR  = 1<<4,
} radio_msg_sensor_frame_status;

/**
 * @brief Radio layer msg header footprint
 */
typedef struct
{
	uint8_t receiver_id;            // Receiver address
	uint8_t sender_id;              // Sender address
	uint8_t msg_id;                // Message ID
	uint8_t payload_len;           // Message payload length
} radio_layer_msg_header;

/**
 * @brief Msg frame footprint, sent to the clock
 */
typedef struct
{
	uint8_t hdr[3];
	uint8_t status;
	uint32_t vbatt;
	float temperature;
	float pressure;
	float humidity;
	uint32_t checksum;
} radio_msg_sensor_frame;

/**
 * @brief Msg frame footprint, received from the clock
 */
typedef struct
{
	uint8_t hdr[3];
	uint8_t status;
	uint32_t crit_vbatt_level;     //[mV]
	uint32_t update_data_interval; //[s] in seconds
	uint32_t checksum;
} radio_msg_clock_frame;



const int csPin = RADIO_NSS_PIN;          // LoRa radio chip select
const int resetPin = RADIO_RST_PIN;       // LoRa radio reset
const int irqPin = RADIO_DI0_PIN;         // change for your board; must be a hardware interrupt pin


volatile bool frameRecv = false;

void sendLedClockMsg();
void onReceive(int packetSize);

#define LORA_RADIO_LEDCLOCK_ID 0xAB
#define LORA_RADIO_SENSOR_ID 0xCD
#define LORA_RADIO_LEDCLOCK_SENSOR_MSG_ID 0x67

byte msgCnt = 0;            // count of outgoing messages
byte localAddr = LORA_RADIO_LEDCLOCK_ID;     // address of this device
byte destAddr = LORA_RADIO_SENSOR_ID;      // destAddr to send to

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Duplex with callback");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.onReceive(onReceive);

  //LoRa.dumpRegisters(Serial);
  LoRa.receive();
  Serial.println("LoRa init succeeded.");
}

void loop() {
  // if (millis() - lastSendTime > interval) {
  //   String message = "HeLoRa From ESP8266";   // send a message
  //   //sendMessage(message);
  //   sendLedClockMsg();
  //   Serial.println("Sending " + message);
  //   lastSendTime = millis();            // timestamp the message
  //   interval = random(20);//random(2000) + 1000;     // 2-3 seconds
  //   //LoRa.dumpRegisters(Serial);
  //   LoRa.receive();                     // go back into receive mode
  // }

  if (frameRecv)
  {
    String message = "HeLoRa From ESP8266";   // send a message
    sendLedClockMsg();
    frameRecv = false;
    Serial.println("Sending " + message + " ID:" + String(msgCnt));
    LoRa.receive();                     // go back into receive mode
  }
  delay(2);
}



//-----------------------------------------------------------------------------
uint16_t radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len)
{
    uint8_t i;
    uint16_t sum = 0;

    for (i = 0; i < data_len; i++)
    {
    	sum = sum ^ data[i];
    }
    return sum;
}


void sendLedClockMsg() {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destAddr);              // add destAddr address
  LoRa.write(localAddr);             // add sender address
  LoRa.write(msgCnt);                 // add message ID

  LoRa.write(sizeof(radio_msg_clock_frame));        // add payload length

  radio_msg_clock_frame msgf;
  // Update clock msg cfg structure
  msgf =
  {
      .hdr = {'L','U','6'},
      .status = MSG_NO_ERROR,
      .crit_vbatt_level     = 3300,     //[mV]
      .update_data_interval = 50, //[s]
  };
  msgf.checksum = radio_msg_frame_checksum((const uint8_t*)&msgf, (sizeof(radio_msg_clock_frame)-sizeof(msgf.checksum)));
  uint8_t s = 0;
  while (s < sizeof(msgf)) // add payload
  {
    LoRa.write((uint8_t)((const uint8_t*)&msgf)[s++]);
  }
  LoRa.endPacket();                     // finish packet and send it
  msgCnt++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0)
  {
      Serial.println(">>>>>>>>>packetSize == 0");
      return;          // if there's no packet, return
  }


  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";                 // payload of packet

  while (LoRa.available()) {            // can't use readString() in callback, so
    incoming += (char)LoRa.read();      // add bytes one by one
  }

  // if (incomingLength != incoming.length()) {   // check length for error
  //   Serial.println("error: message length does not match length");
  //   return;                             // skip rest of function
  // }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddr && recipient != 0xFF) {
    Serial.println(">>>>>>>>>This message is not for me. ID:" + String(recipient));
    frameRecv = false;
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  // Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
  frameRecv = true;
}

