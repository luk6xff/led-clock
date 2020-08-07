
/**
 *  @brief:  Implementation of a SX1278 platform dependent [MBED] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-08-07
 */


#ifndef __SX1278_MBED_H__
#define __SX1278_MBED_H__

#include "mbed.h"
#include "../../sx1278.h"

/**
 * @brief SX1278 mbed specific dev object.
 */
typedef struct
{
    SPI* spi;
    DigitalOut* nss;
    DigitalInOut* reset;
    InterruptIn* dio0;
    InterruptIn* dio1;
    InterruptIn* dio2;
    InterruptIn* dio3;
    InterruptIn* dio4;
    InterruptIn* dio5;
} sx1278_mbed;


/**
 * @brief SX1278 mbed Init function.
 *
 * @param  dev       SX1278 Device object pointer
 * @param  mbed_dev  Mbed device object pointer
 */
void sx1278_mbed_init(sx1278* const dev, sx1278_mbed* const mbed_dev);

void sx1278_mbed_deinit(sx1278* const dev);

#endif // __SX1278_MBED_H__
