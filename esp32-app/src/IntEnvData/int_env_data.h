#ifndef __INTERNAL_ENV_DATA_H__
#define __INTERNAL_ENV_DATA_H__

#include "Config/InternalEnvironmentDataConfigParam.h"

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