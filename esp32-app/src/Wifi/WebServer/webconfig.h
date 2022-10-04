#pragma once

#include <map>
#include <ArduinoJson.h>
#include "App/app_config.h"

/**
 * @brief Class handling web configuration
 */
class WebConfig
{

public:

    enum class ParamsKey : uint16_t
    {
        WIFI_SSID,
        WIFI_PASSWD,
        WIFI_AP_HOSTNAME,
        WIFI_AP_PASSWD,
        API_IP_ADDRESS,
        API_IP_PORT,
        API_ACCESS_TOKEN,
    };

    using ParamsMap = std::map<WebConfig::ParamsKey, const char*>;

    #define WEB_CONFIG_LEN 32

    /**
     * @brief Data configured by web interface
     * @note It shall match data in web-app/gateway-vue-app/src/pages/DeviceConfig.vue
     */
    struct WebConfigData
    {
        char wifiSsid[WEB_CONFIG_LEN];
        char wifiPasswd[WEB_CONFIG_LEN];
        char wifiApHostname[WEB_CONFIG_LEN];
        char wifiApPasswd[WEB_CONFIG_LEN];
        char apiIpAddress[WEB_CONFIG_LEN];
        uint16_t apiIpPort;
        char apiAccessToken[WEB_CONFIG_LEN];
    };

public:
    explicit WebConfig(AppConfig& appCfg);

    bool setConfig(const JsonObject& json);
    void getConfig(String& configPayload);

private:
    bool unpackFromJson(WebConfigData& cfgData, const JsonObject& json);
    String packToJson(const WebConfig::WebConfigData& data);
    void setCfgParamsMap();
    bool checkIfKeyExists(const char *key, const JsonObject& json);
    const char* cfgKey(ParamsKey key);

private:
    AppConfig& m_appCfg;
    ParamsMap m_cfgParamsMap;
};
