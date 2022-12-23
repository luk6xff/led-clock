#pragma once

#include "ConfigTypes.h"
#include "ConfigParam.h"
#include "ConfigDataTypes.h"


class RadioConfigParam : public ConfigParam<RadioConfigData>
{
public:

    enum RadioKeys : ParamsKey
    {
        RADIO_ENABLED,
        RADIO_DATA_UPDATE_INTERVAL,
        RADIO_CRITICAL_VBATT,
        RADIO_MSG_DISP_REPEAT_NUM,
        RADIO_LAST_MSG_DISP_EVERY_MINUTES,
    };


    RadioConfigParam();
    bool setConfigFromJson(const JsonObject& json) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(RadioConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const RadioConfigData& data) override;
};
