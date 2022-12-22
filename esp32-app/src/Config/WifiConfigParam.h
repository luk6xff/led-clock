#pragma once

#include "ConfigParam.h"
#include "ConfigTypes.h"
#include "Config.h"


#undef USE_DEVEL_CFG

#ifdef USE_DEVEL_CFG
    #include "dev_cfg.h"
#endif





class Config;

class WifiConfigParam : public ConfigParam<WifiConfigData, Config>
{
public:

    enum WifiKeys : ParamsKey
    {
        WIFI_SSID,
        WIFI_PASSWD,
        WIFI_AP_HOSTNAME,
        WIFI_AP_PASSWD,
    };

    WifiConfigParam();
    bool setConfigFromJson(const JsonObject& json) override;
    void getConfigAsStr(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(WifiConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const WifiConfigData& data) override;
};
