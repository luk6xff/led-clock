# BMP180 digital pressure sensor library

## Description
BMP180 digital pressure sensor platform independent library

## Usage
Simple demo code how to use the library on [MBED] platform
```cpp

#include "BMP180/platform/bmp180-mbed.h"

// PINOUT FOR MBED BOARD_DISCO_L476VG
// BMP180
#define BMP180_I2C_ADDR         0x68
#define BMP180_SDA              PB_7
#define BMP180_SCL              PB_6
int main(void)
{
    I2C i2c(BMP180_SDA, BMP180_SCL);
    bmp180_mbed mbed_dev =
    {
        .i2c = &i2c,
    };

    bmp180 dev =
    {
        .i2c_addr = BMP180_I2C_ADDR;
    };
    bmp180_mbed_init(dev, mbed_dev);

    bmp180_time_t     time;
    bmp180_calendar_t cal;

    if (bmp180_get_time(&time) != 0)
    {
        return -1;
    }

    if (bmp180_get_calendar(&cal) != 0)
    {
        return -1;
    }

    printf("%d-%d-%d, %d:%d:%d", cal.year, cal.month, cal.date time.hours, time.minutes, time.seconds);
    return 0;
}
```

## Porting to different platforms
If you want to port this library on other platform, the only thing you have to do is to implement HW/Platform dependent functions from `bmp180.h` file as it is done for example for [MBED] platform in `platform/bmp180-mbed.cpp` file.


## Author
* Lukasz Uszko aka `luk6xff` [lukasz.uszko@gmail.com]