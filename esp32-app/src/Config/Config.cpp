#include "Config.h"
#include "Rtos/logger.h"
#include <exception>

#undef USE_DEVEL_CFG

#ifdef USE_DEVEL_CFG
    #include "dev_cfg.h"
#endif


//-----------------------------------------------------------------------------
Config::Config()
{
    // Create config map
    m_cfgMap =
    {
        { CFG_KEY_WIFI, std::unique_ptr<WifiConfigParam>(new WifiConfigParam()) },
        { CFG_KEY_TIME, std::unique_ptr<TimeConfigParam>(new TimeConfigParam()) },
        { CFG_KEY_WEATHER, std::unique_ptr<WeatherConfigParam>(new WeatherConfigParam()) },
        { CFG_KEY_INTENV, std::unique_ptr<InternalEnvironmentDataConfigParam>(new InternalEnvironmentDataConfigParam()) },
        { CFG_KEY_RADIO, std::unique_ptr<RadioConfigParam>(new RadioConfigParam()) },
        { CFG_KEY_DISPLAY, std::unique_ptr<DisplayConfigParam>(new DisplayConfigParam()) },
        { CFG_KEY_APP, std::unique_ptr<AppConfigParam>(new AppConfigParam()) },
    };
}

//------------------------------------------------------------------------------
Config& Config::instance()
{
    static Config cfg;
    return cfg;
}

//------------------------------------------------------------------------------
void Config::init()
{
    // The begin() call is required to initialise the NVS library
    prefs.begin("ledclock", false);

    ConfigMemoryHeader hdr;
    if (readHdr(hdr))
    {
        // Check if memory header is valid
        if (hdr.magic == m_cfgHeader.magic && \
            hdr.version == m_cfgHeader.version)
        {
            logger::inf("ConfigMemoryHeader read succesfully\r\n");
            // Print content
            printCurrentSystemConfig();
            return;
        }
        else
        {
            logger::err("ConfigMemoryHeader has been modified, updating with defaults..\r\n");
        }
    }
    else
    {
        logger::err("ConfigMemoryHeader read failed, updating with defaults..\r\n");
    }

    if (!saveHdr(m_cfgHeader))
    {
        logger::err("Updating ConfigMemoryHeader with defaults failed!, setting current as defaults\r\n");
    }
    else
    {
        logger::inf("Updating ConfigMemoryHeader with defaults succeed!\r\n");
    }
}

//------------------------------------------------------------------------------
void Config::close()
{
    prefs.end();
}


//------------------------------------------------------------------------------
const ConfigParamBase& Config::getCfgParam(ConfigParamKey key)
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    if (m_cfgMap.find(key) != m_cfgMap.end())
    {
        return *m_cfgMap[key].get();
    }
    throw std::invalid_argument("Invalid key applied");
}

//------------------------------------------------------------------------------
const ConfigParamMap& Config::getCfgMap()
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    return m_cfgMap;
}

//------------------------------------------------------------------------------
bool Config::save(ConfigParamKey key, const void *cfg)
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);

    const auto cfgDataIt = m_cfgMap.find(key);
    if (cfgDataIt != m_cfgMap.end())
    {

        // Set the NVS data ready for writing
        size_t ret = prefs.putBytes(key, cfg, cfgDataIt->second->cfgDataSize());

        // Write the data to NVS
        if (ret == cfgDataIt->second->cfgDataSize())
        {
            logger::inf("ConfigData for %s saved in NVS succesfully\r\n", key);
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool Config::read(ConfigParamKey key, void *data)
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    const auto cfgDataIt = m_cfgMap.find(key);
    if (data && cfgDataIt != m_cfgMap.end())
    {
        size_t ret = prefs.getBytes(key, data, cfgDataIt->second->cfgDataSize());
        if (ret == cfgDataIt->second->cfgDataSize())
        {
            logger::inf("ConfigData for %s read from NVS succesfully\r\n", key);
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool Config::saveHdr(const Config::ConfigMemoryHeader& hdr)
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    size_t ret = prefs.putBytes(Config::ConfigMemoryHeader::hdrKey, (const void*)&hdr, sizeof(hdr));
    if (ret == sizeof(hdr))
    {
        logger::inf("ConfigData for %s saved in NVS succesfully\r\n", ConfigMemoryHeader::hdrKey);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool Config::readHdr(ConfigMemoryHeader& hdr)
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    size_t ret = prefs.getBytes(Config::ConfigMemoryHeader::hdrKey, &hdr, sizeof(hdr));
    if (ret == sizeof(hdr))
    {
        logger::inf("ConfigData for %s read from NVS succesfully\r\n", Config::ConfigMemoryHeader::hdrKey);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
void Config::printCurrentSystemConfig()
{
    logger::inf("SYSTEM_CONFIG: <<CURRENT APP SETTINGS>>");
    logger::inf("magic: 0x%08x", m_cfgHeader.magic);
    logger::inf("version: 0x%08x", m_cfgHeader.version);
    for (auto& cfg: m_cfgMap)
    {
        logger::inf(cfg.second->toStr().c_str());
    }
    logger::inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
}

//------------------------------------------------------------------------------
