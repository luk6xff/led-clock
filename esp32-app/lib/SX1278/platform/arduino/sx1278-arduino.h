/**
 *  @brief:  Implementation of a SX1278 platform dependent [ARDUINO] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-08-07
 */

#ifndef __SX1278_ARDUINO_H__
#define __SX1278_ARDUINO_H__

#include <Arduino.h>
#include <SPI.h>

#include "../../sx1278.h"

/**
 * @brief SX1278 arduino specific dev object.
 */
typedef struct
{
    SPISettings spi_settings;
    SPIClass* spi;
    int mosi;
    int miso;
    int sck;
    int nss;
    int reset;
    int dio0;
    int dio1;
    int dio2;
    int dio3;
    int dio4;
    int dio5;
} sx1278_arduino;


/**
 * @brief SX1278 arduino Init function.
 *
 * @param  dev       SX1278 Device object pointer
 * @param  arduino_dev  Arduino device object pointer
 */
void sx1278_arduino_init(sx1278 *const dev, sx1278_arduino *const arduino_dev);

void sx1278_arduino_deinit(sx1278 *const dev);

#endif // __SX1278_ARDUINO_H__
