#pragma once

#include "ConfigParam.h"
#include "ConfigTypes.h"
#include "Config.h"
#include "App/i18n.h"



struct AppConfigData
{

    AppConfigData()
        : appLang(I18N_POLISH)
    {
    }
    i18n_lang appLang;
};


class AppConfigParam : public ConfigParam<AppConfigData, Config>
{
public:

    enum AppKeys : ParamsKey
    {
        APP_LANGUAGE,
    };

    AppConfigParam();
    bool setConfig(const JsonObject& json) override;
    void getConfig(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(AppConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const AppConfigData& data) override;
};


    std::string toJson()
    {
        StaticJsonDocument<512> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(APP_CFG_KEY );
        JsonObject obj = arr.createNestedObject();
        obj[APP_LANGUAGE] = appLang;
        serializeJson(doc, json);
        return json;
    }
