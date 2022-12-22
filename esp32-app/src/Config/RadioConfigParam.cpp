#include "RadioConfigParam.h"
#include "Rtos/logger.h"
#include <cstring>

//------------------------------------------------------------------------------
RadioConfigParam::RadioConfigParam() : ConfigParam(RADIO_CFG_KEY, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool RadioConfigParam::setConfig(const JsonObject& json)
{
    RadioConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        logger::dbg("RadioConfigParam::unpackFromJson - SUCCESS");
        if (std::memcmp(&(m_cfgHndl.getCurrent().radio), &dataFromServer, sizeof(dataFromServer)) != 0)
        {
            logger::dbg("Storing new RadioConfigData settings");
            return m_cfgHndl.save(dataFromServer);
        }
        logger::dbg("Storing RadioConfigData settings skipped, no change detected");
        return false;
    }

    logger::err("RadioConfigParam::unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
void RadioConfigParam::getConfig(String& configPayload)
{
    // Extract config data from application
    configPayload = packToJson(m_cfgHndl.getCurrent().radio);
}

//------------------------------------------------------------------------------
String RadioConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
            cfgParamKey(RadioKeys::RADIO_ENABLED)+colon+String(m_cfgData.enabled)+comma + \
            cfgParamKey(RadioKeys::RADIO_DATA_UPDATE_INTERVAL)+colon+String(m_cfgData.dataUpdateInterval)+comma + \
            cfgParamKey(RadioKeys::RADIO_CRITICAL_VBATT)+colon+String(m_cfgData.critVbattLevel)+comma  + \
            cfgParamKey(RadioKeys::RADIO_MSG_DISP_REPEAT_NUM)+colon+String(m_cfgData.msgDispRepeatNum)+comma  + \
            cfgParamKey(RadioKeys::RADIO_LAST_MSG_DISP_EVERY_MINUTES)+colon+String(m_cfgData.lastMsgDispEveryMinutes);
}

//------------------------------------------------------------------------------
void RadioConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { RadioKeys::RADIO_ENABLED,                     "radio-enabled" },
        { RadioKeys::RADIO_DATA_UPDATE_INTERVAL,        "radio-update-interval" },
        { RadioKeys::RADIO_CRITICAL_VBATT,              "radio-crit-vbatt" },
        { RadioKeys::RADIO_MSG_DISP_REPEAT_NUM,         "radio-msg-disp-repeat-num" },
        { RadioKeys::RADIO_LAST_MSG_DISP_EVERY_MINUTES, "radio-last-msg-disp-every-minutes" },
    };
}

//------------------------------------------------------------------------------
bool RadioConfigParam::unpackFromJson(RadioConfigData& cfgData, const JsonObject& json)
{
    // Not used so far
    bool ret = true;

    JsonArray arr = json[key()].as<JsonArray>();
    for (const auto& v : arr)
    {
        if (v[cfgParamKey(RadioKeys::RADIO_ENABLED)])
        {
            cfgData.enabled = v[cfgParamKey(RadioKeys::RADIO_ENABLED)].as<bool>();
        }
        else if (v[cfgParamKey(RadioKeys::RADIO_DATA_UPDATE_INTERVAL)])
        {
            cfgData.dataUpdateInterval = v[cfgParamKey(RadioKeys::RADIO_DATA_UPDATE_INTERVAL)].as<uint32_t>();
            if (cfgData.dataUpdateInterval == 0)
            {
                cfgData.dataUpdateInterval = 1;
            }
            // Convert from minutes on server to seconds
            cfgData.dataUpdateInterval = cfgData.dataUpdateInterval * 60;
        }
        else if (v[cfgParamKey(RadioKeys::RADIO_CRITICAL_VBATT)])
        {
            cfgData.critVbattLevel = v[cfgParamKey(RadioKeys::RADIO_CRITICAL_VBATT)].as<uint32_t>();
        }
        else if (v[cfgParamKey(RadioKeys::RADIO_MSG_DISP_REPEAT_NUM)])
        {
            cfgData.msgDispRepeatNum = v[cfgParamKey(RadioKeys::RADIO_MSG_DISP_REPEAT_NUM)].as<uint32_t>();
        }
        else if (v[cfgParamKey(RadioKeys::RADIO_LAST_MSG_DISP_EVERY_MINUTES)])
        {
            cfgData.lastMsgDispEveryMinutes = v[cfgParamKey(RadioKeys::RADIO_LAST_MSG_DISP_EVERY_MINUTES)].as<uint32_t>();
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
String RadioConfigParam::packToJson(const RadioConfigData& data)
{
    StaticJsonDocument<1024> doc;
    String json;
    JsonArray arr = doc.createNestedArray(key());
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(RadioKeys::RADIO_ENABLED)] = data.enabled;
    obj = arr.createNestedObject();
    obj[cfgParamKey(RadioKeys::RADIO_DATA_UPDATE_INTERVAL)] = data.dataUpdateInterval / 60; // Convert from seconds to minutes on server.
    obj = arr.createNestedObject();
    obj[cfgParamKey(RadioKeys::RADIO_CRITICAL_VBATT)] = data.critVbattLevel;
    obj = arr.createNestedObject();
    obj[cfgParamKey(RadioKeys::RADIO_MSG_DISP_REPEAT_NUM)] = data.msgDispRepeatNum;
    obj = arr.createNestedObject();
    obj[cfgParamKey(RadioKeys::RADIO_LAST_MSG_DISP_EVERY_MINUTES)] = data.lastMsgDispEveryMinutes;
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
