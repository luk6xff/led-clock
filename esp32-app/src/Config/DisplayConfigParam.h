#pragma once

#include "ConfigTypes.h"
#include "ConfigParam.h"
#include "ConfigDataTypes.h"


class DisplayConfigParam : public ConfigParam<DisplayConfigData>
{
public:

    enum DisplayKeys : ParamsKey
    {
        DISPLAY_ENABLE_AUTO_INTENSITY,
        DISPLAY_INTENSITY_VALUE,
        DISPLAY_ANIMATION_SPEED,
        DISPLAY_TIME_FORMAT,
    };

    DisplayConfigParam();
    bool setConfigFromJson(const JsonObject& json) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(DisplayConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const DisplayConfigData& data) override;
};