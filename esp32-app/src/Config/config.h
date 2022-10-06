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
#include "wifi_config_param.h"

#define Cfg Config::instance()

/**
 * @brief Class handling the whole system configuration
 */
class Config
{
public:

    struct SystemConfig
    {
        uint32_t magic;
        uint32_t version;
        WifiConfigData wifi;
        // SystemTimeSettings time;
        // WeatherSettings weather;
        // RadioSensorSettings radioSensor;
        // InternalEnvDataSettings intEnv;
        // DisplaySettings display;
        // AppSettings other;
    };

public:
    using ConfigParamKey = const char*;
    using ConfigParamMap = std::map<ConfigParamKey, std::unique_ptr<ConfigParamBase>>;


    explicit Config();
    static Config& instance();
    void init();
    void close();

    const SystemConfig& getDefaults();
    SystemConfig& getCurrent();
    const ConfigParamMap& getCfgMap();

    bool save(const SystemConfig& sysCfg);
    bool read();

private:
    void setDefaults();
    void printCurrentSysCfg();

private:

    SystemConfig m_defaultSysCfg;
    SystemConfig m_currentSysCfg;
    rtos::Mutex m_cfgMtx;
    // NVS preferences
    Preferences prefs;
    ConfigParamMap m_cfgMap;
};
