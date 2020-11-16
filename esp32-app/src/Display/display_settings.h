#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>
#include "App/utils.h"


#define DISPLAY_CFG_KEY                 "dev-cfg-display"
#define DISPLAY_CFG_VAL_AUTO_INTENSITY  "display-auto-intensity"
#define DISPLAY_CFG_VAL_INTENSITY_VALUE "display-intensity-val"
#define DISPLAY_CFG_VAL_ANIM_SPEED      "display-anim-speed"
#define DISPLAY_CFG_VAL_TIME_FORMAT     "display-time-format"


struct DisplaySettings
{
    DisplaySettings()
        : enableAutoIntenisty(true)
        , intensityValue(0)
        , animSpeed(70)
        , timeFormat(1)
    {

    }

    DisplaySettings(bool enableOnOff, uint8_t intensityValue,
                    uint8_t animSpeed,  uint8_t timeFormat)
        : enableAutoIntenisty(enableOnOff)
        , intensityValue(intensityValue)
        , animSpeed(animSpeed)
        , timeFormat(timeFormat)
    {

    }

    std::string toJson()
    {
        StaticJsonDocument<512> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(DISPLAY_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[DISPLAY_CFG_VAL_AUTO_INTENSITY] = enableAutoIntenisty;
        obj = arr.createNestedObject();
        obj[DISPLAY_CFG_VAL_INTENSITY_VALUE] = intensityValue;
        obj = arr.createNestedObject();
        obj[DISPLAY_CFG_VAL_ANIM_SPEED] = animSpeed;
        obj = arr.createNestedObject();
        obj[DISPLAY_CFG_VAL_TIME_FORMAT] = timeFormat;
        serializeJson(doc, json);
        return json;
    }

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[DISPLAY_CFG_KEY].as<JsonArray>();
        for (const auto& v : arr)
        {
            if (v[DISPLAY_CFG_VAL_AUTO_INTENSITY])
            {
                enableAutoIntenisty = v[DISPLAY_CFG_VAL_AUTO_INTENSITY].as<uint8_t>();
            }
            else if (v[DISPLAY_CFG_VAL_INTENSITY_VALUE])
            {
                intensityValue = v[DISPLAY_CFG_VAL_INTENSITY_VALUE].as<uint8_t>();
            }
            else if (v[DISPLAY_CFG_VAL_ANIM_SPEED])
            {
                animSpeed = v[DISPLAY_CFG_VAL_ANIM_SPEED].as<uint8_t>();
            }
            else if (v[DISPLAY_CFG_VAL_TIME_FORMAT])
            {
                timeFormat = v[DISPLAY_CFG_VAL_TIME_FORMAT].as<uint8_t>();
            }
        }
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return String(DISPLAY_CFG_KEY)+colon+comma + \
                String(DISPLAY_CFG_VAL_AUTO_INTENSITY)+colon+String(enableAutoIntenisty)+comma + \
                String(DISPLAY_CFG_VAL_INTENSITY_VALUE)+colon+String(intensityValue)+comma  + \
                String(DISPLAY_CFG_VAL_ANIM_SPEED)+colon+String(animSpeed)+comma  + \
                String(DISPLAY_CFG_VAL_TIME_FORMAT)+colon+String(timeFormat);
    }


    uint8_t enableAutoIntenisty;
    uint8_t intensityValue;
    uint8_t animSpeed;
    uint8_t timeFormat; // [s] in seconds

};