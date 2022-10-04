#include "int_env_data_sensor.h"
#include "hw_config.h"
#include "App/rtos_utils.h"

//-----------------------------------------------------------------------------
InternalEnvironmentDataSensor::InternalEnvironmentDataSensor()
{
    dev_arduino.i2c.hndl = &Wire1;
    dev_arduino.i2c.sda_pin = BME280_SDA_PIN;
    dev_arduino.i2c.scl_pin = BME280_SCL_PIN;
    dev.i2c_addr = BME280_I2C_ADDR;
}

//-----------------------------------------------------------------------------
void InternalEnvironmentDataSensor::init()
{
    bme280_arduino_init(&dev, &dev_arduino);
}

//-----------------------------------------------------------------------------
void InternalEnvironmentDataSensor::deinit()
{
    bme280_arduino_deinit(&dev);
}

//-----------------------------------------------------------------------------
bool InternalEnvironmentDataSensor::getData(InternalEnvData& data)
{
    return bme280_read_data(&dev, &data.temperature, &data.pressure, &data.humidity);
}

//-----------------------------------------------------------------------------
