#include "light_sensor.h"
#include "hw_config.h"

//-----------------------------------------------------------------------------
LightSensor::LightSensor()
{
    dev.adc_max_value = 4096;
    dev.adc_samples_num = 8;
    dev.vdd_value = 3.3;
    dev_arduino.analog_gpio_pin = TEMT600_ANALOG_PIN;
    temt6000_arduino_init(&dev, &dev_arduino);
}

//-----------------------------------------------------------------------------
float LightSensor::getIlluminace()
{
    return temt6000_get_light(&dev);
}

//-----------------------------------------------------------------------------
