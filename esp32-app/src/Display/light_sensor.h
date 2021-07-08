#pragma once

#include "bh1750-arduino.h"

class LightSensor
{

public:
    LightSensor();
    void init();
    void deinit();
    float getIlluminance();

private:
    bh1750 bh1750_dev;
    bh1750_arduino bh1750_dev_arduino;
};