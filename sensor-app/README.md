# esp32-app
Sensor board application based on STM32L031F6Px. It collects environment values like temperaature, pressure and humidity and sends them to the clock with LoRa communication protocol.

## HARDWARE
* Microcontroller: STM32L031F6Px

## BUILD INSTRUCTIONS
* `STM32CubeIDE ver.1.3.0` with `STM32Cube FW_L0 V1.11.2` is used for development: https://www.st.com/en/development-tools/stm32cubeide.html
* STM32Cube FW_L0 V1.11.2
* Before a first build, please change all the paths `/home/luk6xff/Projects/led-clock/` in files: `.cproject` , `.mxproject` , `.project`.
* Press `Build Release` button (hammer icon) in IDE to compile a project.
* Press `Run` button (play icon) in IDE to flash a uC.
