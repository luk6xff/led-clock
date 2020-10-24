/*
 * sensor.h
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <stdbool.h>

void sensor_init();

bool sensor_get_data(float *temperature, float *pressure, float *humidity);

#endif /* SENSOR_H_ */
