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
#include "ConfigTypes.h"
#include "ConfigParam.h"
#include "ConfigDataTypes.h"
#include "Rtos/RtosUtils.h"
// Add all the params
#include "WifiConfigParam.h"
#include "TimeConfigParam.h"
#include "DisplayConfigParam.h"
#include "RadioConfigParam.h"
#include "InternalEnvironmentDataConfigParam.h"
#include "WeatherConfigParam.h"
#include "AppConfigParam.h"


#define Cfg Config::instance()

/**
 * @brief Class handling the whole system configuration
 */
class Config : public IConfig
{

public:
    struct ConfigMemoryHeader
    {
        ConfigMemoryHeader()
        : magic(0x4C554B36)
        , version(0x0000000A)
        {
        }
        static constexpr char* hdrKey = "cfg-hdr";
        uint32_t  magic;
        uint32_t  version;
    };

    explicit Config();
    static Config& instance();
    void init();
    void close();

    const ConfigParamMap& getCfgMap();
    const ConfigParamBase& getCfgParam(ConfigParamKey key);

    bool save(ConfigParamKey key, const void *cfg) override;
    bool read(ConfigParamKey key, void *data) override;

private:
    bool saveHdr(const ConfigMemoryHeader& hdr);
    bool readHdr(ConfigMemoryHeader& hdr);
    void printCurrentSystemConfig();

private:
    rtos::Mutex m_cfgMtx;
    // NVS preferences
    Preferences prefs;
    ConfigParamMap m_cfgMap;
    ConfigMemoryHeader m_cfgHeader;

    // Members
    WifiConfigParam wifi;
    TimeConfigParam time;
    DisplayConfigParam display;
    RadioConfigParam radio;
    InternalEnvironmentDataConfigParam intEnv;
    WeatherConfigParam weather;
    AppConfigParam app;
};
