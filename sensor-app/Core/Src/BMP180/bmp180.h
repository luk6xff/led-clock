/**
 *  @brief:   BMP180 digital pressure sensor library
 *  @author:  luk6xff
 *  @email:   lukasz.uszko@gmail.com
 *  @date:    2020-08-12
 *  @license: MIT
 */


#ifndef __BMP180_H__
#define __BMP180_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


///  default address is 0xEF
#define BMP180_I2C_ADDRESS 0xEF

// Oversampling settings
#define BMP180_OSS_ULTRA_LOW_POWER 0        // 1 sample  and  4.5ms for conversion
#define BMP180_OSS_NORMAL          1        // 2 samples and  7.5ms for conversion
#define BMP180_OSS_HIGH_RESOLUTION 2        // 4 samples and 13.5ms for conversion
#define BMP180_OSS_ULTRA_HIGH_RESOLUTION 3  // 8 samples and 25.5ms for conversion

#define BMP180_ADDRESS                (0x77)

#define BMP180_REG_CHIP_ID            (0xD0)
#define BMP180_REG_VERSION            (0xD1)
#define BMP180_REG_SOFT_RESET         (0xE0)

#define BMP180_REG_AC1                (0xAA)
#define BMP180_REG_AC2                (0xAC)
#define BMP180_REG_AC3                (0xAE)
#define BMP180_REG_AC4                (0xB0)
#define BMP180_REG_AC5                (0xB2)
#define BMP180_REG_AC6                (0xB4)
#define BMP180_REG_B1                 (0xB6)
#define BMP180_REG_B2                 (0xB8)
#define BMP180_REG_MB                 (0xBA)
#define BMP180_REG_MC                 (0xBC)
#define BMP180_REG_MD                 (0xBE)
#define BMP180_REG_CONTROL            (0xF4)
#define BMP180_REG_DATA               (0xF6)

#define BMP180_CMD_MEASURE_TEMP       (0x2E) // Max conversion time 4.5ms
#define BMP180_CMD_MEASURE_PRESSURE_0 (0x34) // Max conversion time 4.5ms (OSS = 0)
#define BMP180_CMD_MEASURE_PRESSURE_1 (0x74) // Max conversion time 7.5ms (OSS = 1)
#define BMP180_CMD_MEASURE_PRESSURE_2 (0xB4) // Max conversion time 13.5ms (OSS = 2)
#define BMP180_CMD_MEASURE_PRESSURE_3 (0xF4) // Max conversion time 25.5ms (OSS = 3)


/**
 * @brief BMP180 calibration data
 */
typedef union
{
    uint16_t value[11];
    struct
    {
        int16_t ac1;
        int16_t ac2;
        int16_t ac3;
        uint16_t ac4;
        uint16_t ac5;
        uint16_t ac6;
        int16_t b1;
        int16_t b2;
        int16_t mb;
        int16_t mc;
        int16_t md;
    };
} bmp180_calibration_t;


/**
 * @brief   Oversampling ratio.
 * @details Dictates how many pressure samples to take. Conversion time varies
 *          depending on the number of samples taken. Refer to data sheet
 *          for timing specifications.
 */
typedef enum
{
    ULTRA_LOW_POWER       = 0, ///< 1 pressure sample
    STANDARD              = 1, ///< 2 pressure samples
    HIGH_RESOLUTION       = 2, ///< 4 pressure samples
    ULTRA_HIGH_RESOLUTION = 3, ///< 8 pressure samples
} bmp180_oversampling_t;

/**
 * @brief BMP180 dev object
 */
typedef struct
{
    uint8_t i2c_addr; // i2c device address
    bmp180_calibration_t calib;
    bmp180_oversampling_t oss;
    float altitude;
    int32_t b5;
    void* platform_dev;
} bmp180;

/**
 * @brief Init function
 *
 * @param[in] dev - bmp180 device object
 *
 */
void bmp180_init(bmp180* const dev);

/**
 * @brief Set config params and read BMP180 calibration parameters
 *
 * @param[in] dev - bmp180 device object
 * @param altitude (in meter)
 * @param overSamplingSetting
 * @return true on success, false on error
 */
bool bmp180_set_configuration(bmp180* const dev, float altitude, int overSamplingSetting);

/**
 * @brief Read pressure and temperature from the BMP180.
 *
 * @param[in] dev - bmp180 device object
 * @param temperature [C]
 * @param pressure [hPa]
 * @return true on success, false on error
 */
bool bmp180_read_data(bmp180* const dev, float* temperature, float* pressure);

/**
 * @brief Get temperature from a previous measurement
 *
 * @param[in] dev - bmp180 device object
 * @param[out] temperature - temperature  value in [C]
 * @return true on success, false on error
 */
bool bmp180_get_temperature(bmp180* const dev, float* temperature);

/**
 * @brief Get pressure from a previous measurement
 *
 * @param[in] dev - bmp180 device object
 * @param[out] pressure - pressure value in [hPa]
 * @return true on success, false on error
 */
bool bmp180_get_pressure(bmp180* const dev, float* pressure);



//-----------------------------------------------------------------------------
// @brief HW DEPENDENT FUNCTIONS - must be implemented for each platform/
//-----------------------------------------------------------------------------
/**
 * @brief Write data via I2C to BMP180 registers
 *
 * @param[in] dev - bmp180 device object
 * @param[in] buf - pointer to buffer which will be send to device
 * @param[in] buf_size - number of bytes to be written
 * @return return value = true on success, false on failure
 */
extern bool bmp180_write(bmp180* const dev, const uint8_t* buf, const size_t buf_size);

/**
 * @brief Read data via I2C from BMP180 registers
 *
 * @param[in] dev - bmp180 device object
 * @param[out] buf - pointer to buffer which will be filled with read data from the device
 * @param[in] buf_size - number of bytes to be read
 * @return return value = true on success, false on failure
 */
extern bool bmp180_read(bmp180* const dev, uint8_t* buf, const size_t buf_size);

/**
 * @brief Miliseconds delay function.
 *
 * @param delay_ms: Delay in miliseconds.
 */
extern void bmp180_delay_ms(uint32_t delay_ms);


#ifdef __cplusplus
}
#endif


#endif /* __BMP180_H__*/

