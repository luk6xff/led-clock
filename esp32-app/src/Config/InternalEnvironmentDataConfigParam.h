#pragma once

#include "config_param.h"
#include "config_types.h"
#include "config.h"



struct InternalEnvironmentDataConfigData
{
    bool enabled;                 // is functionality active
    uint32_t dataUpdateInterval; // [s] in seconds
    uint32_t msgDispRepeatNum;   // How many times incoming message shall be displayed
};


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
