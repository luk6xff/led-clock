#pragma once

#include "ConfigParam.h"
#include "ConfigTypes.h"
#include "Config.h"

class Config;

class RadioConfigParam : public ConfigParam<RadioConfigData, Config>
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
    bool setConfig(const JsonObject& json) override;
    void getConfig(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(RadioConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const RadioConfigData& data) override;
};
