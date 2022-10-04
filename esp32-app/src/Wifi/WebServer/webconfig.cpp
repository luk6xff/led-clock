#include "webconfig.h"



//-----------------------------------------------------------------------------
WebConfig::WebConfig(AppConfig& appCfg)
    : m_appCfg(appCfg)
{
    setCfgParamsMap();
}

//-----------------------------------------------------------------------------
bool WebConfig::setConfig(const JsonObject& json)
{
    WebConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        utils::dbg("unpackFromJson - SUCCESS");
        AppConfig::Settings settings = m_appCfg.getCurrent();
        // WIFI
        memcpy(settings.wifi.ssid, dataFromServer.wifiSsid, sizeof(m_appCfg.getCurrent().wifi.ssid));
        memcpy(settings.wifi.pass, dataFromServer.wifiPasswd, sizeof(m_appCfg.getCurrent().wifi.pass));
        memcpy(settings.wifi.ap_hostname, dataFromServer.wifiApHostname, sizeof(m_appCfg.getCurrent().wifi.ap_hostname));
        memcpy(settings.wifi.ap_pass, dataFromServer.wifiApPasswd, sizeof(m_appCfg.getCurrent().wifi.ap_pass));
        // Save settings
        return m_appCfg.saveSettings(settings);
    }

    utils::err("unpackFromJson - ERROR");
    return false;
}

//-----------------------------------------------------------------------------
void WebConfig::getConfig(String& configPayload)
{
    // Extract config data from application
    WebConfigData cfgData;
    // WIFI
    memcpy(cfgData.wifiSsid, m_appCfg.getCurrent().wifi.ssid, sizeof(cfgData.wifiSsid));
    memcpy(cfgData.wifiPasswd, m_appCfg.getCurrent().wifi.pass, sizeof(cfgData.wifiPasswd));
    memcpy(cfgData.wifiApHostname, m_appCfg.getCurrent().wifi.ap_hostname, sizeof(cfgData.wifiApHostname));
    memcpy(cfgData.wifiApPasswd, m_appCfg.getCurrent().wifi.ap_pass, sizeof(cfgData.wifiApPasswd));
    configPayload = packToJson(cfgData);
}

//-----------------------------------------------------------------------------
bool WebConfig::unpackFromJson(WebConfig::WebConfigData& cfgData, const JsonObject& json)
{
    bool ret = true;

    // Apply data
    for (auto& paramKey : m_cfgParamsMap)
    {
        if (checkIfKeyExists(paramKey.second, json))
        {
            switch (paramKey.first)
            {
                case (ParamsKey::WIFI_SSID):
                {
                    memcpy(cfgData.wifiSsid, json[paramKey.second].as<const char*>(), sizeof(cfgData.wifiSsid));
                    break;
                }
                case (ParamsKey::WIFI_PASSWD):
                {
                    memcpy(cfgData.wifiPasswd, json[paramKey.second].as<const char*>(), sizeof(cfgData.wifiPasswd));
                    break;
                }

                case (ParamsKey::WIFI_AP_HOSTNAME):
                {
                    memcpy(cfgData.wifiApHostname, json[paramKey.second].as<const char*>(), sizeof(cfgData.wifiApHostname));
                    break;
                }
                case (ParamsKey::WIFI_AP_PASSWD):
                {
                    memcpy(cfgData.wifiApPasswd, json[paramKey.second].as<const char*>(), sizeof(cfgData.wifiApPasswd));
                    break;
                }
            }
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------
String WebConfig::packToJson(const WebConfig::WebConfigData& data)
{
    DynamicJsonDocument doc(1024);
    String json;
    doc[cfgKey(ParamsKey::WIFI_SSID)] = data.wifiSsid;
    doc[cfgKey(ParamsKey::WIFI_PASSWD)] = data.wifiPasswd;
    doc[cfgKey(ParamsKey::WIFI_AP_HOSTNAME)] = data.wifiApHostname;
    doc[cfgKey(ParamsKey::WIFI_AP_PASSWD)] = data.wifiApPasswd;
    serializeJson(doc, json);
    return json;
}

//-----------------------------------------------------------------------------
void WebConfig::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { ParamsKey::WIFI_SSID, "wifi-ssid" },
        { ParamsKey::WIFI_PASSWD, "wifi-pass" },
        { ParamsKey::WIFI_AP_HOSTNAME, "wifi-ap-hostname" },
        { ParamsKey::WIFI_AP_PASSWD, "wifi-ap-pass" },
    };
}

//-----------------------------------------------------------------------------
bool WebConfig::checkIfKeyExists(const char *key, const JsonObject& json)
{
    if (!key && std::strlen(key) > 0)
    {
        return false;
    }

    if (json.containsKey(key))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
const char* WebConfig::cfgKey(ParamsKey key)
{
    if (m_cfgParamsMap.find(key) != m_cfgParamsMap.end())
    {
        return m_cfgParamsMap[key];
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
