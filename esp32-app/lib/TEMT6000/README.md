# TEMT6000 RTC module library

## Description
TEMT6000 ambient light sensorplatform independent library

## Usage
Simple demo code how to use the library on [ARDUINO] platform
```cpp

#include "TEMT6000/platform/temt6000-arduino.h"

// PINOUT FOR ESP32 WROOM DEVBOARD
// TEMT6000
#define TEMT6000_I2C_ADDR         0x68
#define TEMT6000_SDA              PB_7
#define TEMT6000_SCL              PB_6
int main(void)
{
    temt6000_mbed mbed_dev =
    {
        .i2c = &i2c,
    };

    temt6000 dev =
    {
        .i2c_addr = TEMT6000_I2C_ADDR;
    };
    temt6000_mbed_init(dev, mbed_dev);

    return 0;
}
```

## Porting to different platforms
If you want to port this library on other platform, the only thing you have to do is to implement HW/Platform dependent functions from `temt6000.h` file as it is done for example for [ARDUINO] platform in `platform/temt6000-arduino.cpp` file.


## Author
* Lukasz Uszko aka `luk6xff` [lukasz.uszko@gmail.com]