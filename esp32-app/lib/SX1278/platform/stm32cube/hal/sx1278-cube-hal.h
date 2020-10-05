
/**
 *  @brief:  Implementation of a SX1278 platform dependent [STM32 CUBE HAL] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-01-05
 */


#ifndef __SX1278_CUBE_HAL_H__
#define __SX1278_CUBE_HAL_H__

#include "stm32l0xx_hal.h"
#include "../../../sx1278.h"


/** SX1278 STM32Cube  specific dev object **/
typedef struct
{
    SPI_HandleTypeDef* spi;
    GPIO_TypeDef* nss_port;
    uint16_t      nss_pin;
    GPIO_TypeDef* reset_port;
    uint16_t      reset_pin;
} sx1278_cube_hal;


/**
 * @brief SX1278 STM32Cube Init function driving.
 *
 * @param  dev       SX1278 Device object pointer
 * @param  cube_dev  STM32Cube  device object pointer
 */
void sx1278_cube_hal_init(sx1278 *const dev, sx1278_cube_hal *const cube_dev);


/**
 * @brief SX1278 STM32Cube deinit function driving.
 *
 * @param  dev       SX1278 Device object pointer
 */
void sx1278_cube_hal_deinit(sx1278 *const dev);

#endif // __SX1278_CUBE_HAL_H__
