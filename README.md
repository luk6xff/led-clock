# led-clock
LED clock with some fancy features.

## Details
The clock is configurable over web inteface. The time is provided by extremaly accurate RTC DS3231. It can be also synchronized with NTP server.
It receives and display meteo data from external LORA sensor.
You can also configure [openweather map API](https://openweathermap.org/) to get and display latest weather forecast for your city.
The clock UI supports two languages currently: polish and english.
You can check below the videos to see how it works in action:

* [![video1](https://img.youtube.com/vi/KoFvoKO0Jg0/0.jpg)](https://www.youtube.com/watch?v=KoFvoKO0Jg0)
* [![video1](https://img.youtube.com/vi/WqVvntWIiOE/0.jpg)](https://www.youtube.com/watch?v=WqVvntWIiOE)

## Some pics
### Device
* Printed Case:
![](docs/pics/dev-case1.jpg)
![](docs/pics/dev-case2.jpg)
![](docs/pics/dev-case3.jpg)
![](docs/pics/dev-case4.jpg)

* LoRa meteo sensor:
![](docs/pics/dev-sensor7.jpg)
![](docs/pics/dev-sensor12.jpg)
![](docs/pics/dev-sensor11.jpg)
![](docs/pics/dev-sensor15.jpg)

* Custom Case:
![](docs/pics/dev-custom-case1.jpg)
![](docs/pics/dev-custom-case2.jpg)
![](docs/pics/dev-custom-case3.jpg)

* Prototype:
![](case/images/back.png)
![](case/images/front1.png)
![](case/images/front2.png)
![](case/images/back_1.png)
![](case/images/front_1.png)
![](case/images/front_2.png)
![](docs/pics/dev-case-raw1.jpg)
![](docs/pics/dev-case-raw2.jpg)
![](docs/pics/dev-prototype.jpg)

### Web app
![](docs/pics/web-app-info-ui.png)
![](docs/pics/web-config-ui.png)
![](docs/pics/web-config-ui2.png)
![](docs/pics/web-ota-ui.png)
![](docs/pics/web-ui.png)

### Schematics
* Clock:
![](docs/schematics/clock/led-clock_schematic.png)
* LoRa external sensor:
![](docs/schematics/sensor/ledclock-sensor_schematic.png)

## Parts used
All the modules used in the project are listed [here](./docs/BOM.md).


## The project contains of 2 subprojects:

### [esp32-app](./esp32-app/README.md)
Main clock application based on esp32 wroom device. README here: [esp32-app-readme](./esp32-app/README.md)

### [sensor-app](./sensor-app/README.md)
Sensor board application based on STM32L031F6Px. README here: [sensor-app-readme](./sensor-app/README.md)
