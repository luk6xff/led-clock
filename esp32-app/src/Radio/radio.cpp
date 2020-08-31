#include "radio.h"
#include "hw_config.h"
#include <functional>

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

/**
 * @brief Radio Msg frame status field description
 */
typedef enum
{
	MSG_NO_ERROR   = 1<<0,
	MSG_READ_ERROR = 1<<1,
	MSG_INIT_ERROR = 1<<2,
	MSG_BATT_LOW   = 1<<3,
} radio_msg_frame_status;

/**
 * @brief Radio Msg frame footprint, sent to the clock
 */
typedef struct
{
	uint8_t hdr[3];
	uint8_t status;
	float temperature;
	float pressure;
	float humidity;
	float altitude;
	uint32_t checksum;
} radio_msg_frame;

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
static void parse_incoming_msg(uint8_t *payload, uint16_t size)
{
    const radio_msg_frame *mf = (const radio_msg_frame *)payload;
    const uint32_t checksum = radio_msg_frame_checksum((const uint8_t*)mf, (sizeof(radio_msg_frame)-sizeof(mf->checksum)));
    if (mf->checksum != checksum)
    {
        Serial.printf("INVALID CHECKSUM!, Incoming_Checksum: 0x%x, Computed_Checksum: 0x%x\n\r", mf->checksum, checksum);
        return;
    }
    Serial.printf("T:%3.1f[C], P:%3.1f[Pa], H:%3.1f[%%], A:%3.1f[m]\n\r", mf->temperature, mf->pressure, mf->humidity, mf->altitude);
}

//------------------------------------------------------------------------------
// A nice way to register class member function as "C" callback
#include <functional>

template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)>
{
    template <typename... Args>
    static Ret callback(Args... args)
    {
        return func(args...);
    }
    static std::function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;

typedef void (*on_func_callback)(void);

template<typename T>
on_func_callback makeCCallback(void (T::*method)(), T* r)
{
    Callback<void()>::func = std::bind(method, r);
    void (*c_function_pointer)() = static_cast<decltype(c_function_pointer)>(Callback<void()>::callback);
    return c_function_pointer;
}

typedef void (*on_rx_done_func_callback)(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

template<typename T>
on_rx_done_func_callback makeCCallback2(void (T::*method)(uint8_t *, uint16_t, int16_t, int8_t), T* r)
{
    Callback<void(uint8_t*, uint16_t, int16_t, int8_t)>::func = std::bind(method, r, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
    void (*c_function_pointer)(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) = static_cast<decltype(c_function_pointer)>(Callback<void(uint8_t *, uint16_t, int16_t, int8_t)>::callback);
    return c_function_pointer;
}
RadioEvents_t events;
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
    dev.events = &events;
    dev.events->tx_done = &Radio::on_tx_done;
    dev.events->rx_done = &Radio::on_rx_done;
    dev.events->tx_timeout = &Radio::on_tx_timeout;
    dev.events->rx_timeout = &Radio::on_rx_timeout;
    dev.events->rx_error = &Radio::on_rx_error;
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
    Serial.printf("> on_tx_done\n\r");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_done(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    //sx1278_send(&dev, (uint8_t*)GatewayMsg, sizeof(GatewayMsg));
    //sx1278_set_sleep(&dev);
    Serial.printf("> on_rx_done\n\r");
    Serial.printf("> RssiValue: %d\n\r", rssi);
    Serial.printf("> SnrValue: %d\n\r", snr);
    Serial.printf("> PAYLOAD: %s\n\r", payload);
    parse_incoming_msg(payload, size);
}

//-----------------------------------------------------------------------------
void Radio::on_tx_timeout(void)
{
    Serial.printf("> on_tx_timeout\n\r");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_timeout(void)
{
    Serial.printf("> on_rx_timeout\n\r");
}

//-----------------------------------------------------------------------------
void Radio::on_rx_error(void)
{
    Serial.printf("> on_rx_error\n\r");
}