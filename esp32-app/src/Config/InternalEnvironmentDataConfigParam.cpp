#include "InternalEnvironmentDataConfigParam.h"


//------------------------------------------------------------------------------
InternalEnvironmentDataConfigParam::InternalEnvironmentDataConfigParam() : ConfigParam(TIME_CFG_KEY, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool InternalEnvironmentDataConfigParam::setConfig(const JsonObject& json)
{
    InternalEnvironmentDataConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        log::dbg("InternalEnvironmentDataConfigParam::unpackFromJson - SUCCESS");
        if (m_cfgHndl.getCurrent().time != dataFromServer)
        {
            log::dbg("Storing new InternalEnvironmentDataConfigData settings");
            return m_cfgHndl.save(cfg);
        }
        log::dbg("Storing InternalEnvironmentDataConfigData settings skipped, no change detected");
        return false;
    }

    log::err("InternalEnvironmentDataConfigParam::unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
void InternalEnvironmentDataConfigParam::getConfig(String& configPayload)
{
    // Extract config data from application
    configPayload = packToJson(m_cfgHndl.getCurrent().time);
}

//------------------------------------------------------------------------------
String InternalEnvironmentDataConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
            cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_ENABLED)+colon+String(m_cfgData.dataUpdateInterval)+comma + \
            cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_UPDATE_INTERVAL)+colon+String(m_cfgData.critVbattLevel)+comma  + \
            cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_MSG_DISP_REPEAT_NUM)+colon+String(m_cfgData.msgDispRepeatNum);
}

//------------------------------------------------------------------------------
void InternalEnvironmentDataConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_ENABLED,           "intenv-data-enabled" },
        { InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_UPDATE_INTERVAL,   "intenv-update-interval" },
        { InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_MSG_DISP_REPEAT_NUM,    "intenv-msg-disp-repeat-num" },
    };
}

//------------------------------------------------------------------------------
bool InternalEnvironmentDataConfigParam::unpackFromJson(InternalEnvironmentDataConfigData& cfgData, const JsonObject& json)
{
    // Not used so far
    bool ret = true;

    JsonArray arr = json[key()].as<JsonArray>();
    for (const auto& v : arr)
    {
        if (v[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_ENABLED)])
        {
            cfgData.enabled = v[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_ENABLED)].as<bool>();
        }
        else if (v[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_UPDATE_INTERVAL)])
        {
            cfgData.dataUpdateInterval = v[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_UPDATE_INTERVAL)].as<uint32_t>();
            if (cfgData.dataUpdateInterval == 0)
            {
                cfgData.dataUpdateInterval = 1;
            }
            if (cfgData.dataUpdateInterval > 3600)
            {
                cfgData.dataUpdateInterval = 3600;
            }
            // Convert from minutes on server to seconds
            cfgData.dataUpdateInterval = cfgData.dataUpdateInterval * 60;
        }
        else if (v[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_MSG_DISP_REPEAT_NUM)])
        {
            cfgData.msgDispRepeatNum = v[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_MSG_DISP_REPEAT_NUM)].as<uint32_t>();
            if ((cfgData.msgDispRepeatNum) > 5)
            {
                cfgData.msgDispRepeatNum = 5;
            }
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
String InternalEnvironmentDataConfigParam::packToJson(const InternalEnvironmentDataConfigData& data)
{
    StaticJsonDocument<1024> doc;
    String json;
    JsonArray arr = doc.createNestedArray(key());
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_ENABLED)] = data.enabled;
    obj = arr.createNestedObject();
    obj[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_DATA_UPDATE_INTERVAL)] = data.dataUpdateInterval / 60; // Convert from seconds to minutes on server.
    obj = arr.createNestedObject();
    obj[cfgParamKey(InternalEnvironmentDataKeys::INTERNAL_ENVIRONMENT_MSG_DISP_REPEAT_NUM)] = data.msgDispRepeatNum;
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
