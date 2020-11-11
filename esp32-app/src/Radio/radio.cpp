#include "radio.h"
#include "hw_config.h"
#include "App/utils.h"
#include <functional>

//------------------------------------------------------------------------------
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
#define RX_TIMEOUT_VALUE                            8000      // in ms
#define MAX_PAYLOAD_LENGTH                          60        // bytes



//------------------------------------------------------------------------------
QueueHandle_t Radio::msgSensorDataQ = nullptr;

//------------------------------------------------------------------------------
Radio::Radio(RadioSensorSettings& radioSensorCfg)
    : cfg(radioSensorCfg)
    , spi(HSPI)
{
    // Set arduino_dev
    arduino_dev.spi_settings = SPISettings();
    arduino_dev.spi =   &spi;
    arduino_dev.mosi =  RADIO_MOSI_PIN;
    arduino_dev.miso =  RADIO_MISO_PIN;
    arduino_dev.sck =   RADIO_SCK_PIN;
    arduino_dev.nss =   RADIO_NSS_PIN;
    arduino_dev.reset = RADIO_RST_PIN;
    arduino_dev.dio0 =  RADIO_DI0_PIN;
    arduino_dev.dio1 =  -1; // Not used
    arduino_dev.dio2 =  -1;
    arduino_dev.dio3 =  -1;
    arduino_dev.dio4 =  -1;
    arduino_dev.dio5 =  -1;

    // Set dev
    dev.events = &events;
    //dev.events->tx_done = &Radio::on_tx_done;
    dev.events->rx_done = &Radio::on_rx_done;
    // dev.events->tx_timeout = &Radio::on_tx_timeout;
    // dev.events->rx_timeout = &Radio::on_rx_timeout;
    dev.events->rx_error = &Radio::on_rx_error;
    dev.settings.modem = MODEM_LORA;

    sx1278_arduino_init(&dev, &arduino_dev);
    sx1278_set_channel(&dev, RF_FREQUENCY);

    // Verify if SX1278 connected to the the board
    while (sx1278_read(&dev, REG_VERSION) == 0x00)
    {
        utils::dbg("SX1278 Radio could not be detected!");
        sx1278_delay_ms(1000);
    }
    utils::dbg("REG_VERSION: 0x%x", sx1278_read(&dev, REG_VERSION));

    sx1278_set_max_payload_length(&dev, MODEM_LORA, MAX_PAYLOAD_LENGTH);
    sx1278_set_tx_config(&dev, MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                          LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                          LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                          LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
                          LORA_IQ_INVERSION_ON, 4000);

    sx1278_set_rx_config(&dev, MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                          LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                          LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0,
                          LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
                          LORA_IQ_INVERSION_ON, true);

    // Update clock msg cfg structure
    msgf =
    {
        .hdr = {'L','U','6'},
        .status = MSG_NO_ERROR,
        .crit_vbatt_level     = cfg.crit_vbatt_level,     //[mV]
        .update_data_interval = cfg.update_data_interval, //[s]
    };

    msgSensorDataQ = xQueueCreate(4, sizeof(radio_msg_sensor_frame));
    if (!msgSensorDataQ)
    {
        utils::err("RADIO: msgSensorDataQ has not been created!.");
    }

    sx1278_set_rx(&dev, 0);
}

//-----------------------------------------------------------------------------
void Radio::sendResponseToSensor()
{
    msgf.checksum = radio_msg_frame_checksum((const uint8_t*)&msgf, (sizeof(radio_msg_clock_frame)-sizeof(msgf.checksum)));
    // Send result data
    sx1278_send(&dev, (uint8_t*)&msgf, sizeof(radio_msg_clock_frame));
    sx1278_set_rx(&dev, 0);
}

//------------------------------------------------------------------------------
uint16_t Radio::radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len)
{
    uint8_t i;
    uint16_t sum = 0;

    for (i = 0; i < data_len; i++)
    {
        sum = sum ^ data[i];
    }
    return sum;
}

//------------------------------------------------------------------------------
void Radio::parse_incoming_msg_sensor(uint8_t *payload, uint16_t size)
{
    const radio_msg_sensor_frame *mf = (const radio_msg_sensor_frame *)payload;
    const uint32_t checksum = radio_msg_frame_checksum((const uint8_t*)mf, (sizeof(radio_msg_sensor_frame)-sizeof(mf->checksum)));
    if (mf->checksum != checksum)
    {
        utils::dbg("INVALID CHECKSUM!, Incoming_Checksum: 0x%x, Computed_Checksum: 0x%x", mf->checksum, checksum);
        return;
    }

    if (~(mf->status & MSG_NO_ERROR))
    {
        utils::dbg("MSG_NO_ERROR");
        utils::dbg("VBATT:%d[mV], T:%3.1f[C], P:%3.1f[Pa], H:%3.1f[%%]", mf->vbatt, mf->temperature, mf->pressure, mf->humidity);
    }
	else if (mf->status & MSG_READ_ERROR)
    {
        utils::dbg("MSG_READ_ERROR");
    }
	else if (mf->status & MSG_INIT_ERROR)
    {
        utils::dbg("MSG_INIT_ERROR");
    }
	if (mf->status & MSG_BATT_LOW)
    {
        utils::dbg("MSG_BATT_LOW");
    }
}

//------------------------------------------------------------------------------
void Radio::on_tx_done(void *args)
{
    utils::dbg("> on_tx_done");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    if (msgSensorDataQ)
    {
        radio_msg_sensor_frame frame;
        if (size != sizeof(frame))
        {
            return;
        }

        memcpy(&frame, payload, sizeof(frame));
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(msgSensorDataQ, &frame, &xHigherPriorityTaskWoken);
    }
}

//-----------------------------------------------------------------------------
void Radio::on_tx_timeout(void *args)
{
    utils::dbg("> on_tx_timeout");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_timeout(void *args)
{
    utils::dbg("> on_rx_timeout");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_error(void *args)
{
    utils::dbg("> on_rx_error");
}