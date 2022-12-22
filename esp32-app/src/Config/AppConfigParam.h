#pragma once

#include "ConfigParam.h"
#include "ConfigTypes.h"
#include "Config.h"


class AppConfigParam : public ConfigParam<AppConfigData, Config>
{
public:

    enum AppKeys : ParamsKey
    {
        APP_LANGUAGE,
    };

    AppConfigParam();
    bool setConfigFromJson(const JsonObject& json) override;
    void getConfigAsStr(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(AppConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const AppConfigData& data) override;
};
