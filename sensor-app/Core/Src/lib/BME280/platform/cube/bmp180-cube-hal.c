
/**
 *  @brief:  Implementation of a BME280 platform dependent [STM32 CUBE HAL] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-08-18
 */

#include "bme280-cube-hal.h"

//------------------------------------------------------------------------------
void bme280_cube_hal_init(bme280* const dev, bme280_cube_hal* const cube_hal_dev)
{
    dev->platform_dev = cube_hal_dev;
    bme280_init(dev);
}

//------------------------------------------------------------------------------
void bme280_cube_hal_deinit(void)
{
    // Empty
}

//-----------------------------------------------------------------------------
bool bme280_write(bme280* const dev, const uint8_t* buf, const size_t buf_size)
{
    bme280_cube_hal* const pd = (bme280_cube_hal*)dev->platform_dev;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(pd->i2c, dev->i2c_addr, buf, buf_size, 1000);
    if (status != HAL_OK)
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
bool bme280_read(bme280* const dev, uint8_t* buf, const size_t buf_size)
{
    bme280_cube_hal* const pd = (bme280_cube_hal*)dev->platform_dev;
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(pd->i2c, dev->i2c_addr, buf, buf_size, 1000);
    if (status != HAL_OK)
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
void bme280_delay_ms(uint32_t delay_ms)
{
    uint32_t tickstart_ms = HAL_GetTick();
    while ((HAL_GetTick()-tickstart_ms) < delay_ms);
}

//-----------------------------------------------------------------------------
