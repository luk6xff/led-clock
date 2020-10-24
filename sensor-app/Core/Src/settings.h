/*
 * settings.h
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>
#include <stdbool.h>


/**
 * @brief Application EEPROM settings main structure
 */
typedef struct
{
    uint32_t magic;
    uint32_t version;
    /**
     * @brief Length of measurement time period in seconds [s].
     *        When elapsed a new measurement shall begin
     */
    uint32_t update_interval;
    /**
     * @brief Critical value of vbat after when reached, the device should disable measuremnt
     */
    uint32_t  critical_battery_level;
} app_settings;



void app_settings_init();

app_settings *app_settings_get_current();

bool app_settings_store(const app_settings* settings);
bool app_settings_read();

void app_settings_print_current();

#endif /* SETTINGS_H_ */
