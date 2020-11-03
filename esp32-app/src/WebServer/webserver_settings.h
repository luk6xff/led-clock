#pragma once

#include <Arduino.h>
#include <cstdint>
#include <cstring>


struct WebServerSettings
{
    #define HOST_NAME_LEN       16



    WebServerSettings()
    {

        memset(hostName, 0, HOST_NAME_LEN);
    }


    String toStr()
    {
        return "hostName:" + String(hostName);
    }


    char hostName[HOST_NAME_LEN];
};