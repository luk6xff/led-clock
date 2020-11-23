
/**
 *  @brief:  Implementation of a LORA platform dependent [STM32 CUBE HAL] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-11-22
 */

#include "lora-cube-hal.h"

//-----------------------------------------------------------------------------
void lora_cube_hal_init(lora *const dev, lora_cube_hal *const cube_dev)
{
    dev->platform_dev = cube_dev;

    lora_init(dev);
}

//-----------------------------------------------------------------------------
void lora_cube_hal_deinit(lora *const dev)
{
    // IO
    lora_io_deinit(dev);
}

//-----------------------------------------------------------------------------
void lora_io_init(lora *const dev)
{
    // Init SPI
    // SPI already configured in MX_SPIx_Init - generated by cube
    lora_delay_ms(10);
}

//-----------------------------------------------------------------------------
void lora_io_deinit(lora *const dev)
{
    // EMPTY
}


//-----------------------------------------------------------------------------
void lora_ioirq_init(lora *const dev)
{
    /**
     * @note dio0 HAL_GPIO_EXTI_Callback must be reimplemented in the main and shall
     *       contain a call to the proper lora isr handler function
     *       e.g:
     *		void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
     *		{
     *			if (GPIO_Pin == LORA_DIO0_Pin)
     *			{
     *				(lora_dev.dio_irq[0])(&lora_dev);
     *			}
     *		}
     */
}

//-----------------------------------------------------------------------------
void lora_reset(lora *const dev)
{
    lora_cube_hal *const pd = (lora_cube_hal*)dev->platform_dev;
    HAL_GPIO_WritePin(pd->reset_port, pd->reset_pin, GPIO_PIN_RESET);
    lora_delay_ms(10);
    HAL_GPIO_WritePin(pd->reset_port, pd->reset_pin, GPIO_PIN_SET);
    lora_delay_ms(10);
}

//-----------------------------------------------------------------------------
void lora_write_buffer(lora *const dev, uint8_t addr, const uint8_t *buffer, const uint8_t size)
{
    lora_cube_hal *const pd = (lora_cube_hal*)dev->platform_dev;

    uint8_t address = (addr | 0x80);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(pd->spi, &address, sizeof(addr), 100);
    HAL_SPI_Transmit(pd->spi, (uint8_t*)buffer, size, 1000);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_SET);
}

//-----------------------------------------------------------------------------
void lora_read_buffer(lora *const dev, uint8_t addr, uint8_t *buffer, uint8_t size)
{
    lora_cube_hal *const pd = (lora_cube_hal*)dev->platform_dev;

    uint8_t address = (addr & 0x7F);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(pd->spi, &address, sizeof(addr), 100);
    HAL_SPI_Receive(pd->spi, buffer, size, 1000);
    HAL_GPIO_WritePin(pd->nss_port, pd->nss_pin, GPIO_PIN_SET);
}

//-----------------------------------------------------------------------------
void lora_delay_ms(int delay_ms)
{
	uint32_t tickstart_ms = HAL_GetTick();
	while((HAL_GetTick()-tickstart_ms) < delay_ms);
}

//-----------------------------------------------------------------------------