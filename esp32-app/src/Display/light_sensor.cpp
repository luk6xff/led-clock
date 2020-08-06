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

    temt6000_arduino arduino_dev1 =
    {
        TEMT600_ANALOG_PIN, // analog_gpio_pin
    };

    temt6000 dev1 =
    {
        3.3f, // vdd_value
        4096, // adc_max_value
        8,    // adc_samples_num
    };
}

//-----------------------------------------------------------------------------
float LightSensor::getIlluminace()
{
    return temt6000_get_light(&dev);
}

//-----------------------------------------------------------------------------
