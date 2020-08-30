
/**
 *  @brief:  Implementation of a BME280 platform dependent [MBED] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-08-18
 */

#include "bme280-mbed.h"

//------------------------------------------------------------------------------
void bme280_mbed_init(bme280 *const dev, bme280_mbed* const mbed_dev)
{
    dev->platform_dev = mbed_dev;
    dev->platform_dev->i2c->frequency(400000);
    bme280_init(dev);
}

//------------------------------------------------------------------------------
void bme280_mbed_deinit(void)
{
    // Empty
}

//-----------------------------------------------------------------------------
bool bme280_write(bme280 *const dev, const uint8_t* buf, const size_t buf_size)
{
    bme280_mbed* const pd = (bme280_mbed*)dev->platform_dev;
    return pd->i2c->write((int)dev->i2c_addr, (char*)buf, buf_size) == 0;
}

//-----------------------------------------------------------------------------
bool bme280_read(bme280 *const dev, uint8_t* buf, const size_t buf_size)
{
    bme280_mbed* const pd = (bme280_mbed*)dev->platform_dev;
    return pd->i2c->read(dev->i2c_addr, (char*)buf, buf_size) == 0;
}

//-----------------------------------------------------------------------------
void bme280_delay_ms(uint32_t delay_ms)
{
    wait_ms(delay_ms);
}

//-----------------------------------------------------------------------------
