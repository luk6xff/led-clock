#ifndef __INTERNAL_ENV_DATA_H__
#define __INTERNAL_ENV_DATA_H__

#include "int_env_data_settings.h"

/**
 * @brief Internal measured data structure
 */
typedef struct
{
	float temperature;
	float pressure;
	float humidity;
} InternalEnvData;



#endif // __INTERNAL_ENV_DATA_H__