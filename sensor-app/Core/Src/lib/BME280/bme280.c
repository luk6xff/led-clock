/**
 *  @brief:   BME280 digital pressure, temperature and humidity sensor library
 *  @author:  luk6xff
 *  @email:   lukasz.uszko@gmail.com
 *  @date:    2020-08-18
 *  @license: MIT
 */

#include "bme280.h"
#include <math.h> // pow

// Uncomment to test the documentation algorithm against the documentation example
// Result should be 699.64 hPa and 15°C
// #define BME280_TEST_FORMULA


//------------------------------------------------------------------------------
// @brief PRIVATE FUNCTIONS
//------------------------------------------------------------------------------

/**
 * @brief Reads raw temperature value from the sensor
 *
 * @return temperature [C]
 */
static int bme280_read_raw_temperature(bme280* const dev, int32_t* pUt);

/**
 * @brief Reads raw pressure value from the sensor
 *
 * @return pressure [hPa]
 */
static int bme280_read_raw_pressure(bme280* const dev, int32_t* pUp);

/**
 * @brief Calculation of the temperature from the digital output
 */
static float bme280_calc_true_temperature(bme280* const dev, int32_t ut);

/**
 * @brief Calculation of the pressure from the digital output
 */
static float bme280_calc_true_pressure(bme280* const dev, int32_t up);


//------------------------------------------------------------------------------
// @brief FUNCTIONS DEFINITIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool bme280_init(bme280* const dev)
{
    bme280_set_configuration(dev, dev->altitude, dev->oss);
  // check if sensor, i.e. the chip ID is correct
  const uint8_t sensor_id = bme280_sensor_id(dev);
  if (sensor_id != 0x60)
  {
    return false;
  }

  // reset the device using soft-reset
  // this makes sure the IIR is off, etc.
  write8(BME280_REGISTER_SOFTRESET, 0xB6);

  // wait for chip to wake up.
  bme280_delay_ms(10);

  // if chip is still reading calibration, delay
  while (isReadingCalibration())
    delay(10);

  readCoefficients(); // read trimming parameters, see DS 4.2.2

  setSampling(); // use defaults

  bme280_delay_ms(100);

  return true;
}

//------------------------------------------------------------------------------
bool bme280_set_configuration(bme280* const dev)
{
    uint8_t data[sizeof(dev->calib.data)];
    uint8_t errors = 0;

    // read calibration data
    data[0] = BME280_REGISTER_DIG_T1;
    errors += bme280_write(dev, data, 1);
    errors += bme280_read(dev, data, sizeof(data)); // // read 11 x 16 bits at this position
    bme280_delay_ms(10);

    // store calibration data for further calculus
    dev->calib.dig_T1 = (data[0] << 8 | data[1];
    dev->calib.dig_T2 = (data[2] << 8 | data[3];
    dev->calib.dig_T3 = data[4] << 8 | data[5];

    dev->calib.dig_P1 = data[6] << 8 | data[7];
    dev->calib.dig_P2 = data[8] << 8 | data[9];
    dev->calib.dig_P3 = data[10] << 8 | data[11];
    dev->calib.dig_P4 = data[12] << 8 | data[13];
    dev->calib.dig_P5 = data[14] << 8 | data[15];
    dev->calib.dig_P6 = data[16] << 8 | data[17];
    dev->calib.dig_P7 = data[18] << 8 | data[19];
    dev->calib.dig_P8 = data[20] << 8 | data[21];
    dev->calib.dig_P9 = data[22] << 8 | data[23];

    dev->calib.dig_H1 = data[24];
    dev->calib.dig_H2 = data[25] << 8 | data[26];
    dev->calib.dig_H3 = data[27] << 8 | data[28];
    dev->calib.dig_H4 = ((int8_t)data[29] << 4) | (data[30] & 0xF);
    dev->calib.dig_H5 = ((int8_t)read8(BME280_REGISTER_DIG_H5 + 1) << 4) |
                            (read8(BME280_REGISTER_DIG_H5) >> 4);
    dev->calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
    dev->calib.ac1 =
    dev->calib.ac2 =
    dev->calib.ac3 =
    dev->calib.ac4 =
    dev->calib.ac5 =
    dev->calib.ac6 =
    dev->calib.b1  =
    dev->calib.b2  =
    dev->calib.mb  =
    dev->calib.mc  =
    dev->calib.md  =

#ifdef BME280_TEST_FORMULA
    dev->calib.ac1 = 408;
    dev->calib.ac2 = -72;
    dev->calib.ac3 = -14383;
    dev->calib.ac4 = 32741;
    dev->calib.ac5 = 32757;
    dev->calib.ac6 = 23153;
    dev->calib.b1 = 6190;
    dev->calib.b2 = 4;
    dev->calib.mb = -32768;
    dev->calib.mc = -8711;
    dev->calib.md = 2868;
    dev->oss = 0;
#endif // #ifdef BME280_TEST_FORMULA

    return errors ? 1 : 0;
}


bool bme280_reset(bme280* const dev)
{
    const uint8_t data[2] = { BME280_REGISTER_CHIPID, 0xB6 };
    bme280_write(dev, data, sizeof(data));
}

//------------------------------------------------------------------------------
int bme280_sensor_id(bme280* const dev)
{
    uint8_t data = BME280_REGISTER_CHIPID;
    bme280_write(dev, data, 1);
    bme280_read(dev, data, 1);
    return data;
}

// //------------------------------------------------------------------------------
// bool bme280_read_data(bme280* const dev, float* temperature, float* pressure)
// {
//     if (!bme280_get_temperature(dev, temperature) || !bme280_get_pressure(dev, pressure))
//     {
//         return false;
//     }
//     return true;
// }

//------------------------------------------------------------------------------
/*!
 *  @brief  Take a new measurement (only possible in forced mode)
 */
bool takeForcedMeasurement() {
  // If we are in forced mode, the BME sensor goes back to sleep after each
  // measurement and we need to set it to forced mode once at this point, so
  // it will take the next measurement and then return to sleep again.
  // In normal mode simply does new measurements periodically.
  if (_measReg.mode == MODE_FORCED) {
    // set to forced mode, i.e. "take next measurement"
    write8(BME280_REGISTER_CONTROL, _measReg.get());
    // wait until measurement has been completed, otherwise we would read
    // the values from the last measurement
    while (read8(BME280_REGISTER_STATUS) & 0x08)
      delay(1);
  }
}

/*!
 *   @brief  Reads the factory-set coefficients
 */
bool readCoefficients(void) {
}

/*!
 *   @brief return true if chip is busy reading cal data
 *   @returns true if reading calibration, false otherwise
 */
bool isReadingCalibration(void) {
  uint8_t const rStatus = read8(BME280_REGISTER_STATUS);

  return (rStatus & (1 << 0)) != 0;
}

/*!
 *   @brief  Returns the temperature from the sensor
 *   @returns the temperature read from the device
 */
bool bme280_read_temperature(bme280* const dev, float* temperature)
{
  int32_t var1, var2;

  int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
  if (adc_T == 0x800000) // value in case temp measurement was disabled
    return NAN;
  adc_T >>= 4;

  var1 = ((((adc_T >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) *
          ((int32_t)dev->calib.dig_T2)) >>
         11;

  var2 = (((((adc_T >> 4) - ((int32_t)dev->calib.dig_T1)) *
            ((adc_T >> 4) - ((int32_t)dev->calib.dig_T1))) >>
           12) *
          ((int32_t)dev->calib.dig_T3)) >>
         14;

  t_fine = var1 + var2 + t_fine_adjust;

  float T = (t_fine * 5 + 128) >> 8;
  return T / 100;
}

/*!
 *   @brief  Returns the pressure from the sensor
 *   @returns the pressure value (in Pascal) read from the device
 */
float readPressure(void) {
  int64_t var1, var2, p;

  readTemperature(); // must be done first to get t_fine

  int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
  if (adc_P == 0x800000) // value in case pressure measurement was disabled
    return NAN;
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
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
  return (float)p / 256;
}

/*!
 *  @brief  Returns the humidity from the sensor
 *  @returns the humidity value read from the device
 */
float readHumidity(void) {
  readTemperature(); // must be done first to get t_fine

  int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
  if (adc_H == 0x8000) // value in case humidity measurement was disabled
    return NAN;

  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)dev->calib.dig_H4) << 20) -
                  (((int32_t)dev->calib.dig_H5) * v_x1_u32r)) +
                 ((int32_t)16384)) >>
                15) *
               (((((((v_x1_u32r * ((int32_t)dev->calib.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)dev->calib.dig_H3)) >> 11) +
                     ((int32_t)32768))) >>
                   10) +
                  ((int32_t)2097152)) *
                     ((int32_t)dev->calib.dig_H2) +
                 8192) >>
                14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)dev->calib.dig_H1)) >>
                            4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r >> 12);
  return h / 1024.0;
}

/*!
 *   Calculates the altitude (in meters) from the specified atmospheric
 *   pressure (in hPa), and sea-level pressure (in hPa).
 *   @param  seaLevel      Sea-level pressure in hPa
 *   @returns the altitude value read from the device
 */
float bme280_read_altitude(float seaLevel) {
  // Equation taken from BMP180 datasheet (page 16):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude. See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

  float atmospheric = readPressure() / 100.0F;
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

/*!
 *   Calculates the pressure at sea level (in hPa) from the specified
 * altitude (in meters), and atmospheric pressure (in hPa).
 *   @param  altitude      Altitude in meters
 *   @param  atmospheric   Atmospheric pressure in hPa
 *   @returns the pressure at sea level (in hPa) from the specified altitude
 */
float bme280_sea_level_for_altitude(float altitude, float atmospheric) {
  // Equation taken from BMP180 datasheet (page 17):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude. See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

  return atmospheric / pow(1.0 - (altitude / 44330.0), 5.255);
}

/*!
 *   Returns the current temperature compensation value in degrees Celcius
 *   @returns the current temperature compensation value in degrees Celcius
 */
float bme280_get_temperature_compensation(void) {
  return float(((t_fine_adjust * 5) >> 8) / 100);
};

/*!
 *  Sets a value to be added to each temperature reading. This adjusted
 *  temperature is used in pressure and humidity readings.
 *  @param  adjustment  Value to be added to each tempature reading in Celcius
 */
void bme280_set_temperature_compensation(float adjustment) {
  // convert the value in C into and adjustment to t_fine
  t_fine_adjust = ((int32_t(adjustment * 100) << 8)) / 5;
};
