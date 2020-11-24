/*
 * radio.c
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */

#include "radio.h"
#include "settings.h"
#include <lora-cube-hal.h>
#include <main.h>


//-----------------------------------------------------------------------------
extern SPI_HandleTypeDef hspi1;
extern char dbg_buf[32];
extern void dbg(const char* msg);

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
static lora_cube_hal cube_hal_dev =
{
	.spi = &hspi1,
	.nss_port = SX1278_NSS_GPIO_Port,
	.nss_pin = SX1278_NSS_Pin,
	.reset_port = SX1278_RESET_GPIO_Port,
	.reset_pin = SX1278_RESET_Pin,
};

static lora dev;

//-----------------------------------------------------------------------------
void radio_init()
{
	// Set dev
	dev.frequency = 433E6;
	dev.on_receive = NULL;
	dev.on_tx_done = NULL;

	while (!lora_cube_hal_init(&dev, &cube_hal_dev))
	{
		dbg("R_RDN");
		lora_delay_ms(1000);
	}
	lora_on_receive(&dev, on_rx_done);
	// Switch into idle mode
	lora_idle(&dev);
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
	dbg("R_DTS");
    // Switch to RX mode
    lora_receive(&dev, 0);
}

//-----------------------------------------------------------------------------
void radio_sleep()
{
	lora_sleep(&dev);
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == SX1278_DIO0_Pin)
	{
		(dev.dio_irq)(&dev);
	}
}

//-----------------------------------------------------------------------------
static void on_rx_done(void *ctx, int packet_size)
{
	dbg("R_ORD");
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
	hdr.receiver_id = lora_read(dev);          // recipient address
	hdr.sender_id = lora_read(dev);            // sender address
	hdr.msg_id = lora_read(dev);     // incoming msg ID
	hdr.payload_len = lora_read(dev);    // incoming msg length

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
        dbg("R_ICH");
        return;
    }

    if (~(mf->status & MSG_NO_ERROR))
    {
        dbg("R_MRECV");
        // Check and store setting if needed
        bool settings_update = false;
        if (mf->update_data_interval != app_settings_get_current()->update_interval)
        {
            app_settings_get_current()->update_interval = mf->update_data_interval;
            sprintf(dbg_buf, "R_UT:%d", mf->update_data_interval);
            dbg(dbg_buf);
            settings_update = true;
        }
        if (mf->crit_vbatt_level != app_settings_get_current()->critical_battery_level)
        {
            app_settings_get_current()->critical_battery_level = mf->crit_vbatt_level;
            sprintf(dbg_buf, "R_CB:%d", mf->crit_vbatt_level);
            dbg(dbg_buf);
            settings_update = true;
        }

        if (settings_update)
        {
        	if (!app_settings_store(app_settings_get_current()))
        	{
                dbg("R_SUF");
        	}
        }
    }
}

