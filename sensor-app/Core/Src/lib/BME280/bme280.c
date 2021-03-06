/**
 *  @brief:   BME280 digital pressure, temperature and humidity sensor library
 *  @author:  luk6xff
 *  @email:   lukasz.uszko@gmail.com
 *  @date:    2020-08-18
 *  @license: MIT
 */

#include "bme280.h"
#include <math.h> // pow


//------------------------------------------------------------------------------
// @brief PRIVATE FUNCTIONS
//------------------------------------------------------------------------------

/**
 * @brief Write a byte of data into a register
 */
static bool write8(bme280 *const dev, const uint8_t reg, const uint8_t value)
{
    const uint8_t data[2] = { reg, value };
    return bme280_write(dev, data, sizeof(data));
}

/**
 * @brief Read a byte of data from the register
 */
static bool read8(bme280 *const dev, const uint8_t reg, uint8_t* value)
{
    uint8_t errors = 0;
    errors += bme280_write(dev, &reg, 1);
    errors += bme280_read(dev, value, 1);
    return (errors == 2) ? true : false;
}


//------------------------------------------------------------------------------
// @brief FUNCTIONS DEFINITIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool bme280_init(bme280 *const dev)
{
    bme280_set_configuration(dev);

    // Check if sensor, i.e. the chip ID is correct
    const uint8_t sensor_id = bme280_sensor_id(dev);
    if (sensor_id != 0x60)
    {
        return false;
    }

    // Wait for chip to wake up.
    bme280_delay_ms(10);

    return true;
}

//------------------------------------------------------------------------------
bool bme280_set_configuration(bme280 *const dev)
{
    uint8_t cmd[18];
    uint8_t status = 0;

    // Set configuration registers

    // Settings shall be done in sleep_mode
    status += bme280_set_mode(dev, MODE_SLEEP);

    // ctrl_hum
    cmd[0] = BME280_REGISTER_CTRL_HUM;
    ctrl_hum_reg h = { .osrs_h = SAMPLING_X1, .none = 0 };
    cmd[1] = h.reg;
    status += bme280_write(dev, cmd, 2);
    // ctrl_meas
    cmd[0] = BME280_REGISTER_CTRL_MEAS;
    ctrl_meas_reg m = { .mode = MODE_SLEEP, .osrs_p = SAMPLING_X1, .osrs_t = SAMPLING_X1 };
    cmd[1] = m.reg;
    status += bme280_write(dev, cmd, 2);
    // config
    cmd[0] = BME280_REGISTER_CONFIG;
    config_reg c = { .filter = FILTER_OFF, .t_sb = STANDBY_MS_1000 };
    cmd[1] = c.reg;
    status += bme280_write(dev, cmd, 2);


    // Read calibration data
    cmd[0] = BME280_REGISTER_DIG_T1; // read dig_T regs
    status += bme280_write(dev, cmd, 1);
    status += bme280_read(dev, cmd, 6);

    dev->calib.dig_T1 = (cmd[1] << 8) | cmd[0];
    dev->calib.dig_T2 = (cmd[3] << 8) | cmd[2];
    dev->calib.dig_T3 = (cmd[5] << 8) | cmd[4];

    cmd[0] = 0x8E; // read dig_P regs
    status += bme280_write(dev, cmd, 1);
    status += bme280_read(dev, cmd, 18);

    dev->calib.dig_P1 = (cmd[ 1] << 8) | cmd[ 0];
    dev->calib.dig_P2 = (cmd[ 3] << 8) | cmd[ 2];
    dev->calib.dig_P3 = (cmd[ 5] << 8) | cmd[ 4];
    dev->calib.dig_P4 = (cmd[ 7] << 8) | cmd[ 6];
    dev->calib.dig_P5 = (cmd[ 9] << 8) | cmd[ 8];
    dev->calib.dig_P6 = (cmd[11] << 8) | cmd[10];
    dev->calib.dig_P7 = (cmd[13] << 8) | cmd[12];
    dev->calib.dig_P8 = (cmd[15] << 8) | cmd[14];
    dev->calib.dig_P9 = (cmd[17] << 8) | cmd[16];

    cmd[0] = BME280_REGISTER_DIG_H1; // read dig_H regs
    status += bme280_write(dev, cmd, 1);
    status += bme280_read(dev, cmd, 1);
    cmd[1] = BME280_REGISTER_DIG_H2; // read dig_H regs
    status += bme280_write(dev, &cmd[1], 1);
    status += bme280_read(dev, &cmd[1], 7);

    dev->calib.dig_H1 = cmd[0];
    dev->calib.dig_H2 = (cmd[2] << 8) | cmd[1];
    dev->calib.dig_H3 = cmd[3];
    dev->calib.dig_H4 = (cmd[4] << 4) | (cmd[5] & 0x0F);
    dev->calib.dig_H5 = (cmd[6] << 4) | ((cmd[5]>>4) & 0x0F);
    dev->calib.dig_H6 = cmd[7];

    return status == 12;
}

//------------------------------------------------------------------------------
bool bme280_reset(bme280 *const dev)
{
	return write8(dev, BME280_REGISTER_CHIPID, 0xB6);
}

//------------------------------------------------------------------------------
bool bme280_set_mode(bme280 *const dev, bme280_sensor_mode mode)
{
    uint8_t data;
    if (read8(dev, BME280_REGISTER_CTRL_MEAS, &data))
    {
		ctrl_meas_reg *p = (ctrl_meas_reg*)&data;
		p->mode = mode;
		return write8(dev, BME280_REGISTER_CTRL_MEAS, p->reg);
	}
    return false;
}


//------------------------------------------------------------------------------
uint8_t bme280_sensor_id(bme280 *const dev)
{
	uint8_t data;
	if (read8(dev, BME280_REGISTER_CHIPID, &data))
	{
		return data;
	}
	return 0xFF;
}


//------------------------------------------------------------------------------
bool bme280_is_measuring(bme280 *const dev)
{
	uint8_t stat;
	if (read8(dev, BME280_REGISTER_STATUS, &stat))
	{
		return(stat & (1<<3)); // If the measuring bit (3) is set, return true
	}

	return false;
}


//------------------------------------------------------------------------------
bool bme280_read_data(bme280 *const dev, float *temperature, float *pressure, float *humidity)
{
	bool ret = false;
	dev->burst_read_mode = true;

	if (bme280_set_mode(dev, MODE_FORCED))
	{
		// Wait for sensor to start measurement
		while (bme280_is_measuring(dev) == false);
		// Hang out while sensor completes the reading
		while (bme280_is_measuring(dev) == true);

		if (bme280_read_temperature(dev, temperature) &&
			bme280_read_pressure(dev, pressure) &&
			bme280_read_humidity(dev, humidity))
		{
			ret = true;
		}
	}

	dev->burst_read_mode = false;
    return ret;
}


//------------------------------------------------------------------------------
bool bme280_read_temperature(bme280 *const dev, float *temperature)
{
    int32_t temp_raw;
	int64_t var1, var2;
    float tempf;
    uint8_t cmd[4];

    cmd[0] = BME280_REGISTER_TEMPDATA;
    bme280_write(dev, &cmd[0], 1);
    bme280_read(dev, &cmd[1], 3);

    temp_raw = (cmd[1] << 12) | (cmd[2] << 4) | ((cmd[3] >> 4) & 0x0F);

	var1 = ((((temp_raw >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) * ((int32_t)dev->calib.dig_T2)) >> 11;
	var2 = (((((temp_raw >> 4) - ((int32_t)dev->calib.dig_T1)) * ((temp_raw >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) *
			((int32_t)dev->calib.dig_T3)) >> 14;
	dev->t_fine = var1 + var2 + dev->t_fine_adjust;

    tempf = (dev->t_fine * 5 + 128) >> 8;
    *temperature = (tempf / 100.0f);

    return true;
}


//------------------------------------------------------------------------------
bool bme280_read_pressure(bme280 *const dev, float *pressure)
{
    int64_t var1, var2, p;
    int32_t press_raw;
    uint8_t cmd[4];
    float temperature;

    if (!dev->burst_read_mode)
    {
        // Must be done first to get t_fine
        bme280_read_temperature(dev, &temperature);
    }

    cmd[0] = BME280_REGISTER_PRESSUREDATA;
    bme280_write(dev, &cmd[0], 1);
    bme280_read(dev, &cmd[1], 3);

    press_raw = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);

    var1 = ((int64_t)dev->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dev->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->calib.dig_P3) >> 8) +
            ((var1 * (int64_t)dev->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dev->calib.dig_P1) >> 33;

    if (var1 == 0)
    {
        return false; // avoid exception caused by division by zero
    }
    p = 1048576 - press_raw;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dev->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);
    *pressure = ((float)p / 256.0f);

    return true;
}


//------------------------------------------------------------------------------
bool bme280_read_humidity(bme280 *const dev, float *humidity)
{
    int32_t var1;
    int32_t humid_raw;
    uint8_t cmd[3];
    float temperature;

    if (!dev->burst_read_mode)
    {
        // Must be done first to get t_fine
        bme280_read_temperature(dev, &temperature);
    }

    cmd[0] = BME280_REGISTER_HUMIDDATA; // press_msb
    bme280_write(dev, &cmd[0], 1);
    bme280_read(dev, &cmd[1], 2);

    humid_raw = (cmd[1] << 8) | cmd[2];

    // Value in case humidity measurement was disabled
    if (humid_raw == 0x8000)
    {
        return false;
    }

	var1 = (dev->t_fine - ((int32_t)76800));
	var1 = (((((humid_raw << 14) - (((int32_t)dev->calib.dig_H4) << 20) - (((int32_t)dev->calib.dig_H5) * var1)) +
			((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)dev->calib.dig_H6)) >> 10) * (((var1 * ((int32_t)dev->calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
			((int32_t)dev->calib.dig_H2) + 8192) >> 14));
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)dev->calib.dig_H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);
	*humidity = (float)(var1>>12) / 1024.0f;

    return true;
}

//------------------------------------------------------------------------------
bool bme280_read_altitude(bme280 *const dev, const float sea_level_pressure, float *altitude)
{
    float atmospheric;
    bool ret = bme280_read_pressure(dev, &atmospheric);
    atmospheric = (atmospheric / 100.0f);
    *altitude = (44330.0 * (1.0 - pow(atmospheric / sea_level_pressure, 0.1903)));
    return ret;
}


//------------------------------------------------------------------------------
float bme280_sea_level_for_altitude(float altitude, float atmospheric)
{
    return atmospheric / pow(1.0 - (altitude / 44330.0), 5.255);
}

//------------------------------------------------------------------------------
float bme280_get_temperature_compensation(bme280 *const dev)
{
    return (float)((((uint32_t)(dev->t_fine_adjust * 5)) >> 8) / 100);
};


//------------------------------------------------------------------------------
void bme280_set_temperature_compensation(bme280 *const dev, float adjustment)
{
    // Convert the value in C into and adjustment to t_fine
    dev->t_fine_adjust = (int32_t)(((int32_t)adjustment * 100) << 8) / 5;
};
