#pragma once
#include "temt6000-arduino.h"
#include "max44009-arduino.h"

class LightSensor
{

public:
    LightSensor();
    float getIlluminace();

private:
    temt6000 temt6000_dev;
    temt6000_arduino temt6000_dev_arduino;

    max44009 max44009_dev;
    max44009_arduino max44009_dev_arduino;
};