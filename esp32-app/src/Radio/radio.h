#ifndef __RADIO_H__
#define __RADIO_H__

#include "radio_settings.h"
//#include "../SX1278/platform/arduino/sx1278-arduino.h"
#include "../LORA/platform/arduino/lora-arduino.h"

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


/**
 * @brief Radio msg queue data structure
 */
typedef struct
{
	radio_layer_msg_header hdr;
	radio_msg_sensor_frame frame;
	int rssi;
	float snr;
} radio_msg_queue_data;



class Radio final
{

public:
    explicit Radio(RadioSensorSettings& radioSensorCfg);
    void sendResponseToSensor();

    static radio_msg_sensor_frame_status parse_incoming_msg_sensor(uint8_t *payload, uint16_t size);

private:
    static uint16_t radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len);

    // Callbacks
    static void on_tx_done(void *args);
    //static void on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
    static void on_rx_done(void *ctx, int packetSize);
    static void on_tx_timeout(void *args);
    static void on_rx_timeout(void *args);
    static void on_rx_error(void *args);

public:
    /**
     * @brief Radio sensor global data queue
     */
    static QueueHandle_t msgSensorDataQ;

private:
    RadioSensorSettings& cfg;
    SPIClass spi;
    // RadioEvents_t events;
    // sx1278 dev;
    // sx1278_arduino arduino_dev;
    lora dev;
    lora_arduino arduino_dev;
    /**
     * @brief Clock global Msg frame
     */
    radio_msg_clock_frame msgf;
};


#endif // __RADIO_H__