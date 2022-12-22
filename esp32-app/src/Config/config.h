#pragma once
/**
 * @brief App configuration stuff
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2022-10-07
 */

#include <map>
#include <memory>
#include <Preferences.h>
#include "RtosUtils/rtos_utils.h"
// Add all the params
#include "WifiConfigParam.h"
#include "TimeConfigParam.h"
#include "DisplayConfigParam.h"
#include "RadioConfigParam.h"
#include "InternalEnvironmentDataConfigParam.h"


#define Cfg Config::instance()

/**
 * @brief Class handling the whole system configuration
 */
class Config
{
public:

    struct ConfigData
    {
        uint32_t magic;
        uint32_t version;
        WifiConfigData wifi;
        TimeConfigData time;
        DisplayConfigData display;
        RadioConfigData radio;
        InternalEnvironmentDataConfigData intEnv;
        // WeatherSettins weather;
        // AppSettings other;
    };

public:
    using ConfigParamKey = const char*;
    using ConfigParamMap = std::map<ConfigParamKey, std::unique_ptr<ConfigParamBase>>;


    explicit Config();
    static Config& instance();
    void init();
    void close();

    const ConfigData& getDefaults();
    ConfigData& getCurrent();
    const ConfigParamMap& getCfgMap();

    bool save(const ConfigData& sysCfg);
    bool read();

private:
    void setDefaults();
    void printCurrentSysCfg();

private:

    ConfigData m_defaultCfgData;
    ConfigData m_currentCfgData;
    rtos::Mutex m_cfgMtx;
    // NVS preferences
    Preferences prefs;
    ConfigParamMap m_cfgMap;
};
