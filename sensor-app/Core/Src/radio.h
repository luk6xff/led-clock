/*
 * radio.h
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */

#ifndef RADIO_H_
#define RADIO_H_

#include <stdint.h>

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


void radio_init();

void radio_send(radio_msg_sensor_frame *msgf);

void radio_sleep();

#endif /* RADIO_H_ */
