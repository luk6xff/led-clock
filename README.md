# led-clock
LED clock with some fancy features.

## Details
The clock is configurable via web inteface. The time is driven by extremaly accurate RTC: DS3231. It can be also synchronized with NTP.
It receives and display meteo data from external LORA sensor.

## Some pics
Web app
![1](docs/pics/app-info-ui.png)
![2](docs/pics/config-ui.png)
![3](docs/pics/config-ui2.png)
![4](docs/pics/ota-ui.png)
![5](docs/pics/web-ui.png)

Schematics
* Clock:
![1](docs/schematics/clock/led-clock_schematic.png)
* LoRa external sensor:
![2](docs/schematics/sensor/ledclock-sensor_schematic.png)

## Parts used
All the modules used in the project are listed [here](./docs/BOM.md).


## The project contains of 2 subprojects:

### [esp32-app](./esp32-app/README.md)
Main clock application based on esp32 wroom device. README here: [esp32-app-readme](./esp32-app/README.md)

### [sensor-app](./sensor-app/README.md)
Sensor board application based on STM32L031F6Px. README here: [sensor-app-readme](./sensor-app/README.md)
