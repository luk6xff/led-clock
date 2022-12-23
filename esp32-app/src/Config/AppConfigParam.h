#pragma once

#include "ConfigTypes.h"
#include "ConfigParam.h"
#include "ConfigDataTypes.h"


class AppConfigParam : public ConfigParam<AppConfigData>
{
public:

    enum AppKeys : ParamsKey
    {
        APP_LANGUAGE,
    };

    AppConfigParam();
    bool setConfigFromJson(const JsonObject& json) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(AppConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const AppConfigData& data) override;
};
