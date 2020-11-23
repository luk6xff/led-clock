#include "radio.h"
#include "hw_config.h"
#include "App/utils.h"
#include <functional>

//------------------------------------------------------------------------------
// Radio LORA settings
// #define RF_FREQUENCY                                RF_FREQUENCY_434_0
// #define TX_OUTPUT_POWER                             20//14        // dBm
// #define LORA_BANDWIDTH                              LORA_BANDWIDTH_125kHz//LORA_BANDWIDTH_125kHz//LORA_BANDWIDTH_125kHz
// #define LORA_SPREADING_FACTOR                       LORA_SF12//LORA_SF8
// #define LORA_CODINGRATE                             LORA_ERROR_CODING_RATE_4_6
// #define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
// #define LORA_SYMBOL_TIMEOUT                         5         // Symbols
// #define LORA_FIX_LENGTH_PAYLOAD_ON                  false
// #define LORA_FHSS_ENABLED                           false
// #define LORA_NB_SYMB_HOP                            4
// #define LORA_IQ_INVERSION_ON                        false
// #define LORA_CRC_ENABLED                            true
// #define RX_TIMEOUT_VALUE                            8000      // in ms
// #define TX_TIMEOUT_VALUE                            8000      // in ms
// #define MAX_PAYLOAD_LENGTH                          60        // bytes
#define LORA_RADIO_LEDCLOCK_ID 0xAB
#define LORA_RADIO_SENSOR_ID 0xCD
#define LORA_RADIO_LEDCLOCK_SENSOR_MSG_ID 0x67


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

    // Set dev
    dev.frequency = 433E6;
    dev.on_receive = NULL;
    dev.on_tx_done = NULL;

    // dev.events = &events;
    // //dev.events->tx_done = &Radio::on_tx_done;
    // dev.events->rx_done = &Radio::on_rx_done;
    // // dev.events->tx_timeout = &Radio::on_tx_timeout;
    // // dev.events->rx_timeout = &Radio::on_rx_timeout;
    // dev.events->rx_error = &Radio::on_rx_error;
    // dev.settings.modem = MODEM_LORA;

    while (!lora_arduino_init(&dev, &arduino_dev))
    {
        utils::dbg("LORA Radio cannot be detected!, check your connections.");
        lora_delay_ms(1000);
    }
    lora_on_receive(&dev, &Radio::on_rx_done);

    // Update clock msg cfg structure
    msgf =
    {
        .hdr = {'L','U','6'},
        .status = MSG_NO_ERROR,
        .crit_vbatt_level     = cfg.crit_vbatt_level,     //[mV]
        .update_data_interval = cfg.update_data_interval, //[s]
    };

    msgSensorDataQ = xQueueCreate(4, sizeof(radio_msg_queue_data));
    if (!msgSensorDataQ)
    {
        utils::err("RADIO: msgSensorDataQ has not been created!.");
    }

    // SWitch into RX mode
    lora_receive(&dev, 0);
}

//-----------------------------------------------------------------------------
void Radio::sendResponseToSensor()
{
    msgf.checksum = radio_msg_frame_checksum((const uint8_t*)&msgf, (sizeof(radio_msg_clock_frame)-sizeof(msgf.checksum)));
    // Send result data
    radio_layer_msg_header hdr;
    hdr.receiver_id = LORA_RADIO_SENSOR_ID;
    hdr.sender_id = LORA_RADIO_LEDCLOCK_ID;
    hdr.msg_id = LORA_RADIO_LEDCLOCK_SENSOR_MSG_ID;
    hdr.payload_len = sizeof(radio_msg_clock_frame);

    lora_begin_packet(&dev, false);                     // start packet
    lora_write_data(&dev, (const uint8_t*)&hdr, sizeof(radio_layer_msg_header));
    lora_write_data(&dev, (const uint8_t*)&msgf, sizeof(radio_msg_clock_frame));
    lora_end_packet(&dev, false);                       // finish packet and send it

    // Switch to RX mode
    //lora_reset(&dev);
    lora_receive(&dev, 0);
    lora_ioirq_init(&dev);
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
radio_msg_sensor_frame_status Radio::parse_incoming_msg_sensor(uint8_t *payload, uint16_t size)
{
    const radio_msg_sensor_frame *mf = (const radio_msg_sensor_frame *)payload;
    const uint32_t checksum = radio_msg_frame_checksum((const uint8_t*)mf, (sizeof(radio_msg_sensor_frame)-sizeof(mf->checksum)));
    radio_msg_sensor_frame_status status = MSG_NO_ERROR;
    if (mf->checksum != checksum)
    {
        utils::err("INVALID CHECKSUM!, Incoming_Checksum: 0x%x, Computed_Checksum: 0x%x", mf->checksum, checksum);
        status = MSG_CHECKSUM_ERROR;
    }
    else
    {
        if (~(mf->status & MSG_NO_ERROR))
        {
            utils::dbg("MSG_NO_ERROR");
            utils::dbg("VBATT:%d[mV], T:%3.1f[C], P:%3.1f[Pa], H:%3.1f[%%]", mf->vbatt, mf->temperature, mf->pressure, mf->humidity);
        }
        else if (mf->status & MSG_READ_ERROR)
        {
            utils::err("MSG_READ_ERROR");
            status = MSG_READ_ERROR;
        }
        else if (mf->status & MSG_INIT_ERROR)
        {
            utils::err("MSG_INIT_ERROR");
            status = MSG_INIT_ERROR;
        }
        if (mf->status & MSG_BATT_LOW)
        {
            utils::err("MSG_BATT_LOW");
            status = MSG_BATT_LOW;
        }
    }

    return status;
}

//------------------------------------------------------------------------------
void Radio::on_tx_done(void *args)
{
    utils::dbg("> on_tx_done");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_done(void*ctx, int packetSize)
{
    lora *const dev = (lora*const) ctx;

    radio_layer_msg_header hdr;
    radio_msg_sensor_frame frame;
    uint8_t *p = (uint8_t*)&frame;
    size_t i = 0;

    if (packetSize == 0)
    {
        goto err;
    }

    // Read packet header bytes:
    hdr.receiver_id = lora_read(dev);          // recipient address
    hdr.sender_id = lora_read(dev);            // sender address
    hdr.msg_id = lora_read(dev);     // incoming msg ID
    hdr.payload_len = lora_read(dev);    // incoming msg length

    // If the recipient isn't this device or broadcast,
    if (hdr.receiver_id != LORA_RADIO_LEDCLOCK_ID && hdr.receiver_id != 0xFF)
    {
        goto err;
    }
    // Check payload length
    if (hdr.payload_len != sizeof(radio_msg_sensor_frame))
    {
        goto err;
    }

    // Read payload frame
    while (lora_available(dev) && i < hdr.payload_len)
    {
        *(p+i) = (uint8_t)lora_read(dev);
        i++;
    }

    if (msgSensorDataQ)
    {
        radio_msg_queue_data data;
        memcpy(&data.hdr, &hdr, sizeof(hdr));
        memcpy(&data.frame, &frame, sizeof(frame));
        data.rssi = lora_packet_rssi(dev);
        data.snr = lora_packet_snr(dev);
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(msgSensorDataQ, &data, &xHigherPriorityTaskWoken);
        return;
    }

err:
    // Enable interrupts and switch into rx mode
    lora_receive(dev, 0);
    lora_ioirq_init(dev);
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