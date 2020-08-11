/**
 *  @brief:  Implementation of a SX1278 platform dependent [MBED] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-08-07
 */

#include "sx1278-mbed.h"


/**
 * SPI Interface
 */
SPI* spi; // mosimiso, sclk
DigitalOut* nss;

/**
 * SX1278 Reset pin
 */
DigitalInOut* reset;

/**
 * SX1278 DIO pins
 */
InterruptIn* dio0;
InterruptIn* dio1;
InterruptIn* dio2;
InterruptIn* dio3;
InterruptIn* dio4;
DigitalIn* dio5;

/**
 * Tx and Rx timers
 */
Timeout tx_timeout_timer;
Timeout rx_timeout_timer;
Timeout rx_timeout_syncword;


//-----------------------------------------------------------------------------
void sx1278_mbed_init(sx1278* const dev, sx1278_mbed* const mbed_dev)

{
    dev->platform_dev = mbed_dev;
    sx1278_init(dev);
}

//-----------------------------------------------------------------------------
void sx1278_mbed_deinit(sx1278* const dev)
{
    // IO
    sx1278_io_deinit(dev);
    // Timers
    tx_timeout_timer.detach();
    rx_timeout_timer.detach();
    rx_timeout_syncword.detach();
}

//-----------------------------------------------------------------------------
void sx1278_io_init(sx1278* const dev)
{
    sx1278_mbed* const pd = (sx1278_mbed*)dev->platform_dev;
    // Init SPI
    *(pd->nss) = 1;
    pd->spi->format(8,0);
    uint32_t frequencyToSet = 8000000;
    pd->spi->frequency(frequencyToSet);
    sx1278_delay_ms(100);
}

//-----------------------------------------------------------------------------
void sx1278_io_deinit(sx1278* const dev)
{
    // EMPTY
}


//-----------------------------------------------------------------------------
void sx1278_ioirq_init(sx1278* const dev)
{
    sx1278_mbed* const pd = (sx1278_mbed*)dev->platform_dev;
    // dio0 - dio4
    pd->dio0->rise(mbed::callback(dev->dio_irq[0], (void*)dev));
    pd->dio1->rise(mbed::callback(dev->dio_irq[1], (void*)dev));
    pd->dio2->rise(mbed::callback(dev->dio_irq[2], (void*)dev));
    pd->dio3->rise(mbed::callback(dev->dio_irq[3], (void*)dev));
    pd->dio4->rise(mbed::callback(dev->dio_irq[4], (void*)dev));
}

//-----------------------------------------------------------------------------
void sx1278_reset(sx1278* const dev)
{
    sx1278_mbed* const pd = (sx1278_mbed*)dev->platform_dev;
    pd->reset->output();
    *(pd->reset) = 0;
    sx1278_delay_ms(1);
    pd->reset->input();
    sx1278_delay_ms(6);
}

//-----------------------------------------------------------------------------
void sx1278_write_buffer(sx1278* const dev, uint8_t addr, const uint8_t *buffer, const uint8_t size)
{
    sx1278_mbed* const pd = (sx1278_mbed*)dev->platform_dev;
    uint8_t i;

    *(pd->nss) = 0;
    pd->spi->write(addr | 0x80);
    for(i = 0; i < size; i++)
    {
        pd->spi->write(buffer[i]);
    }
    *(pd->nss) = 1;
}

//-----------------------------------------------------------------------------
void sx1278_read_buffer(sx1278* const dev, uint8_t addr, uint8_t *buffer, uint8_t size)
{
    sx1278_mbed* const pd = (sx1278_mbed*)dev->platform_dev;
    uint8_t i;

    *(pd->nss) = 0;
    pd->spi->write(addr & 0x7F);
    for(i = 0; i < size; i++)
    {
        buffer[i] = pd->spi->write(0);
    }
    *(pd->nss) = 1;
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
                rx_timeout_timer.attach_us(mbed::callback(func), timeout_ms*1000);
            }
            else
            {
                rx_timeout_timer.detach();
            }
            break;
        }
        case TXTimeoutTimer:
        {
            if (func)
            {
                tx_timeout_timer.attach_us(mbed::callback(func), timeout_ms*1000);
            }
            else
            {
                tx_timeout_timer.detach();
            }
            break;
        }
        case RXTimeoutSyncWordTimer:
        {
            if (func)
            {
                rx_timeout_syncword.attach_us(mbed::callback(func), timeout_ms*1000);
            }
            else
            {
                rx_timeout_syncword.detach();
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------
void sx1278_delay_ms(int ms)
{
    wait_us(ms*1000);
}

//-----------------------------------------------------------------------------
