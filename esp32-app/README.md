# esp32-app
Main led-clock application based on esp32 wroom device.

## Details
TODO

## Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[Core:5.0.1, Home:3.3.0])


## Building and flashing
```sh
# Build an application
platformio run

# Build filesystem
platformio run --target buildfs --environment esp32dev

# Upload filesystem image
platformio run --target uploadfs --environment esp32dev

# Upload the application binary
platformio run --target upload

# All in one-liner
platformio run  &&  platformio run --target buildfs --environment esp32dev  &&  platformio run --target uploadfs --environment esp32dev  &&  platformio run --target upload
```

## HARDWARE
* https://docs.platformio.org/en/latest/platforms/espressif32.html
* https://docs.platformio.org/en/latest/boards/espressif32/esp32dev.html#board-espressif32-esp32dev

## Pinout
* HW pinout defined at: `src/hw_config.h`
