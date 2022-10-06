#include "wifi_config_param.h"


//------------------------------------------------------------------------------
WifiConfigParam::WifiConfigParam() : ConfigParam(WIFI_CFG_KEY, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool WifiConfigParam::setConfig(const JsonObject& json)
{
    WifiConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        log::dbg("unpackFromJson - SUCCESS");
        auto cfg = m_cfgHndl.getCurrent();
        memcpy(cfg.wifi.ssid, dataFromServer.ssid, sizeof(m_cfgHndl.getCurrent().wifi.ssid));
        memcpy(cfg.wifi.passwd, dataFromServer.passwd, sizeof(m_cfgHndl.getCurrent().wifi.passwd));
        memcpy(cfg.wifi.apHostname, dataFromServer.apHostname, sizeof(m_cfgHndl.getCurrent().wifi.apHostname));
        memcpy(cfg.wifi.apPasswd, dataFromServer.apPasswd, sizeof(m_cfgHndl.getCurrent().wifi.apPasswd));
        // Save cfg
        return m_cfgHndl.save(cfg);
    }

    log::err("unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
void WifiConfigParam::getConfig(String& configPayload)
{
    // Extract config data from application
    WifiConfigData cfgData;
    memcpy(cfgData.ssid, m_cfgHndl.getCurrent().wifi.ssid, sizeof(cfgData.ssid));
    memcpy(cfgData.passwd, m_cfgHndl.getCurrent().wifi.pass, sizeof(cfgData.passwd));
    memcpy(cfgData.apHostname, m_cfgHndl.getCurrent().wifi.ap_hostname, sizeof(cfgData.apHostname));
    memcpy(cfgData.apPasswd, m_cfgHndl.getCurrent().wifi.ap_pass, sizeof(cfgData.apPasswd));
    configPayload = packToJson(cfgData);
}

//------------------------------------------------------------------------------
String WifiConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
            cfgParamKey(WifiKeys::WIFI_SSID)+colon+String(m_cfgData.ssid)+comma + \
            cfgParamKey(WifiKeys::WIFI_PASSWD)+colon+String(m_cfgData.passwd)+comma  + \
            cfgParamKey(WifiKeys::WIFI_AP_HOSTNAME)+colon+String(m_cfgData.apHostname)+comma  + \
            cfgParamKey(WifiKeys::WIFI_AP_PASSWD)+colon+String(m_cfgData.apPasswd);
}

//------------------------------------------------------------------------------
void WifiConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { WifiKeys::WIFI_SSID, "wifi-ssid" },
        { WifiKeys::WIFI_PASSWD, "wifi-pass" },
        { WifiKeys::WIFI_AP_HOSTNAME, "wifi-ap-hostname" },
        { WifiKeys::WIFI_AP_PASSWD, "wifi-ap-pass" },
    };
}

//------------------------------------------------------------------------------
bool WifiConfigParam::unpackFromJson(WifiConfigData& cfgData, const JsonObject& json)
{
    // Not used so far
    bool ret = true;

    JsonArray arr = json[key()].as<JsonArray>();
    for (const auto& v : arr)
    {
        if (v[cfgParamKey(WifiKeys::WIFI_SSID)])
        {
            memset(cfgData.ssid, 0, sizeof(cfgData.ssid));
            memcpy(cfgData.ssid, v[cfgParamKey(WifiKeys::WIFI_SSID)].as<const char*>(), sizeof(cfgData.ssid));
        }
        else if (v[cfgParamKey(WifiKeys::WIFI_PASSWD)])
        {
            memset(cfgData.passwd, 0, sizeof(cfgData.passwd));
            memcpy(cfgData.passwd, v[cfgParamKey(WifiKeys::WIFI_PASSWD)].as<const char*>(), sizeof(cfgData.passwd));
        }
        else if (v[cfgParamKey(WifiKeys::WIFI_AP_HOSTNAME)])
        {
            memset(cfgData.apHostname, 0, sizeof(cfgData.apHostname));
            memcpy(cfgData.apHostname, v[cfgParamKey(WifiKeys::WIFI_AP_HOSTNAME)].as<const char*>(), sizeof(cfgData.apHostname));
        }
        else if (v[cfgParamKey(WifiKeys::WIFI_AP_PASSWD)])
        {
            String pass = v[cfgParamKey(WifiKeys::WIFI_AP_PASSWD)].as<const char*>();
            if (pass.length() < 8 || pass.length() > 32)
            {
                pass = "admin123";
            }
            memset(cfgData.apPasswd, 0, sizeof(cfgData.apPasswd));
            memcpy(cfgData.apPasswd, pass.c_str(), sizeof(cfgData.apPasswd));
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
String WifiConfigParam::packToJson(const WifiConfigData& data)
{
    StaticJsonDocument<512> doc;
    String json;
    JsonArray arr = doc.createNestedArray(key());
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(WifiKeys::WIFI_SSID)] = data.ssid;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WifiKeys::WIFI_PASSWD)] = data.passwd;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WifiKeys::WIFI_AP_HOSTNAME)] = data.apHostname;
    obj = arr.createNestedObject();
    obj[cfgParamKey(WifiKeys::WIFI_AP_PASSWD)] = data.apPasswd;
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
