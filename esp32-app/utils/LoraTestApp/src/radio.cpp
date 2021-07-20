/*
 * radio.c
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */

#include "radio.h"
#include "hw-config.h"
#include "../../../LORA/platform/arduino/lora-arduino.h"


//-----------------------------------------------------------------------------
// Radio LORA settings
#define LORA_RADIO_LEDCLOCK_ID 0xAB
#define LORA_RADIO_SENSOR_ID 0xCD
#define LORA_RADIO_LEDCLOCK_SENSOR_MSG_ID 0x67

static void on_rx_done(void *args, int packet_size);
static void parse_incoming_msg_clock(uint8_t *payload, uint16_t size);
static uint16_t radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len);

//-----------------------------------------------------------------------------
// LORA SX1278 RADIO
static lora_arduino arduino_dev;

static lora dev;

static SPIClass spi;

static volatile uint8_t radio_data_received = false;

static radio_msg_clock_frame last_clock_response;

//-----------------------------------------------------------------------------
void radio_init()
{
    // Set arduino_dev
    arduino_dev.spi_settings = SPISettings();
    arduino_dev.spi =   &spi;
    arduino_dev.mosi =  RADIO_MOSI_PIN;
    arduino_dev.miso =  RADIO_MISO_PIN;
    arduino_dev.sck =   RADIO_SCK_PIN;
    arduino_dev.nss =   RADIO_NSS_PIN;
    arduino_dev.reset = RADIO_RST_PIN;
    arduino_dev.dio0 =  RADIO_DIO0_PIN;

    // Set dev
    dev.frequency = 433E6;
    dev.on_receive = NULL;
    dev.on_tx_done = NULL;

	while (!lora_arduino_init(&dev, &arduino_dev))
	{
		Serial.printf("R_RDN");
		lora_delay_ms(1000);
	}
	lora_on_receive(&dev, on_rx_done);
	// Switch sleep idle mode
	lora_sleep(&dev);


    // Not used in production
    last_clock_response.update_data_interval = 30; //s
}

//-----------------------------------------------------------------------------
void radio_send(radio_msg_sensor_frame *msgf)
{
	static uint8_t msg_cnt = 0;
	if (!msgf)
	{
		return;
	}

	msgf->checksum = radio_msg_frame_checksum((const uint8_t*)msgf, (sizeof(radio_msg_sensor_frame)-sizeof(msgf->checksum)));
	// Send result data
    radio_layer_msg_header hdr;
    hdr.receiver_id = LORA_RADIO_LEDCLOCK_ID;
    hdr.sender_id = LORA_RADIO_SENSOR_ID;
    hdr.msg_id = msg_cnt++;
    hdr.payload_len = sizeof(radio_msg_sensor_frame);
    lora_begin_packet(&dev, false);                     // start packet
    lora_write_data(&dev, (const uint8_t*)&hdr, sizeof(radio_layer_msg_header));
    lora_write_data(&dev, (const uint8_t*)msgf, sizeof(radio_msg_sensor_frame));
    lora_end_packet(&dev, false);                       // finish packet and send it
	Serial.printf("R_DTS");
    // Switch to RX mode
    lora_receive(&dev, 0);
}

//-----------------------------------------------------------------------------
void radio_sleep()
{
	lora_sleep(&dev);
}

//-----------------------------------------------------------------------------
void radio_restart()
{
    // Deinitilaize radio
    lora_arduino_deinit(&dev);
    lora_delay_ms(100);

    // Initialize it back
    const uint8_t attempts_num = 5;
    uint8_t attempts = 0;
    while (!lora_arduino_init(&dev, &arduino_dev) && (attempts++ < attempts_num))
    {
        Serial.printf("LORA Radio cannot be detected!, check your connections.");
        lora_delay_ms(2000);
    }

    lora_on_receive(&dev, on_rx_done);
    // Switch into RX mode
    lora_receive(&dev, 0);
}

//-----------------------------------------------------------------------------
uint8_t radio_was_data_received()
{
	return radio_data_received;
}

//-----------------------------------------------------------------------------
void radio_data_received_clear()
{
	radio_data_received = false;
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

//-----------------------------------------------------------------------------
static void on_rx_done(void *ctx, int packet_size)
{
	Serial.printf("R_ORD");
	lora *const dev = (lora*const) ctx;

	radio_layer_msg_header hdr;
	radio_msg_sensor_frame frame;
	uint8_t *p = (uint8_t*)&frame;
	size_t i = 0;

	if (packet_size == 0)
	{
		goto err;
	}

	// Read packet header bytes:
	hdr.receiver_id = lora_read(dev);		// recipient address
	hdr.sender_id = lora_read(dev);			// sender address
	hdr.msg_id = lora_read(dev);			// incoming msg ID
	hdr.payload_len = lora_read(dev);		// incoming msg length

	// If the recipient isn't this device or broadcast,
	if (hdr.receiver_id != LORA_RADIO_SENSOR_ID && hdr.receiver_id != 0xFF)
	{
		goto err;
	}
	// Check payload length
	if (hdr.payload_len != sizeof(radio_msg_clock_frame))
	{
		goto err;
	}

	// Read payload frame
	while (lora_available(dev) && i < hdr.payload_len)
	{
		*(p+i) = (uint8_t)lora_read(dev);
		i++;
	}

	parse_incoming_msg_clock(p, hdr.payload_len);
	// Set global flag
	radio_data_received = true;
err:
	radio_sleep();
}

//------------------------------------------------------------------------------
static void parse_incoming_msg_clock(uint8_t *payload, uint16_t size)
{
    const radio_msg_clock_frame *mf = (const radio_msg_clock_frame *)payload;
    const uint32_t checksum = radio_msg_frame_checksum((const uint8_t*)mf, (sizeof(radio_msg_clock_frame)-sizeof(mf->checksum)));
    if (mf->checksum != checksum)
    {
        Serial.printf("R_ICH");
        return;
    }

    if ((mf->status & MSG_NO_ERROR))
    {
        Serial.printf("R_MRECV: update_data_interval:%d,  crit_vbatt_level:%3.2f", mf->update_data_interval, mf->crit_vbatt_level);
        memcpy(&last_clock_response, mf, sizeof(last_clock_response));
    }
}

//------------------------------------------------------------------------------
const radio_msg_clock_frame* radio_get_last_clock_response()
{
    return &last_clock_response;
}


//------------------------------------------------------------------------------
