#include "wifi_config_param.h"


WifiConfigParam::WifiConfigParam() : WebConfigParam(WIFI_CFG_KEY)
{
    setCfgParamsMap();
}

bool WifiConfigParam::setConfig(const JsonObject& json)
{
    return true;
}

void WifiConfigParam::getConfig(String& configPayload)
{

}

void WifiConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { WifiKeys::WIFI_SSID, "wifi-ssid" },
        { WifiKeys::WIFI_PASSWD, "wifi-pass" },
        { WifiKeys::WIFI_AP_HOSTNAME, "wifi-ap-hostname" },
        { WifiKeys::WIFI_AP_PASSWD, "wifi-ap-pass" },
    };
}

bool WifiConfigParam::unpackFromJson(WifiConfigData& cfgData, const JsonObject& json)
{
    return true;
}

String WifiConfigParam::packToJson(const WifiConfigData& data)
{
    return "";
}

