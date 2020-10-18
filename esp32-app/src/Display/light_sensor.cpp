#include "light_sensor.h"
#include "hw_config.h"
#include "App/rtos_common.h"

//-----------------------------------------------------------------------------
LightSensor::LightSensor()
{
    rtos::LockGuard<rtos::Mutex> lock(g_i2cMutex);
    bh1750_dev_arduino.i2c = &Wire;
    bh1750_dev.i2c_addr = BH1750_I2C_ADDRESS1;
    bh1750_arduino_init(&bh1750_dev, &bh1750_dev_arduino);
    bh1750_set_state(&bh1750_dev, BH1750_STATE_POWER_ON);
    bh1750_set_mode(&bh1750_dev, BH1750_MODE_CONT_HIGH_RES);
}

//-----------------------------------------------------------------------------
float LightSensor::getIlluminance()
{
    rtos::LockGuard<rtos::Mutex> lock(g_i2cMutex);
    float lux;
    bh1750_get_lux(&bh1750_dev, &lux);
    return lux;
}

//-----------------------------------------------------------------------------
