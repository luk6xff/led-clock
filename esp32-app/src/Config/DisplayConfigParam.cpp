#include "DisplayConfigParam.h"
#include "Rtos/logger.h"
#include "Config.h"

//------------------------------------------------------------------------------
DisplayConfigParam::DisplayConfigParam() : ConfigParam(CFG_KEY_DISPLAY, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool DisplayConfigParam::setConfigFromJson(const JsonObject& json)
{
    DisplayConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        logger::dbg("DisplayConfigParam::unpackFromJson - SUCCESS");
        if (std::memcmp(&m_cfgData, &dataFromServer, cfgDataSize()) != 0)
        {
            logger::dbg("Storing new DisplayConfigData settings");
            return m_cfgHndl.save(key(), &dataFromServer);
        }
        logger::dbg("Storing DisplayConfigData settings skipped, no change detected");
        return false;
    }

    logger::err("DisplayConfigParam::unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
String DisplayConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
        cfgParamKey(DisplayKeys::DISPLAY_ENABLE_AUTO_INTENSITY)+colon+String(m_cfgData.enableAutoIntenisty)+comma + \
        cfgParamKey(DisplayKeys::DISPLAY_INTENSITY_VALUE)+colon+String(m_cfgData.intensityValue)+comma  + \
        cfgParamKey(DisplayKeys::DISPLAY_ANIMATION_SPEED)+colon+String(m_cfgData.animationSpeed)+comma  + \
        cfgParamKey(DisplayKeys::DISPLAY_TIME_FORMAT)+colon+String(m_cfgData.timeFormat);
}

//------------------------------------------------------------------------------
void DisplayConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { DisplayKeys::DISPLAY_ENABLE_AUTO_INTENSITY,   "display-auto-intensity" },
        { DisplayKeys::DISPLAY_INTENSITY_VALUE,         "display-intensity-val" },
        { DisplayKeys::DISPLAY_ANIMATION_SPEED,         "display-anim-speed" },
        { DisplayKeys::DISPLAY_TIME_FORMAT,             "display-time-format" },
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
        if (v[cfgParamKey(DisplayKeys::DISPLAY_ENABLE_AUTO_INTENSITY)])
        {
            cfgData.enableAutoIntenisty = v[cfgParamKey(DisplayKeys::DISPLAY_ENABLE_AUTO_INTENSITY)].as<uint8_t>();
        }
        else if (v[cfgParamKey(DisplayKeys::DISPLAY_INTENSITY_VALUE)])
        {
            cfgData.intensityValue = v[cfgParamKey(DisplayKeys::DISPLAY_INTENSITY_VALUE)].as<uint8_t>();
            if (cfgData.intensityValue > 15)
            {
                cfgData.intensityValue = 0;
            }
        }
        else if (v[cfgParamKey(DisplayKeys::DISPLAY_ANIMATION_SPEED)])
        {
            cfgData.animationSpeed = v[cfgParamKey(DisplayKeys::DISPLAY_ANIMATION_SPEED)].as<uint8_t>();
            if (cfgData.animationSpeed < 1 || cfgData.animationSpeed > 1000)
            {
                cfgData.animationSpeed = 50;
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
    JsonArray arr = doc.createNestedArray(CFG_KEY_DISPLAY);
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DISPLAY_ENABLE_AUTO_INTENSITY)] = data.enableAutoIntenisty;
    obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DISPLAY_INTENSITY_VALUE)] = data.intensityValue;
    obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DISPLAY_ANIMATION_SPEED)] = data.animationSpeed;
    obj = arr.createNestedObject();
    obj[cfgParamKey(DisplayKeys::DISPLAY_TIME_FORMAT)] = data.timeFormat;
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
