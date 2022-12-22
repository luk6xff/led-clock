#pragma once

#include "ConfigParam.h"
#include "ConfigTypes.h"
#include "Config.h"

class Config;

class InternalEnvironmentDataConfigParam : public ConfigParam<InternalEnvironmentDataConfigData, Config>
{
public:

    enum InternalEnvironmentDataKeys : ParamsKey
    {
        INTERNAL_ENVIRONMENT_DATA_ENABLED,
        INTERNAL_ENVIRONMENT_DATA_UPDATE_INTERVAL,
        INTERNAL_ENVIRONMENT_MSG_DISP_REPEAT_NUM,
    };

    InternalEnvironmentDataConfigParam();
    bool setConfig(const JsonObject& json) override;
    void getConfig(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(InternalEnvironmentDataConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const InternalEnvironmentDataConfigData& data) override;
};
