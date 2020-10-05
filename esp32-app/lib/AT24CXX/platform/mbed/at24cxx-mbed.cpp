#include "at24cxx-mbed.h"




//-----------------------------------------------------------------------------
void at24cxx_mbed_init(at24cxx *const dev, at24cxx_mbed *const mbed_dev)
{
    dev->platform_dev = mbed_dev;
    at24cxx_init(dev);
}

//-----------------------------------------------------------------------------
void at24cxx_mbed_deinit(at24cxx *const dev)
{
    at24cxx_io_deinit(dev);
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_io_init(at24cxx *const dev)
{
    at24cxx_mbed* pd = (at24cxx_mbed*)dev->platform_dev;
    // Set I2C frequency
    pd->i2c->frequency(400000);
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_io_deinit(at24cxx *const dev)
{
    // Empty
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_write_buffer(const at24cxx *const dev, uint32_t addr,
                                    const uint8_t *buf, size_t buf_size)
{
    // Check space
	if (!at24cxx_check_space(dev, addr, buf_size))
    {
        return AT24CXX_OUT_OF_RANGE;
    }

    const at24cxx_mbed *const pd = (at24cxx_mbed*)dev->platform_dev;
    // uint8_t address[2];
    // address[0] = addr >> 8;
    // address[1] = addr;
    //int ack = pd->i2c->write((int)dev->addr, (char*)address, 2, true);
    // if (ack != 0)
    // {
    //     return AT24CXX_I2C_ERR;
    // }
    uint8_t tmp[2+buf_size];
    tmp[0] = addr >> 8;
    tmp[1] = addr;
    memcpy(&tmp[2], buf, buf_size);


    //ack = pd->i2c->write((int)dev->addr, (char*)buf, buf_size);
    int ack = pd->i2c->write((int)dev->addr, (char*)tmp, sizeof(tmp));
    if (ack != 0)
    {
        return AT24CXX_I2C_ERR;
    }
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_read_buffer(const at24cxx *const dev, uint32_t addr,
                                   uint8_t *buf, size_t buf_size)
{
    const at24cxx_mbed *const pd = (at24cxx_mbed*)dev->platform_dev;
    const uint8_t addr_len = 2; //at24cxx_devices[dev->type].word_addr_len;
    uint8_t address[addr_len];
    address[0] = addr >> 8;
    address[1] = addr;

    // Write addr
    int ack = pd->i2c->write((int)dev->addr, (char*)address, addr_len, true);
    if (ack != 0)
    {
        return AT24CXX_I2C_ERR;
    }
    // Sequential Read
    ack = pd->i2c->read(dev->addr, (char*)buf, buf_size);
    if (ack != 0)
    {
        return AT24CXX_I2C_ERR;
    }
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
void at24cxx_enable_wp(at24cxx *const dev, bool enable)
{
    at24cxx_mbed* pd = (at24cxx_mbed*)dev->platform_dev;
    if (enable)
    {
        *(pd->wp) = 1;
        return;
    }
    *(pd->wp) = 0;
}


//-----------------------------------------------------------------------------
void at24cxx_delay_ms(uint32_t delay_ms)
{
    wait_us(delay_ms*1000);
}

//-----------------------------------------------------------------------------

