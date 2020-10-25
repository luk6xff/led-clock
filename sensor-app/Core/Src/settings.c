/*
 * settings.c
 *
 *  Created on: Oct 24, 2020
 *      Author: luk6xff
 */

#include "settings.h"

#include <stdint.h>
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_flash_ex.h"

//------------------------------------------------------------------------------
// EXTERNS
//------------------------------------------------------------------------------
void dbg(const char* msg);

//------------------------------------------------------------------------------
// DEFINES, MACROS, TYPEDEFS
//------------------------------------------------------------------------------
#define SETTINGS_EEPROM_ADDR DATA_EEPROM_BASE

//------------------------------------------------------------------------------
// STATIC LOCAL FUNCTIONS DECLARATIONS
//------------------------------------------------------------------------------
static void eeprom_unlock();

/**
 * @brief Fills whole EEPROM memory with zeroes
 */
static void eeprom_lock();

/**
 * @brief Fills whole EEPROM memory with zeroes
 */
static void eeprom_clear();

/**
 * @brief Read a given number of bytes from eeprom memory
 */
static bool eeprom_read(const uint32_t addr, uint8_t* data, const uint32_t data_size);

/**
 * @brief Write a given number of bytes to eeprom memory
 */
static bool eeprom_write(const uint32_t addr, const uint8_t* data, const uint32_t data_size);


//------------------------------------------------------------------------------
// STATIC LOCAL OBJECTS
//------------------------------------------------------------------------------
static const app_settings app_settings_default =
{
	.magic					= 0xAABBCCDD,
	.version				= 0x00000001,
	.critical_battery_level = 3000,
	.update_interval		= 30,

};

static app_settings app_settings_current;


//------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
app_settings *app_settings_get_current()
{
	return &app_settings_current;
}

//------------------------------------------------------------------------------
void app_settings_init()
{
    if (app_settings_read())
    {
        // Print content
    	//app_settings_print_current();

        // Check if settings are valid
        if (app_settings_get_current()->magic != app_settings_default.magic || \
        	app_settings_get_current()->version != app_settings_default.version)
        {
            if (!app_settings_store(&app_settings_default))
            {
                dbg("STNE");
                app_settings_current = app_settings_default;
            }
            else
            {
                dbg("STNS");
            }
        }
    }
    else
    {
        dbg("STND");
        app_settings_current = app_settings_default;
    }
}

//------------------------------------------------------------------------------
bool app_settings_store(const app_settings* settings)
{
	return eeprom_write(SETTINGS_EEPROM_ADDR, (const uint8_t*)settings, sizeof(app_settings_current));
}

//------------------------------------------------------------------------------
bool app_settings_read()
{
	return eeprom_read(SETTINGS_EEPROM_ADDR, (uint8_t*)&app_settings_current, sizeof(app_settings_current));
}

//------------------------------------------------------------------------------
void app_settings_print_current()
{

}

//------------------------------------------------------------------------------
// STATIC LOCAL FUNCTIONS DEFINITIONS
//------------------------------------------------------------------------------
static void eeprom_unlock()
{
	HAL_FLASHEx_DATAEEPROM_Unlock();
}

//------------------------------------------------------------------------------
static void eeprom_lock()
{
	HAL_FLASHEx_DATAEEPROM_Lock();
}

//------------------------------------------------------------------------------
static void eeprom_clear()
{
	eeprom_unlock();
	for (uint32_t addr = DATA_EEPROM_BASE; addr < DATA_EEPROM_END; addr = addr+4)
	{
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, addr, 0);
	}
	eeprom_lock();
}

//------------------------------------------------------------------------------
static bool eeprom_read(const uint32_t addr, uint8_t* data, const uint32_t data_size)
{
	eeprom_unlock();
	for (uint32_t i = 0; i < data_size; ++i)
	{
		const uint32_t eeprom_addr = addr + i;
		if (eeprom_addr < DATA_EEPROM_BASE || eeprom_addr > DATA_EEPROM_END)
		{
			return false;
		}
		data[i] = *(__IO uint8_t*)eeprom_addr;
	}
	eeprom_lock();
	return true;
}

//------------------------------------------------------------------------------
static bool eeprom_write(const uint32_t addr, const uint8_t* data, const uint32_t data_size)
{
	HAL_StatusTypeDef status;
	eeprom_unlock();
	for (uint32_t i = 0; i < data_size; ++i)
	{
		const uint32_t eeprom_addr = addr + i;
		if (eeprom_addr < DATA_EEPROM_BASE || eeprom_addr > DATA_EEPROM_END)
		{
			return false;
		}
		status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_BYTE, eeprom_addr , data[i]);

		if (status != HAL_OK)
		{
			eeprom_lock();
			return false;
		}
	}
	eeprom_lock();
	return true;
}

//------------------------------------------------------------------------------
