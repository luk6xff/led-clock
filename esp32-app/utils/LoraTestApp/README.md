# usbmux
Energy meter probe gateway

## Details
TODO

## Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[Core:5.0.1, Home:3.3.0])


## Building and flashing
```sh
# Build an application
platformio run

# Build filesystem
platformio run --target buildfs --environment d1_mini

# Upload filesystem image
platformio run --target uploadfs --environment d1_mini

# Upload the application binary
platformio run --target upload

# All in one-liner
platformio run  &&  platformio run --target buildfs --environment d1_mini  &&  platformio run --target uploadfs --environment d1_mini  &&  platformio run --target upload
```

## Pinout
* HW pinout defined at: `src/Common/hw-config.h`


## More random pics

## BOM

| PART NAME                          | SHOP LINK                                                                                        | DATASHEET | QUANTITY | PRICE [PLN] |
|------------------------------------|--------------------------------------------------------------------------------------------------|-----------|----------|-------------|
| ESP8266 WEMOS D1-MINI | [AliExpress](https://aliexpress.com/item/32845061455.html)|[ESP8266-D1MINI](./https://docs.wemos.cc/en/latest/d1/d1_mini.html) | 1 | 7.99          |