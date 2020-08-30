
/**
 *  @brief:  Implementation of a SX1278 platform dependent [STM32 CUBE HAL] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-01-05
 */

#include "sx1278-cube-hal.h"

//-----------------------------------------------------------------------------
void sx1278_cube_hal_init(sx1278* const dev, sx1278_cube_hal* const cube_dev)
{
    dev->platform_dev = cube_dev;

    sx1278_init(dev);
}

//-----------------------------------------------------------------------------
void sx1278_cube_hal_deinit(sx1278* const dev)
{
    // IO
    sx1278_io_deinit(dev);
}

//-----------------------------------------------------------------------------
void sx1278_io_init(sx1278* const dev)
{
    // Init SPI
    // SPI already configured in MX_SPIx_Init - generated by cube
    sx1278_delay_ms(10);
}

//-----------------------------------------------------------------------------
void sx1278_io_deinit(sx1278* const dev)
{
    // EMPTY
}


//-----------------------------------------------------------------------------
void sx1278_ioirq_init(sx1278* const dev)
{
    /**
     * @note dio0 HAL_GPIO_EXTI_Callback must be reimplemented in the main and shall
     *       contain a call to the proper sx1278 isr handler function
     */
}

//-----------------------------------------------------------------------------
void sx1278_reset(sx1278* const dev)
{
    sx1278_cube_hal* const pd = (sx1278_cube_hal*)dev->platform_dev;
    HAL_GPIO_WritePin(pd->reset_port, pd->reset_pin, GPIO_PIN_RESET);
    sx1278_delay_ms(10);
    HAL_GPIO_WritePin(pd->reset_port, pd->reset_pin, GPIO_PIN_SET);
    sx1278_delay_ms(10);
}

//-----------------------------------------------------------------------------
void sx1278_write_buffer(sx1278* const dev, uint8_t addr, const uint8_t *buffer, const uint8_t size)
{
    sx1278_cube_hal* const pd = (sx1278_cube_hal*)dev->platform_dev;

    uint8_t address = (addr | 0x80);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(pd->spi, &address, sizeof(addr), 100);
    HAL_SPI_Transmit(pd->spi, (uint8_t*)buffer, size, 1000);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_SET);
}

//-----------------------------------------------------------------------------
void sx1278_read_buffer(sx1278* const dev, uint8_t addr, uint8_t *buffer, uint8_t size)
{
    sx1278_cube_hal* const pd = (sx1278_cube_hal*)dev->platform_dev;

    uint8_t address = (addr & 0x7F);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(pd->spi, &address, sizeof(addr), 100);
    HAL_SPI_Receive(pd->spi, buffer, size, 1000);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_SET);
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
                // TODO Shall be based on https://github.com/Lora-net/LoRaMac-node/blob/master/src/system/timer.h
                //rx_timeout_timer.attach_us(mbed::callback(func), timeout_ms*1000);
            }
            else
            {
                //rx_timeout_timer.detach();
            }
            break;
        }
        case TXTimeoutTimer:
        {
            if (func)
            {
                //tx_timeout_timer.attach_us(mbed::callback(func), timeout_ms*1000);
            }
            else
            {
                //tx_timeout_timer.detach();
            }
            break;
        }
        case RXTimeoutSyncWordTimer:
        {
            if (func)
            {
                //rx_timeout_syncword.attach_us(mbed::callback(func), timeout_ms*1000);
            }
            else
            {
                //rx_timeout_syncword.detach();
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------
void sx1278_delay_ms(int delay_ms)
{
	uint32_t tickstart_ms = HAL_GetTick();
	while((HAL_GetTick()-tickstart_ms) < delay_ms);
}

//-----------------------------------------------------------------------------