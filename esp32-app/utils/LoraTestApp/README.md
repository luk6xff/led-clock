# LORA_SENSOR_TEST_APP
Lora Sensor test app

## Details
Emulates a sensor-app built on STM32, behaving the same way but in ESP8266 WEMOS D1_MINI board.

## Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[Core:5.0.1, Home:3.3.0])


## Building and flashing
```sh
# Build an application and upload the application binary
platformio run  && platformio run --target upload
```

## Debug monitor
```
platformio device monitor -f esp8266_exception_decoder -b 9600 -p /dev/ttyUSB1
```

## Pinout
* HW pinout defined at: `src/hw-config.h`
