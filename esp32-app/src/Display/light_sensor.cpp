#include "light_sensor.h"
#include "hw_config.h"
#include "App/rtos_utils.h"

//-----------------------------------------------------------------------------
LightSensor::LightSensor()
{
    bh1750_dev_arduino.i2c = &Wire1;
    bh1750_dev_arduino.sda_pin = BH1750_SDA_PIN;
    bh1750_dev_arduino.scl_pin = BH1750_SCL_PIN;
    bh1750_dev.i2c_addr = BH1750_I2C_ADDRESS1;

    // Not needed, So far only BH1750 uses Wire1
    //rtos::LockGuard<rtos::Mutex> lock(g_i2c1Mutex);
    bh1750_arduino_init(&bh1750_dev, &bh1750_dev_arduino);
}

//-----------------------------------------------------------------------------
void LightSensor::init()
{
    bh1750_set_state(&bh1750_dev, BH1750_STATE_POWER_ON);
    bh1750_set_mode(&bh1750_dev, BH1750_MODE_CONT_HIGH_RES);
}

//-----------------------------------------------------------------------------
void LightSensor::deinit()
{
    bh1750_set_state(&bh1750_dev, BH1750_STATE_POWER_DOWN);
}

//-----------------------------------------------------------------------------
float LightSensor::getIlluminance()
{
    //rtos::LockGuard<rtos::Mutex> lock(g_i2c1Mutex);
    float lux;
    bh1750_get_lux(&bh1750_dev, &lux);
    return lux;
}

//-----------------------------------------------------------------------------
