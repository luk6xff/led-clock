#pragma once

#include "ConfigTypes.h"
#include "ConfigParam.h"
#include "ConfigDataTypes.h"


class InternalEnvironmentDataConfigParam : public ConfigParam<InternalEnvironmentDataConfigData>
{
public:

    enum InternalEnvironmentDataKeys : ParamsKey
    {
        INTERNAL_ENVIRONMENT_DATA_ENABLED,
        INTERNAL_ENVIRONMENT_DATA_UPDATE_INTERVAL,
        INTERNAL_ENVIRONMENT_MSG_DISP_REPEAT_NUM,
    };

    InternalEnvironmentDataConfigParam();
    bool setConfigFromJson(const JsonObject& json) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(InternalEnvironmentDataConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const InternalEnvironmentDataConfigData& data) override;
};
