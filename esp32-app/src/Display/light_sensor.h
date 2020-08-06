#pragma once
#include "temt6000-arduino.h"

class LightSensor
{

public:
    LightSensor();
    float getIlluminace();

private:
    temt6000 dev;
    temt6000_arduino dev_arduino;
};