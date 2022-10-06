#pragma once

#include "config_param.h"
#include "config_types.h"
#include "config.h"


struct DisplayConfigData
{
    DisplayConfigData()
        : enableAutoIntenisty(true)
        , intensityValue(0)
        , animSpeed(70)
        , timeFormat(1)
    {

    }

    DisplayConfigData(bool enableOnOff, uint8_t intensityValue,
                    uint8_t animSpeed,  uint8_t timeFormat)
        : enableAutoIntenisty(enableOnOff)
        , intensityValue(intensityValue)
        , animSpeed(animSpeed)
        , timeFormat(timeFormat)
    {

    }

    uint8_t enableAutoIntenisty;
    uint8_t intensityValue;
    uint8_t animSpeed;
    uint8_t timeFormat; // [s] in seconds
};

class DisplayConfigParam : public ConfigParam<DisplayConfigData, Config>
{
public:

    enum DisplayKeys : ParamsKey
    {
        WIFI_SSID,
        WIFI_PASSWD,
        WIFI_AP_HOSTNAME,
        WIFI_AP_PASSWD,
    };

    DisplayConfigParam();
    bool setConfig(const JsonObject& json) override;
    void getConfig(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(DisplayConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const DisplayConfigData& data) override;
};