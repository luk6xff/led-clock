#pragma once

#include "web_config_param.h"
#include "web_config_types.h"

#define WIFI_CONFIG_LEN 32

struct WifiConfigData
{
    // WIFI
    char ssid[WIFI_CONFIG_LEN];
    char passwd[WIFI_CONFIG_LEN];
    char apHostname[WIFI_CONFIG_LEN];
    char apPasswd[WIFI_CONFIG_LEN];
};

class WifiConfigParam : public WebConfigParam<WifiConfigData>
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


private:
    void setCfgParamsMap();
    bool unpackFromJson(WifiConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const WifiConfigData& data) override;
};