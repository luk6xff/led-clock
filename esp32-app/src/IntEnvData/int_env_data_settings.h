#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>
#include "App/utils.h"


#define INTENV_CFG_KEY                               "dev-cfg-intenv"
#define INTENV_CFG_VAL_UPDATE_INTERVAL               "intenv-update-interval"
#define INTENV_CFG_VAL_MSG_DISP_REPEAT_NUM           "intenv-msg-disp-repeat-num"

/**
 * @brief InternalEnvData global settings structure
 */
struct InternalEnvDataSettings
{
    std::string toJson()
    {
        StaticJsonDocument<512> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(INTENV_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[INTENV_CFG_VAL_UPDATE_INTERVAL] = update_data_interval / 60; // Convert from seconds to minutes on server.
        obj = arr.createNestedObject();
        obj[INTENV_CFG_VAL_MSG_DISP_REPEAT_NUM] = msg_disp_repeat_num;
        serializeJson(doc, json);
        return json;
    }

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[INTENV_CFG_KEY].as<JsonArray>();
        for (const auto& v : arr)
        {
            if (v[INTENV_CFG_VAL_UPDATE_INTERVAL])
            {
                update_data_interval = v[INTENV_CFG_VAL_UPDATE_INTERVAL].as<uint32_t>();
                if (update_data_interval == 0)
                {
                    update_data_interval = 1;
                }
                // Convert from minutes on server to seconds
                update_data_interval = update_data_interval * 60;
            }
            else if (v[INTENV_CFG_VAL_MSG_DISP_REPEAT_NUM])
            {
                msg_disp_repeat_num = v[INTENV_CFG_VAL_MSG_DISP_REPEAT_NUM].as<uint32_t>();
                if ((msg_disp_repeat_num) > 5)
                {
                    msg_disp_repeat_num = 5;
                }
            }
        }
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return String(INTENV_CFG_KEY)+colon+comma + \
                String(INTENV_CFG_VAL_UPDATE_INTERVAL)+colon+String(update_data_interval)+comma + \
                String(INTENV_CFG_VAL_MSG_DISP_REPEAT_NUM)+colon+String(msg_disp_repeat_num);
    }


    uint32_t update_data_interval; // [s] in seconds
    uint32_t msg_disp_repeat_num;  // How many times incoming message shall be displayed
};