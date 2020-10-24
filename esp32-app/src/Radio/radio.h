#ifndef __RADIO_H__
#define __RADIO_H__

#include "../SX1278/platform/arduino/sx1278-arduino.h"


class Radio
{

public:
    Radio();

private:
    static void on_tx_done(void *args);
    static void on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
    static void on_tx_timeout(void *args);
    static void on_rx_timeout(void *args);
    static void on_rx_error(void *args);

private:
    SPIClass spi;
    sx1278 dev;
    sx1278_arduino arduino_dev;
};


#endif // __RADIO_H__