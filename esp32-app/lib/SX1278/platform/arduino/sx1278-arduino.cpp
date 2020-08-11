/**
 *  @brief:  Implementation of a SX1278 platform dependent [ARDUINO] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-08-07
 */

#include "sx1278-arduino.h"
#include <SPI.h>
#include "TimeOut.h"

/**
 * Tx and Rx timers
 */
TimeOut tx_timeout_timer;
TimeOut rx_timeout_timer;
TimeOut rx_timeout_syncword;


//-----------------------------------------------------------------------------
void sx1278_arduino_init(sx1278* const dev, sx1278_arduino* const arduino_dev)

{
    dev->platform_dev = arduino_dev;
    sx1278_init(dev);
}

//-----------------------------------------------------------------------------
void sx1278_arduino_deinit(sx1278* const dev)
{
    // IO
    sx1278_io_deinit(dev);
    // Timers
    tx_timeout_timer.cancel();
    rx_timeout_timer.cancel();
    rx_timeout_syncword.cancel();
}

//-----------------------------------------------------------------------------
void sx1278_io_init(sx1278* const dev)
{
    // Get arduino platform data
    sx1278_arduino* const pd = (sx1278_arduino*)dev->platform_dev;
    // Init SPI
    pd->spi_settings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
    // Setup pins
    pinMode(pd->nss, OUTPUT);
    // Set SS high
    digitalWrite(pd->nss, HIGH);
    // Start SPI
    pd->spi->begin(pd->sck, pd->miso, pd->mosi, pd->nss);
    sx1278_delay_ms(10);
}

//-----------------------------------------------------------------------------
void sx1278_io_deinit(sx1278* const dev)
{
    // Empty
}


//-----------------------------------------------------------------------------
void sx1278_ioirq_init(sx1278* const dev)
{

    sx1278_arduino* const pd = (sx1278_arduino*)dev->platform_dev;
    if (dev->dio_irq == NULL)
    {
        // TODO print error here
        while(1);
        return;
    }
    // dio0
    int interrupt_num = digitalPinToInterrupt(pd->dio0);
    if (interrupt_num == NOT_AN_INTERRUPT)
    {
        // TODO print error here
        while(1);
        return;
    }
    // Only dio0 used
    pinMode(pd->dio0, INPUT);
    attachInterruptArg(digitalPinToInterrupt(pd->dio0), dev->dio_irq[0], dev, RISING);
}

//-----------------------------------------------------------------------------
void sx1278_reset(sx1278* const dev)
{
    sx1278_arduino* const pd = (sx1278_arduino*)dev->platform_dev;

    pinMode(pd->reset, OUTPUT);
    digitalWrite(pd->reset, LOW);
    sx1278_delay_ms(1);
    pinMode(pd->reset, INPUT);
}

//-----------------------------------------------------------------------------
void sx1278_write_buffer(sx1278* const dev, uint8_t addr, const uint8_t *buffer, const uint8_t size)
{
    sx1278_arduino* const pd = (sx1278_arduino*)dev->platform_dev;

    digitalWrite(pd->nss, LOW);
    pd->spi->beginTransaction(pd->spi_settings);
    pd->spi->transfer(addr | 0x80);
    pd->spi->writeBytes(buffer, size);
    pd->spi->endTransaction();
    digitalWrite(pd->nss, HIGH);
}

//-----------------------------------------------------------------------------
void sx1278_read_buffer(sx1278* const dev, uint8_t addr, uint8_t *buffer, uint8_t size)
{
    sx1278_arduino* const pd = (sx1278_arduino*)dev->platform_dev;

    digitalWrite(pd->nss, LOW);
    pd->spi->beginTransaction(pd->spi_settings);
    pd->spi->transfer(addr & 0x7F);
    for(int i = 0; i < size; i++)
    {
        buffer[i] = pd->spi->transfer(0);
    }
    pd->spi->endTransaction();
    digitalWrite(pd->nss, HIGH);
}



//-----------------------------------------------------------------------------
void sx1278_set_timeout(sx1278* const dev, TimeoutTimer_t timer, TimeoutFuncPtr func, int timeout_ms)
{
    switch(timer)
    {
	    case RXTimeoutTimer:
        {
            if (func)
            {
                rx_timeout_timer.timeOut(timeout_ms, func);
            }
            else
            {
                rx_timeout_timer.cancel();
            }
            break;
        }
        case TXTimeoutTimer:
        {
            if (func)
            {
                tx_timeout_timer.timeOut(timeout_ms, func);
            }
            else
            {
                tx_timeout_timer.cancel();
            }
            break;
        }
        case RXTimeoutSyncWordTimer:
        {
            if (func)
            {
                rx_timeout_syncword.timeOut(timeout_ms, func);
            }
            else
            {
                rx_timeout_syncword.cancel();
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------
void sx1278_delay_ms(int ms)
{
    delay(ms);
}

//-----------------------------------------------------------------------------
