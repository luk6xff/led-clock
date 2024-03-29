# esp32-app
Main led-clock application based on esp32 wroom device.

## Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[Core:5.0.1, Home:3.3.0])
* (Platform Espressif32 version 2.0.0)[https://github.com/platformio/platform-espressif32/releases/tag/v2.0.0]

## PIO tool path
If `pio` tool not visible globally, add:
```
alias pio=~/.platformio/penv/bin/pio
```


## Building and flashing
```sh
# Build an application
pio run --environment esp32dev

# Upload the application binary
pio run --target upload  --environment esp32dev

# Build filesystem
pio run --target buildfs --environment esp32dev

# Upload filesystem image
pio run --target uploadfs --environment esp32dev


# All in one-liner
pio run --environment esp32dev  &&  pio run --target upload --environment esp32dev  &&  pio run --target buildfs --environment esp32dev  &&  pio run --target uploadfs --environment esp32dev
```

## Running unit tests
```
pio test --environment native
```

## Debug monitor
```
pio device monitor -f esp32_exception_decoder -b 115200 -p /dev/ttyUSB1
```

## HARDWARE
* https://docs.platformio.org/en/latest/platforms/espressif32.html
* https://docs.platformio.org/en/latest/boards/espressif32/esp32dev.html#board-espressif32-esp32dev

## Pinout
* HW pinout defined at: `src/hw_config.h`
