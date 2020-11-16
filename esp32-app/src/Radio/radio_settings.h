#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>
#include "App/utils.h"


#define RADIO_CFG_KEY                   "dev-cfg-radio"
#define RADIO_CFG_VAL_UPDATE_INTERVAL   "radio-update-interval"
#define RADIO_CFG_VAL_CRIT_VBATT        "radio-crit-vbatt"

/**
 * @brief RadiSensor global settings structure
 */
struct RadioSensorSettings
{
    std::string toJson()
    {
        StaticJsonDocument<256> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(RADIO_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[RADIO_CFG_VAL_UPDATE_INTERVAL] = crit_vbatt_level;
        obj = arr.createNestedObject();
        obj[RADIO_CFG_VAL_CRIT_VBATT] = update_data_interval;
        serializeJson(doc, json);
        return json;
    }

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[RADIO_CFG_KEY].as<JsonArray>();
        for (const auto& v : arr)
        {
            if (v[RADIO_CFG_VAL_UPDATE_INTERVAL])
            {
                update_data_interval = v[RADIO_CFG_VAL_UPDATE_INTERVAL].as<uint32_t>();
            }
            else if (v[RADIO_CFG_VAL_CRIT_VBATT])
            {
                crit_vbatt_level = v[RADIO_CFG_VAL_CRIT_VBATT].as<uint32_t>();
            }
        }
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return String(RADIO_CFG_KEY)+colon+comma + \
                String(RADIO_CFG_VAL_UPDATE_INTERVAL)+colon+String(update_data_interval)+comma + \
                String(RADIO_CFG_VAL_CRIT_VBATT)+colon+String(crit_vbatt_level);
    }


    uint32_t update_data_interval; //[s] in seconds
    uint32_t crit_vbatt_level;     //[mV]
};