#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>
#include "App/utils.h"


#define RADIO_CFG_KEY                               "dev-cfg-radio"
#define RADIO_CFG_VAL_UPDATE_INTERVAL               "radio-update-interval"
#define RADIO_CFG_VAL_CRIT_VBATT                    "radio-crit-vbatt"
#define RADIO_CFG_VAL_MSG_DISP_REPEAT_NUM           "radio-msg-disp-repeat-num"
#define RADIO_CFG_VAL_LAST_MSG_DISP_EVERY_MINUTES   "radio-last-msg-disp-every-minutes"

/**
 * @brief RadiSensor global settings structure
 */
struct RadioSensorSettings
{
    std::string toJson()
    {
        StaticJsonDocument<512> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(RADIO_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[RADIO_CFG_VAL_UPDATE_INTERVAL] = update_data_interval / 60; // Convert from seconds to minutes on server.
        obj = arr.createNestedObject();
        obj[RADIO_CFG_VAL_CRIT_VBATT] = crit_vbatt_level;
        obj = arr.createNestedObject();
        obj[RADIO_CFG_VAL_MSG_DISP_REPEAT_NUM] = msg_disp_repeat_num;
        obj = arr.createNestedObject();
        obj[RADIO_CFG_VAL_LAST_MSG_DISP_EVERY_MINUTES] = last_msg_disp_every_minutes;
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
                if (update_data_interval == 0)
                {
                    update_data_interval = 1;
                }
                // Convert from minutes on server to seconds
                update_data_interval = update_data_interval * 60;
            }
            else if (v[RADIO_CFG_VAL_CRIT_VBATT])
            {
                crit_vbatt_level = v[RADIO_CFG_VAL_CRIT_VBATT].as<uint32_t>();
            }
            else if (v[RADIO_CFG_VAL_MSG_DISP_REPEAT_NUM])
            {
                msg_disp_repeat_num = v[RADIO_CFG_VAL_MSG_DISP_REPEAT_NUM].as<uint32_t>();
                if ((msg_disp_repeat_num) > 5)
                {
                    msg_disp_repeat_num = 5;
                }
            }
            else if (v[RADIO_CFG_VAL_LAST_MSG_DISP_EVERY_MINUTES])
            {
                last_msg_disp_every_minutes = v[RADIO_CFG_VAL_LAST_MSG_DISP_EVERY_MINUTES].as<uint32_t>();
            }
        }
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return String(RADIO_CFG_KEY)+colon+comma + \
                String(RADIO_CFG_VAL_UPDATE_INTERVAL)+colon+String(update_data_interval)+comma + \
                String(RADIO_CFG_VAL_CRIT_VBATT)+colon+String(crit_vbatt_level)+comma + \
                String(RADIO_CFG_VAL_MSG_DISP_REPEAT_NUM)+colon+String(msg_disp_repeat_num)+comma + \
                String(RADIO_CFG_VAL_LAST_MSG_DISP_EVERY_MINUTES)+colon+String(last_msg_disp_every_minutes);
    }


    uint32_t update_data_interval; // [s] in seconds
    uint32_t crit_vbatt_level;     // [mV]
    uint32_t msg_disp_repeat_num;  // How many times incoming message shall be displayed
    uint32_t last_msg_disp_every_minutes; // How often shall be last sensor msg displayed in minutes [min]
};