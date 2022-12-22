#include "AppConfigParam.h"


//------------------------------------------------------------------------------
AppConfigParam::AppConfigParam() : ConfigParam(INTENV_CFG_KEY, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool AppConfigParam::setConfig(const JsonObject& json)
{
    AppConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        log::dbg("AppConfigParam::unpackFromJson - SUCCESS");
        if (m_cfgHndl.getCurrent().app != dataFromServer)
        {
            log::dbg("Storing new AppConfigData settings");
            return m_cfgHndl.save(dataFromServer);
        }
        log::dbg("Storing AppConfigData settings skipped, no change detected");
        return false;
    }

    log::err("AppConfigParam::unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
void AppConfigParam::getConfig(String& configPayload)
{
    // Extract config data from application
    configPayload = packToJson(m_cfgHndl.getCurrent().app);
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
