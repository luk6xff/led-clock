#include "AppConfigParam.h"
#include "Rtos/logger.h"
#include "Config.h"

//------------------------------------------------------------------------------
AppConfigParam::AppConfigParam() : ConfigParam(CFG_KEY_INTENV, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool AppConfigParam::setConfigFromJson(const JsonObject& json)
{
    AppConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        logger::dbg("AppConfigParam::unpackFromJson - SUCCESS");
        if (std::memcmp(&m_cfgData, &dataFromServer, cfgDataSize()) != 0)
        {
            logger::dbg("Storing new AppConfigData settings");
            return m_cfgHndl.save(key(), &dataFromServer);
        }
        logger::dbg("Storing AppConfigData settings skipped, no change detected");
        return false;
    }

    logger::err("AppConfigParam::unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
String AppConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
            cfgParamKey(AppKeys::APP_LANGUAGE)+colon+String(m_cfgData.appLang);
}

//------------------------------------------------------------------------------
void AppConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { AppKeys::APP_LANGUAGE, "app-lang" },
    };
}

//------------------------------------------------------------------------------
bool AppConfigParam::unpackFromJson(AppConfigData& cfgData, const JsonObject& json)
{
    // Not used so far
    bool ret = true;

    JsonArray arr = json[key()].as<JsonArray>();
    for (const auto& v : arr)
    {
        if (v[cfgParamKey(AppKeys::APP_LANGUAGE)])
        {
            cfgData.appLang = v[cfgParamKey(AppKeys::APP_LANGUAGE)].as<i18n_lang>();
            if (cfgData.appLang < 0 || cfgData.appLang >= I18N_LANGUAGES_NUM)
            {
                cfgData.appLang = I18N_ENGLISH;
            }
        }
    }
    return ret;
}

//------------------------------------------------------------------------------
String AppConfigParam::packToJson(const AppConfigData& data)
{
    StaticJsonDocument<512> doc;
    String json;
    JsonArray arr = doc.createNestedArray(key());
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(AppKeys::APP_LANGUAGE)] = data.appLang;
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
