#pragma once

#include "bme280-arduino.h"
#include "int_env_data.h"

class InternalEnvironmentDataSensor
{

public:
    InternalEnvironmentDataSensor();
    void init();
    void deinit();
    bool getData(InternalEnvData& data);

private:
    bme280 dev;
    bme280_arduino dev_arduino;
};
