#include "light_sensor.h"
#include "hw_config.h"

//-----------------------------------------------------------------------------
LightSensor::LightSensor()
{
    temt6000_dev.adc_max_value = 4096;
    temt6000_dev.adc_samples_num = 8;
    temt6000_dev.vdd_value = 3.3;
    temt6000_dev_arduino.analog_gpio_pin = TEMT600_ANALOG_PIN;
    temt6000_arduino_init(&temt6000_dev, &temt6000_dev_arduino);

    max44009_dev_arduino.i2c = &Wire;
    max44009_dev.i2c_addr = MAX44009_I2C_ADDR;
    max44009_arduino_init(&max44009_dev, &max44009_dev_arduino);

}

//-----------------------------------------------------------------------------
float LightSensor::getIlluminace()
{
    //return temt6000_get_light(&temt6000_dev);
    float lux;
    max44009_read_lux_intenity(&max44009_dev, &lux);
    return lux;
}

//-----------------------------------------------------------------------------
