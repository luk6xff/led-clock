#ifndef __RADIO_H__
#define __RADIO_H__

#include "../SX1278/platform/arduino/sx1278-arduino.h"

/**
 * @brief RadiSensor global settings structure
 */
struct RadioSensorSettings
{
	uint32_t crit_vbatt_level;     //[mV]
	uint32_t update_data_interval; //[s] in seconds

    String toStr()
    {
        return String("crit_vbatt_level: " + String(crit_vbatt_level) + " update_data_interval:" + String(update_data_interval));
    }
};


/**
 * @brief Radio Msg frame status field description
 */
typedef enum
{
	MSG_NO_ERROR   = 1<<0,
	MSG_READ_ERROR = 1<<1,
	MSG_INIT_ERROR = 1<<2,
	MSG_BATT_LOW   = 1<<3,
} radio_msg_sensor_frame_status;

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


class Radio
{

public:
    explicit Radio(RadioSensorSettings& radioSensorCfg);

private:
    static uint16_t radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len);
    static void parse_incoming_msg_sensor(uint8_t *payload, uint16_t size);

    // Callbacks
    static void on_tx_done(void *args);
    static void on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
    static void on_tx_timeout(void *args);
    static void on_rx_timeout(void *args);
    static void on_rx_error(void *args);

private:
    RadioSensorSettings& cfg;
    SPIClass spi;
    RadioEvents_t events;
    sx1278 dev;
    sx1278_arduino arduino_dev;

    /**
     * @brief Clock global Msg frame
     */
    static radio_msg_clock_frame msgf;
};


#endif // __RADIO_H__