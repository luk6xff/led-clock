/**
 *  @brief:  LoRa library for SEMTECH SX127x devices
 *  @author: luk6xff based on the code: https://github.com/sandeepmistry/arduino-LoRa
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-11-22
 */

#ifndef __LORA_H__
#define __LORA_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>


#define LORA_MAX_PKT_LEN           255

/**
 * @brief Hardware IO IRQ callback function definition
 */
typedef void (*DioIrqHandler)(void*);

/**
 * @brief LoRa dev object
 */
typedef struct
{
    DioIrqHandler dio_irq;               // Callbacks for pins from dio0 to dio5
    uint8_t rx_tx_buffer[LORA_MAX_PKT_LEN];   // Data reception buffer
    long frequency;
    int packet_index;
    int implicit_header_mode;
    void (*on_receive)(void*, int);
    void (*on_tx_done)(void*);
    void *platform_dev;                     // Platform dependent data
} lora;


bool lora_init(lora *const dev);
void lora_end(lora *const dev);

int lora_beginPacket(lora *const dev, int implicitHeader);
int lora_endPacket(lora *const dev, bool async);

int lora_parsePacket(lora *const dev, int size);
int lora_packetRssi(lora *const dev);
float lora_packetSnr(lora *const dev);
long lora_packetFrequencyError(lora *const dev);

int lora_rssi();

// from Print
uint32_t lora_write(lora *const dev, uint8_t byte);
uint32_t lora_write_data(lora *const dev, const uint8_t *buffer, uint32_t size);

// from Stream
int lora_available(lora *const dev);
int lora_read(lora *const dev);
int lora_peek(lora *const dev);
void lora_flush(lora *const dev);

void lora_onReceive(lora *const dev, void(*callback)(void*, int));
void lora_onTxDone(lora *const dev, void(*callback)(void*));

void lora_receive(lora *const dev, int size);

void lora_idle(lora *const dev);
void lora_sleep(lora *const dev);

void lora_setTxPower(lora *const dev, int level, int outputPin);
void lora_setFrequency(lora *const dev, long frequency);
void lora_setSpreadingFactor(lora *const dev, int sf);
void lora_setSignalBandwidth(lora *const dev, long sbw);
void lora_setCodingRate4(lora *const dev, int denominator);
void lora_setPreambleLength(lora *const dev, long length);
void lora_setSyncWord(lora *const dev, int sw);
void lora_enableCrc(lora *const dev);
void lora_disableCrc(lora *const dev);
void lora_enableInvertIQ(lora *const dev);
void lora_disableInvertIQ(lora *const dev);

void lora_setOCP(lora *const dev, uint8_t mA); // Over Current Protection control

void lora_setGain(lora *const dev, uint8_t gain); // Set LNA gain

uint8_t lora_random(lora *const dev);

void lora_dumpRegisters(lora *const dev);

void lora_explicitHeaderMode(lora *const dev);
void lora_implicitHeaderMode(lora *const dev);

void lora_handleDio0Rise(lora *const dev);
bool lora_isTransmitting(lora *const dev);

int lora_getSpreadingFactor(lora *const dev);
long lora_getSignalBandwidth(lora *const dev);

void lora_setLdoFlag(lora *const dev);

/**
 * @brief Writes the radio register at the specified address
 *
 * @param[in]: addr Register address
 * @param[in]: data New register value
 */
void lora_write_reg(lora *const dev, uint8_t addr, uint8_t data);

/**
 * @brief Reads the radio register at the specified address
 *
 * @param[in]: addr Register address
 * @retval data Register value
 */
uint8_t lora_read_reg(lora *const dev, uint8_t addr);




//-------------------------------------------------------------------------
//  Board/Platform relative (HW dependent) functions,
//  Must be implemented in folder platform.
//-------------------------------------------------------------------------

/**
 * @brief Initializes the radio I/Os pins interface
 */
extern void lora_io_init(lora *const dev);

/**
 * @brief De-initializes the radio I/Os pins interface.
 *
 * @note Useful when going in MCU lowpower modes
 */
extern void lora_io_deinit(lora *const dev);

/**
 * @brief Initializes DIO IRQ handlers
 *
 * @param[in] irq_handlers Array containing the IRQ callback functions
 */
extern void lora_ioirq_init(lora *const dev);

/**
 * @brief Resets the LORA device
 */
extern void lora_reset(lora *const dev);

/**
 * @brief Writes multiple radio registers starting at address
 *
 * @param[in] addr   First Radio register address
 * @param[in] buffer Buffer containing the new register's values
 * @param[in] size   Number of registers to be written
 */
extern void lora_write_buffer(lora *const dev, uint8_t addr, const uint8_t *buffer, const uint8_t size);

/**
 * @brief Reads multiple radio registers starting at address
 *
 * @param[in] addr First Radio register address
 * @param[out] buffer Buffer where to copy the registers data
 * @param[in] size Number of registers to be read
 */
extern void lora_read_buffer(lora *const dev, uint8_t addr, uint8_t *buffer, uint8_t size);

/**
 * @brief A simple ms sleep
 */
extern void lora_delay_ms(int ms);

/**
 * @brief Print all the registers
 */
extern void lora_dumpRegisters(lora* const dev);


#ifdef __cplusplus
}
#endif

#endif // __LORA_H__
