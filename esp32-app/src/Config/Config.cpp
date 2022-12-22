#include "Config.h"
#include "Rtos/logger.h"


#undef USE_DEVEL_CFG

#ifdef USE_DEVEL_CFG
    #include "dev_cfg.h"
#endif


//-----------------------------------------------------------------------------
Config::Config()
{
    // Set defaults
    setDefaults();
    // Create config map
    m_cfgMap =
    {
        { WIFI_CFG_KEY, std::unique_ptr<WifiConfigParam>(new WifiConfigParam()) },
        { TIME_CFG_KEY, std::unique_ptr<TimeConfigParam>(new TimeConfigParam()) },
        { WEATHER_CFG_KEY, std::unique_ptr<WeatherConfigParam>(new WeatherConfigParam()) },
        { INTENV_CFG_KEY, std::unique_ptr<InternalEnvironmentDataConfigParam>(new InternalEnvironmentDataConfigParam()) },
        { RADIO_CFG_KEY, std::unique_ptr<RadioConfigParam>(new RadioConfigParam()) },
        { DISPLAY_CFG_KEY, std::unique_ptr<DisplayConfigParam>(new DisplayConfigParam()) },
        { APP_CFG_KEY, std::unique_ptr<AppConfigParam>(new AppConfigParam()) },

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

    read();

    // Check if sysCfg are valid
    if (getCurrent().magic == getDefaults().magic && \
        getCurrent().version == getDefaults().version)
    {
        logger::inf("SysCfg read succesfully\r\n");
        // Print content
        printCurrentSysCfg();
    }
    else
    {
        logger::inf("SysCfg read failed, updating with defaults..\r\n");
        if (!save(getDefaults()))
        {
            logger::inf("Updating SysCfg with defaults failed!, setting current as defaults\r\n");
            m_currentCfgData = m_defaultCfgData;
        }
        else
        {
            logger::inf("Updating SysCfg with defaults succeed!\r\n");
        }
    }
}

//------------------------------------------------------------------------------
void Config::close()
{
    prefs.end();
}

//------------------------------------------------------------------------------
const Config::ConfigData& Config::getDefaults()
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    return m_defaultCfgData;
}

//------------------------------------------------------------------------------
Config::ConfigData& Config::getCurrent()
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    return m_currentCfgData;
}

//------------------------------------------------------------------------------
const Config::ConfigParamMap& Config::getCfgMap()
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
        // Check if cfg are upto date, do not write them
        if (memcmp(cfgDataIt->second, &m_currentCfgData, sizeof(ConfigData)) == 0)
        {
            logger::inf("Current Cfg is equal to provided sysCfg, NVS save skipped!\r\n");
            m_currentCfgData = sysCfg;
            return false;
        }
        // Set the NVS data ready for writing
        size_t ret = prefs.putBytes("syscfg", (const void*)&sysCfg, sizeof(sysCfg));

        // Write the data to NVS
        if (ret == sizeof(sysCfg))
        {
            logger::inf("SysCfg saved in NVS succesfully\r\n");
            m_currentCfgData = sysCfg;
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool Config::read()
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    size_t ret = prefs.getBytes("syscfg", &m_currentCfgData, sizeof(m_currentCfgData));
    if (ret == sizeof(m_currentCfgData))
    {
        logger::inf("SysCfg read from NVS succesfully\r\n");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
void Config::setDefaults()
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    // Modify according to your application
    // MAGIC
    m_defaultCfgData =
    {
        .magic   = 0x4C554B36,  // LUK6
        .version = 0x0000000A,
    };

    // WIFI
    WifiConfigData wifi;
#ifdef DEV_CFG
    memcpy(wifi.ssid, DEV_CFG_WIFI_SSID, sizeof(wifi.ssid));
    memcpy(wifi.pass, DEV_CFG_WIFI_PASS, sizeof(wifi.passwd));
#else
    memcpy(wifi.ssid, "admin", sizeof(wifi.ssid));
    memcpy(wifi.passwd, "admin", sizeof(wifi.passwd));
#endif
    memcpy(wifi.apHostname, "ledclock", sizeof(wifi.apHostname));
    memcpy(wifi.apPasswd, "admin123", sizeof(wifi.apPasswd));
    m_defaultCfgData.wifi = wifi;

    // TIME
    TimeConfigData time = {
        2,
        1,// {"CET", Last, Sun, Oct, 3, 60},  // Central European Standard Time
        2,// {"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
        true,
        0,
        (1000*3600),
        "time.google.com",
        "pl.pool.ntp.org",
        "pool.ntp.org"
    };
    m_defaultCfgData.time = time;

    // DISPLAY
    DisplayConfigData display = {false, 0, 30, 1};
    m_defaultCfgData.display = display;

    // RADIO_SENSOR
    RadioConfigData radio = {false, 1800, 3000, 1, 5};
    m_defaultCfgData.radio = radio;

    // INTERNAL ENVIRONMENT DATA
    InternalEnvironmentDataConfigData intEnv = {false, 900, 1};
    m_defaultCfgData.intEnv = intEnv;

    // WEATHER
    WeatherConfigData weather;
#ifdef DEV_CFG
    memcpy(weather.owmAppid, DEV_CFG_WEATHER_OWM_APPID, OWM_APPID_MAXLEN);
#endif
    m_defaultCfgData.weather = weather;

    // APP
    AppConfigData app;
    m_defaultCfgData.app = app;
}

//------------------------------------------------------------------------------
void Config::printCurrentSysCfg()
{
    logger::inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
    logger::inf("SysCfg size: %d bytes", sizeof(getCurrent()));
    logger::inf("magic: 0x%08x", getCurrent().magic);
    logger::inf("version: 0x%08x", getCurrent().version);
    for (auto& cfg: m_cfgMap)
    {
        logger::inf(cfg.second->toStr().c_str());
    }
    logger::inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
}

//------------------------------------------------------------------------------
