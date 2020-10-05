
#if AT24CXX_STM32CUBE

#include "at24cxx-cube.h"

// I2C handle
static I2C_HandleTypeDef* _i2c;
static GPIO_TypeDef* _wp_port;
static uint16_t _wp_pin;

//-----------------------------------------------------------------------------
void at24cxx_cube_init(I2C_HandleTypeDef* i2c, GPIO_TypeDef* GPIO_WP_Port, uint16_t GPIO_WP_Pin,
		   	   	   	   uint8_t chip_addr, size_t chip_size, size_t page_size)
{
	// I2C already initialized
	_i2c = i2c;
	// GPIO
	_wp_port = GPIO_WP_Port;
	_wp_pin = GPIO_WP_Pin;
    at24cxx_init(chip_addr, chip_size, page_size);
}

//-----------------------------------------------------------------------------
void at24cxx_cube_deinit(void)
{
    at24cxx_deinit();
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_io_init()
{
	// Empty
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_io_deinit()
{
    // Empty
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_write_buffer(uint16_t addr, uint8_t *buf, size_t buf_size)
{
    // Check space
	if (!at24cxx_check_space(addr, buf_size))
    {
        return AT24CXX_OUT_OF_RANGE;
    }

    int ack = HAL_I2C_Master_Transmit(_i2c, addr, buf, buf_size, 1000);
    if (ack != 0)
    {
        return AT24CXX_ERR;
    }
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_read_buffer(uint16_t addr, uint8_t *buf, size_t buf_size)
{
    int retVal = HAL_I2C_Master_Receive(_i2c, addr, buf, buf_size, 1000);
    if (retVal != 0)
    {
        return AT24CXX_ERR;
    }
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
void at24cxx_enable_wp(bool enable)
{
    if (enable)
    {
    	HAL_GPIO_WritePin(_wp_port, _wp_pin, GPIO_PIN_SET);
        return;
    }
	HAL_GPIO_WritePin(_wp_port, _wp_pin, GPIO_PIN_RESET);
}


//-----------------------------------------------------------------------------
void at24cxx_delay_ms(uint32_t delay_ms)
{
	  uint32_t tickstart_ms = HAL_GetTick();
	  while((HAL_GetTick()-tickstart_ms) < delay_ms);
}

//-----------------------------------------------------------------------------

#endif