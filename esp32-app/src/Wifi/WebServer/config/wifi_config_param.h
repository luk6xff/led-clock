#pragma once

#include "web_config_param.h"
#include "web_config_types.h"

#define WIFI_CONFIG_LEN 32

struct WifiConfigData
{
    // WIFI
    char wifiSsid[WIFI_CONFIG_LEN];
    char wifiPasswd[WIFI_CONFIG_LEN];
    char wifiApHostname[WIFI_CONFIG_LEN];
    char wifiApPasswd[WIFI_CONFIG_LEN];
};

class WifiConfigParam:
public WebConfigParam<WifiConfigData>
{
public:

    enum WifiKeys : ParamsKey
    {
        WIFI_SSID,
        WIFI_PASSWD,
        WIFI_AP_HOSTNAME,
        WIFI_AP_PASSWD,
    };

    WifiConfigParam() : WebConfigParam(WIFI_CFG_KEY)
    {
        setCfgParamsMap();
    }

    bool setConfig(const JsonObject& json) override;

    void getConfig(String& configPayload) override;


private:

    void setCfgParamsMap() override
    {
        m_cfgParamsMap = {
            { WifiKeys::WIFI_SSID, "wifi-ssid" },
            { WifiKeys::WIFI_PASSWD, "wifi-pass" },
            { WifiKeys::WIFI_AP_HOSTNAME, "wifi-ap-hostname" },
            { WifiKeys::WIFI_AP_PASSWD, "wifi-ap-pass" },
        };
    }

    bool unpackFromJson(WifiConfigData& cfgData, const JsonObject& json) override
    {
        return true;
    }

    String packToJson(const WifiConfigData& data) override
    {
        return "";
    }
};