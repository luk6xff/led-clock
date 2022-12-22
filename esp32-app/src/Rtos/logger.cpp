#include "logger.h"

#define DBG_BAUDRATE 115200
#define DBG_VERBOSITY_LEVEL DBG_VERBOSE

//------------------------------------------------------------------------------
void logger::init()
{
    Serial.begin(DBG_BAUDRATE);
    Debug.setDebugOutputStream(&Serial);
    Debug.setDebugLevel(DBG_VERBOSITY_LEVEL);
    Debug.timestampOff();
    logger::inf(">>> LED-CLOCK by luk6xff 2020 <<<\r\n");
}

//------------------------------------------------------------------------------

