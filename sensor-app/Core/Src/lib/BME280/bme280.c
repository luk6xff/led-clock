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
    bme280_write(dev, &reg, 1);
    return bme280_read(dev, value, 1);
}


//------------------------------------------------------------------------------
// @brief FUNCTIONS DEFINITIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool bme280_init(bme280 *const dev)
{
    bme280_set_configuration(dev);

    // check if sensor, i.e. the chip ID is correct
    const uint8_t sensor_id = bme280_sensor_id(dev);
    if (sensor_id != 0x60)
    {
        return false;
    }

    // Reset the device using soft-reset
    // This makes sure the IIR is off, etc.
    write8(dev, BME280_REGISTER_SOFTRESET, 0xB6);

    // wait for chip to wake up.
    bme280_delay_ms(10);

    //setSampling(); // use defaults

    bme280_delay_ms(100);

    return true;
}

//------------------------------------------------------------------------------
bool bme280_set_configuration(bme280 *const dev)
{
    // Read calibration data
    uint8_t cmd[sizeof(dev->calib.data)];
    uint8_t errors = 0;

    cmd[0] = 0xf2; // ctrl_hum
    cmd[1] = 0x01; // Humidity oversampling x1
    bme280_write(dev, cmd, 2);

    cmd[0] = 0xf4; // ctrl_meas
    cmd[1] = 0x27; // Temparature oversampling x1, Pressure oversampling x1, Normal mode
    bme280_write(dev, cmd, 2);

    cmd[0] = 0xf5; // config
    cmd[1] = 0xa0; // Standby 1000ms, Filter off
    bme280_write(dev, cmd, 2);

    cmd[0] = BME280_REGISTER_DIG_T1; // read dig_T regs
    bme280_write(dev, cmd, 1);
    bme280_read(dev, cmd, 6);

    dev->calib.dig_T1 = (cmd[1] << 8) | cmd[0];
    dev->calib.dig_T2 = (cmd[3] << 8) | cmd[2];
    dev->calib.dig_T3 = (cmd[5] << 8) | cmd[4];

    cmd[0] = 0x8E; // read dig_P regs
    bme280_write(dev, cmd, 1);
    bme280_read(dev, cmd, 18);

    dev->calib.dig_P1 = (cmd[ 1] << 8) | cmd[ 0];
    dev->calib.dig_P2 = (cmd[ 3] << 8) | cmd[ 2];
    dev->calib.dig_P3 = (cmd[ 5] << 8) | cmd[ 4];
    dev->calib.dig_P4 = (cmd[ 7] << 8) | cmd[ 6];
    dev->calib.dig_P5 = (cmd[ 9] << 8) | cmd[ 8];
    dev->calib.dig_P6 = (cmd[11] << 8) | cmd[10];
    dev->calib.dig_P7 = (cmd[13] << 8) | cmd[12];
    dev->calib.dig_P8 = (cmd[15] << 8) | cmd[14];
    dev->calib.dig_P9 = (cmd[17] << 8) | cmd[16];

    cmd[0] = 0xA1; // read dig_H regs
    bme280_write(dev, cmd, 1);
    bme280_read(dev, cmd, 1);
    cmd[1] = 0xE1; // read dig_H regs
    bme280_write(dev, &cmd[1], 1);
    bme280_read(dev, &cmd[1], 7);

    dev->calib.dig_H1 = cmd[0];
    dev->calib.dig_H2 = (cmd[2] << 8) | cmd[1];
    dev->calib.dig_H3 = cmd[3];
    dev->calib.dig_H4 = (cmd[4] << 4) | (cmd[5] & 0x0f);
    dev->calib.dig_H5 = (cmd[6] << 4) | ((cmd[5]>>4) & 0x0f);
    dev->calib.dig_H6 = cmd[7];

    return true;
}

//------------------------------------------------------------------------------
bool bme280_reset(bme280 *const dev)
{
    write8(dev, BME280_REGISTER_CHIPID, 0xB6);
    return true;
}

//------------------------------------------------------------------------------
uint8_t bme280_sensor_id(bme280 *const dev)
{
    uint8_t data;
    read8(dev, BME280_REGISTER_CHIPID, &data);
    return data;
}

//------------------------------------------------------------------------------
bool bme280_read_data(bme280 *const dev, float *temperature, float *pressure, float *humidity)
{
    if (!bme280_read_temperature(dev, temperature) ||
        !bme280_read_pressure(dev, pressure) ||
        !bme280_read_humidity(dev, humidity))
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/*!
 *   @brief return true if chip is busy reading cal data
 *   @returns true if reading calibration, false otherwise
 */
// bool isReadingCalibration(void) {
//   uint8_t const rStatus = read8(BME280_REGISTER_STATUS);

//   return (rStatus & (1 << 0)) != 0;
// }


//------------------------------------------------------------------------------
bool bme280_read_temperature(bme280 *const dev, float *temperature)
{
    uint32_t adc_T;
    int32_t var1, var2;
    float tempf;
    uint8_t cmd[4];

    cmd[0] = BME280_REGISTER_TEMPDATA; // temp_msb
    bme280_write(dev, &cmd[0], 1);
    bme280_read(dev, &cmd[1], 3);

    adc_T = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);

    var1 = ((((adc_T >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) *
            ((int32_t)dev->calib.dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)dev->calib.dig_T1)) *
            ((adc_T >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) *
            ((int32_t)dev->calib.dig_T3)) >> 14;

    dev->t_fine = var1 + var2 + dev->t_fine_adjust;

    tempf = (float)((uint32_t)(dev->t_fine * 5 + 128) >> 8);
    *temperature = (tempf / 100.0f);
    return true;
}


//------------------------------------------------------------------------------
bool bme280_read_pressure(bme280 *const dev, float *pressure)
{
    int64_t var1, var2, p;
    uint32_t adc_P;
    uint8_t cmd[4];
    float pressf;
    float temperature;

    // Must be done first to get t_fine
    bme280_read_temperature(dev, &temperature);

    cmd[0] = BME280_REGISTER_PRESSUREDATA; // press_msb
    bme280_write(dev, &cmd[0], 1);
    bme280_read(dev, &cmd[1], 3);

    adc_P = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);

    var1 = ((int64_t)dev->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dev->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->calib.dig_P3) >> 8) +
            ((var1 * (int64_t)dev->calib.dig_P2) << 12);
    var1 =
        (((((int64_t)1) << 47) + var1)) * ((int64_t)dev->calib.dig_P1) >> 33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dev->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);
    pressf = ((float)p / 256);
    *pressure = pressf;

    return true;
}


//------------------------------------------------------------------------------
bool bme280_read_humidity(bme280 *const dev, float *humidity)
{
    int64_t var1, var2, p;
    int32_t adc_H;
    uint8_t cmd[3];
    float humidf;
    float temperature;

    // Must be done first to get t_fine
    bme280_read_temperature(dev, &temperature);

    cmd[0] = BME280_REGISTER_HUMIDDATA; // press_msb
    bme280_write(dev, &cmd[0], 1);
    bme280_read(dev, &cmd[1], 2);

    adc_H = (cmd[1] << 8) | cmd[2];
    // Value in case humidity measurement was disabled
    if (adc_H == 0x8000)
    {
        return false;
    }

    int32_t v_x1_u32r;

    v_x1_u32r = (dev->t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dev->calib.dig_H4) << 20) -
                    (((int32_t)dev->calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                (((((((v_x1_u32r * ((int32_t)dev->calib.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)dev->calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 0) +
                    ((int32_t)2097152)) * ((int32_t)dev->calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                                ((int32_t)dev->calib.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    humidf = (v_x1_u32r >> 12);
    *humidity = (humidf / 1024.0f);
    return true;
}

//------------------------------------------------------------------------------
float bme280_read_altitude(bme280 *const dev, float sea_level)
{
    // Equation taken from BMP180 datasheet (page 16):
    //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    // Note that using the equation from wikipedia can give bad results
    // at high altitude. See this thread for more information:
    //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

    float atmospheric;
    bme280_read_pressure(dev, &atmospheric);
    atmospheric = (atmospheric / 100.0F);
    return 44330.0 * (1.0 - pow(atmospheric / sea_level, 0.1903));
}


//------------------------------------------------------------------------------
float bme280_sea_level_for_altitude(float altitude, float atmospheric)
{
    // Equation taken from BMP180 datasheet (page 17):
    //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    // Note that using the equation from wikipedia can give bad results
    // at high altitude. See this thread for more information:
    //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

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
    // convert the value in C into and adjustment to t_fine
    dev->t_fine_adjust = (int32_t)(((int32_t)adjustment * 100) << 8) / 5;
};
