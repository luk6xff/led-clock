#include "WifiConfigParam.h"
#include "Rtos/logger.h"
#include "Config.h"

//------------------------------------------------------------------------------
WifiConfigParam::WifiConfigParam()
: ConfigParam(CFG_KEY_WIFI, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool WifiConfigParam::setConfigFromJson(const JsonObject& json)
{
    WifiConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        logger::dbg("WifiConfigParam::unpackFromJson - SUCCESS");
        if (std::memcmp(&m_cfgData, &dataFromServer, cfgDataSize()) != 0)
        {
            logger::dbg("Storing new WifiConfigParam settings");
            return m_cfgHndl.save(key(), &dataFromServer);
        }
        logger::dbg("Storing WifiConfigParam settings skipped, no change detected");
        return false;
    }

    logger::err("WifiConfigParam::unpackFromJson - ERROR");
    return false;
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
