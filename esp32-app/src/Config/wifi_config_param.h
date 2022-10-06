#pragma once

#include "config_param.h"
#include "config_types.h"
#include "config.h"


#undef USE_DEVEL_CFG

#ifdef USE_DEVEL_CFG
    #include "dev_cfg.h"
#endif



#define WIFI_CONFIG_LEN 32

struct WifiConfigData
{
    char ssid[WIFI_CONFIG_LEN];
    char passwd[WIFI_CONFIG_LEN];
    char apHostname[WIFI_CONFIG_LEN];
    char apPasswd[WIFI_CONFIG_LEN];
};

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
    bool setConfig(const JsonObject& json) override;
    void getConfig(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(WifiConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const WifiConfigData& data) override;
};