#pragma once

#include "display_config_param.h"


//------------------------------------------------------------------------------
DisplayConfigParam::DisplayConfigParam() : ConfigParam(WIFI_CFG_KEY, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool DisplayConfigParam::setConfig(const JsonObject& json)
{
    DisplayConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        log::dbg("unpackFromJson - SUCCESS");
        auto cfg = m_cfgHndl.getCurrent();
        cfg.display.enableAutoIntenisty = dataFromServer.enableAutoIntenisty;
        cfg.display.intensityValue      = dataFromServer.intensityValue;
        cfg.display.animSpeed           = dataFromServer.animSpeed;
        cfg.display.timeFormat          = dataFromServer.timeFormat;
        // Save cfg
        return m_cfgHndl.save(cfg);
    }

    log::err("unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
void DisplayConfigParam::getConfig(String& configPayload)
{
    // Extract config data from application
    DisplayConfigData cfgData;
    cfgData = m_cfgHndl.getCurrent().display;
    configPayload = packToJson(cfgData);
}

//------------------------------------------------------------------------------
String DisplayConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
        cfgParamKey(DisplayKeys::DISPLAY_AUTO_INTENSITY)+colon+String(m_cfgData.enableAutoIntenisty)+comma + \
        cfgParamKey(DisplayKeys::DISPLAY_INTENSITY_VALUE)+colon+String(m_cfgData.intensityValue)+comma  + \
        cfgParamKey(DisplayKeys::DISPLAY_ANIM_SPEED)+colon+String(m_cfgData.animSpeed)+comma  + \
        cfgParamKey(DisplayKeys::DISPLAY_TIME_FORMAT)+colon+String(m_cfgData.timeFormat);
}

//------------------------------------------------------------------------------
void DisplayConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { DisplayKeys::DISPLAY_AUTO_INTENSITY,  "display-auto-intensity" },
        { DisplayKeys::DISPLAY_INTENSITY_VALUE, "display-intensity-val" },
        { DisplayKeys::DISPLAY_ANIM_SPEED,      "display-anim-speed" },
        { DisplayKeys::DISPLAY_TIME_FORMAT,     "display-time-format" },
    };
}

//------------------------------------------------------------------------------
bool DisplayConfigParam::unpackFromJson(DisplayConfigData& cfgData, const JsonObject& json)
{
    // Not used so far
    bool ret = true;

    JsonArray arr = json[key()].as<JsonArray>();
    for (const auto& v : arr)
    {
        if (v[cfgParamKey(DisplayKeys::DISPLAY_AUTO_INTENSITY)])
        {
            cfgData.enableAutoIntenisty = v[cfgParamKey(DisplayKeys::DISPLAY_AUTO_INTENSITY)].as<uint8_t>();
        }
        else if (v[cfgParamKey(DisplayKeys::DISPLAY_INTENSITY_VALUE)])
        {
            cfgData.intensityValue = v[cfgParamKey(DisplayKeys::DISPLAY_INTENSITY_VALUE)].as<uint8_t>();
            if (cfgData.intensityValue > 15)
            {
                cfgData.intensityValue = 0;
            }
        }
        else if (v[cfgParamKey(DisplayKeys::DISPLAY_ANIM_SPEED)])
        {
            cfgData.animSpeed = v[cfgParamKey(DisplayKeys::DISPLAY_ANIM_SPEED)].as<uint8_t>();
            if (cfgData.animSpeed < 1 || cfgData.animSpeed > 1000)
            {
                cfgData.animSpeed = 50;
            }
        }
        else if (v[cfgParamKey(DisplayKeys::DISPLAY_TIME_FORMAT)])
        {
            cfgData.timeFormat = v[cfgParamKey(DisplayKeys::DISPLAY_TIME_FORMAT)].as<uint8_t>();
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
String DisplayConfigParam::packToJson(const DisplayConfigData& data)
{
    StaticJsonDocument<512> doc;
    String json;
    JsonArray arr = doc.createNestedArray(DISPLAY_CFG_KEY);
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DisplayKeys::DISPLAY_AUTO_INTENSITY)] = data.enableAutoIntenisty;
    obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DisplayKeys::DISPLAY_INTENSITY_VALUE)] = data.intensityValue;
    obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DisplayKeys::DISPLAY_ANIM_SPEED)] = data.animSpeed;
    obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DisplayKeys::DISPLAY_TIME_FORMAT)] = data.timeFormat;
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
