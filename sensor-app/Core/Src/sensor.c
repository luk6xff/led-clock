/*
 * sensor.c
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */
#include "sensor.h"
#include "lib/BME280/platform/cube/bme280-cube-hal.h"
#include <main.h>

//-----------------------------------------------------------------------------
extern void dbg(const char* msg);
extern I2C_HandleTypeDef hi2c1;
//-----------------------------------------------------------------------------
// BME280 SENSOR
static bme280 bme280_dev =
{
	.intf = BME280_INTF_I2C,
	.i2c_addr = 0xEC,
	.t_fine = 0,
	.t_fine_adjust = 0,
};

static bme280_cube_hal bme280_cube_hal_dev =
{
	.i2c = &hi2c1,
};

//-----------------------------------------------------------------------------
void sensor_init()
{
	// Set sensors_dev
	if (!bme280_cube_hal_init(&bme280_dev, &bme280_cube_hal_dev))
	{
		dbg("SEE");
		Error_Handler();
	}
}

//-----------------------------------------------------------------------------
bool sensor_get_data(float *temperature, float *pressure, float *humidity)
{
    if (bme280_read_data(&bme280_dev, temperature, pressure, humidity))
    {
    	return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
