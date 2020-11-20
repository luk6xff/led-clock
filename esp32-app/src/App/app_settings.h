#pragma once

#include <ArduinoJson.h>
#include <cstdint>
#include <string>
#include "App/utils.h"
#include "App/i18n.h"


#define APP_CFG_KEY   "dev-cfg-app"
#define APP_LANGUAGE  "app-lang"


struct AppSettings
{
    AppSettings()
        : appLang(I18N_POLISH)
    {
    }

    AppSettings(i18n_lang lang)
        : appLang(lang)
    {
    }

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

    void fromJson(const JsonObject& json)
    {
        JsonArray arr = json[APP_CFG_KEY ].as<JsonArray>();
        for (const auto& v : arr)
        {
            if (v[APP_LANGUAGE])
            {
                appLang = v[APP_LANGUAGE].as<i18n_lang>();
                if (appLang < 0 || appLang >= I18N_LANGUAGES_NUM)
                {
                    appLang = I18N_ENGLISH;
                }
            }
        }
    }

    String toStr()
    {
        const String colon = ":";
        const String comma =", ";
        return String(APP_CFG_KEY )+colon+comma + \
                String(APP_LANGUAGE)+colon+String(appLang);
    }


    i18n_lang appLang;
};