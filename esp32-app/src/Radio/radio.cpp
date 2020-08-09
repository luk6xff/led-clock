#include "radio.h"
#include "hw_config.h"


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

const char GatewayMsg[] = "Hello FROM LORA GATEWAY!";
const char ClientMsg[] = "Hello FROM LORA CLIENT!";


//------------------------------------------------------------------------------
Radio::Radio()
    : spi(HSPI)
{
    // Set arduino_dev
    arduino_dev.spi_settings = SPISettings();
    arduino_dev.spi =   &spi;
    arduino_dev.mosi =  GPIO_NUM_13;
    arduino_dev.miso =  GPIO_NUM_12;
    arduino_dev.sck =   GPIO_NUM_14;
    arduino_dev.nss =   GPIO_NUM_15;
    arduino_dev.reset = GPIO_NUM_27;
    arduino_dev.dio0 =  GPIO_NUM_26;
    arduino_dev.dio1 =  -1; // Not used
    arduino_dev.dio2 =  -1;
    arduino_dev.dio3 =  -1;
    arduino_dev.dio4 =  -1;
    arduino_dev.dio5 =  -1;

    // Set dev
    dev.events->tx_done = on_tx_done;
    // dev.events->rx_done = on_rx_done;
    // dev.events->tx_timeout = on_tx_timeout;
    // dev.events->rx_timeout = on_rx_timeout;
    // dev.events->rx_error = on_rx_error;
    sx1278_arduino_init(&dev, &arduino_dev);
    sx1278_set_channel(&dev, RF_FREQUENCY);

    // Verify if SX1278 connected to the the board
    while (sx1278_read(&dev, REG_VERSION) == 0x00)
    {
        Serial.printf("SX1278 Radio could not be detected!\n\r");
        sx1278_delay_ms(1000);
    }
    Serial.printf("REG_VERSION: 0x%x", sx1278_read(&dev, REG_VERSION));

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

    sx1278_set_rx(&dev, RX_TIMEOUT_VALUE);
}

//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Radio::on_tx_done(void)
{
    sx1278_set_sleep(&dev);
    Serial.printf("> on_tx_done\n\r");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_done(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    sx1278_send(&dev, (uint8_t*)GatewayMsg, sizeof(GatewayMsg));
    sx1278_set_sleep(&dev);
    Serial.printf("> RssiValue: %d\n\r", rssi);
    Serial.printf("> SnrValue: %d\n\r", snr);
    Serial.printf("> PAYLOAD: %s\n\r", payload);
    Serial.printf("> Onrx_done\n\r");
}

//-----------------------------------------------------------------------------
void Radio::on_tx_timeout(void)
{
    sx1278_set_sleep(&dev);
    Serial.printf("> Ontx_timeout\n\r");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_timeout(void)
{
    sx1278_set_sleep(&dev);
    Serial.printf("> Onrx_timeout\n\r");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_error(void)
{
    sx1278_set_sleep(&dev);
    Serial.printf("> Onrx_error\n\r");
}
