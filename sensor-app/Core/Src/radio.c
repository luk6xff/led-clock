/*
 * radio.c
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */

#include "radio.h"
#include "settings.h"
#include <sx1278-cube-hal.h>
#include <main.h>


//-----------------------------------------------------------------------------
extern SPI_HandleTypeDef hspi1;
extern char dbg_buf[32];
extern void dbg(const char* msg);

//-----------------------------------------------------------------------------
// Radio LORA settings
#define RF_FREQUENCY                                RF_FREQUENCY_434_0
#define TX_OUTPUT_POWER                             14        // dBm
#define LORA_BANDWIDTH                              LORA_BANDWIDTH_125kHz
#define LORA_SPREADING_FACTOR                       LORA_SF8
#define LORA_CODINGRATE                             LORA_ERROR_CODING_RATE_4_5
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_FHSS_ENABLED                           false
#define LORA_NB_SYMB_HOP                            4
#define LORA_IQ_INVERSION_ON                        false
#define LORA_CRC_ENABLED                            true
#define RX_TIMEOUT_VALUE                            5000      // in ms
#define MAX_PAYLOAD_LENGTH                          60        // bytes

//-----------------------------------------------------------------------------
static void on_tx_done(void *args);
static void on_tx_timeout(void *args);
static void on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
static void parse_incoming_msg_clock(uint8_t *payload, uint16_t size);
static uint16_t radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len);

//-----------------------------------------------------------------------------
// LORA SX1278 RADIO
static sx1278_cube_hal radio_cube_hal_dev =
{
	.spi = &hspi1,
	.nss_port = SX1278_NSS_GPIO_Port,
	.nss_pin = SX1278_NSS_Pin,
	.reset_port = SX1278_RESET_GPIO_Port,
	.reset_pin = SX1278_RESET_Pin,
};

static RadioEvents_t radio_events;

static sx1278 radio_dev;

//-----------------------------------------------------------------------------
void radio_init()
{
	// Set radio_dev
	radio_dev.events = &radio_events;
	radio_dev.events->tx_done = NULL;
	radio_dev.events->rx_done = on_rx_done;
	radio_dev.events->tx_timeout = NULL;
	radio_dev.events->rx_timeout = NULL;
	radio_dev.events->rx_error = NULL;
	radio_dev.settings.modem = MODEM_LORA;
	sx1278_cube_hal_init(&radio_dev, &radio_cube_hal_dev);
	sx1278_set_channel(&radio_dev, RF_FREQUENCY);

	// Verify if SX1278 connected to the the board
	while (sx1278_read(&radio_dev, REG_VERSION) != 0x12)
	{
	    dbg("RDN");
	    sx1278_delay_ms(1000);
	}

	sx1278_set_max_payload_length(&radio_dev, MODEM_LORA, MAX_PAYLOAD_LENGTH);
	sx1278_set_tx_config(&radio_dev, MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
	                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
	                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
	                      LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
	                      LORA_IQ_INVERSION_ON, 4000);

    sx1278_set_rx_config(&radio_dev, MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                          LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                          LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0,
                          LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
                          LORA_IQ_INVERSION_ON, true);
}

//-----------------------------------------------------------------------------
void radio_send(radio_msg_sensor_frame *msgf)
{
	if (!msgf)
	{
		return;
	}

	msgf->checksum = radio_msg_frame_checksum((const uint8_t*)msgf, (sizeof(radio_msg_sensor_frame)-sizeof(msgf->checksum)));
	// Send result data
	sx1278_send(&radio_dev, (uint8_t*)msgf, sizeof(radio_msg_sensor_frame));
	dbg("DTS");
	sx1278_set_rx(&radio_dev, 0);
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
		(radio_dev.dio_irq[0])(&radio_dev);
	}
}

//-----------------------------------------------------------------------------
static void on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
	dbg("ORD");
	parse_incoming_msg_clock(payload, size);
}

//------------------------------------------------------------------------------
static void parse_incoming_msg_clock(uint8_t *payload, uint16_t size)
{
    const radio_msg_clock_frame *mf = (const radio_msg_clock_frame *)payload;
    const uint32_t checksum = radio_msg_frame_checksum((const uint8_t*)mf, (sizeof(radio_msg_clock_frame)-sizeof(mf->checksum)));
    if (mf->checksum != checksum)
    {
        dbg("ICHK");
        return;
    }

    if (~(mf->status & MSG_NO_ERROR))
    {
        dbg("MRCV");
        // Check and store setting if needed
        bool settings_update = false;
        if (mf->update_data_interval != app_settings_get_current()->update_interval)
        {
            app_settings_get_current()->update_interval = mf->update_data_interval;
            sprintf(dbg_buf, "UT:%d", mf->update_data_interval);
            dbg(dbg_buf);
            settings_update = true;
        }
        if (mf->crit_vbatt_level != app_settings_get_current()->critical_battery_level)
        {
            app_settings_get_current()->critical_battery_level = mf->crit_vbatt_level;
            sprintf(dbg_buf, "CB:%d", mf->crit_vbatt_level);
            dbg(dbg_buf);
            settings_update = true;
        }

        if (settings_update)
        {
        	if (!app_settings_store(app_settings_get_current()))
        	{
                dbg("SUF");
        	}
        }
    }
}

