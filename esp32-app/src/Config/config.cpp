#include "config.h"
#include "RtosUtils/utils.h"


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
    }
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
        log::inf("SysCfg read succesfully\r\n");
        // Print content
        printCurrentSysCfg();
    }
    else
    {
        log::inf("SysCfg read failed, updating with defaults..\r\n");
        if (!save(getDefaults()))
        {
            log::inf("Updating SysCfg with defaults failed!, setting current as defaults\r\n");
            m_currentCfgData = m_defaultCfgData;
        }
        else
        {
            log::inf("Updating SysCfg with defaults succeed!\r\n");
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
const ConfigParamMap& Config::getCfgMap()
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    return m_cfgMap;
}

//------------------------------------------------------------------------------
bool Config::save(const ConfigData& sysCfg)
{
    rtos::LockGuard<rtos::Mutex> lk(m_cfgMtx);
    // Check if sysCfg are upto date, do not write them
    if (memcmp(&sysCfg, &m_currentCfgData, sizeof(ConfigData)) == 0)
    {
        log::inf("Current Cfg is equal to provided sysCfg, NVS save skipped!\r\n");
        m_currentCfgData = sysCfg;
        return false;
    }
    // Set the NVS data ready for writing
    size_t ret = prefs.putBytes("syscfg", (const void*)&sysCfg, sizeof(sysCfg));

    // Write the data to NVS
    if (ret == sizeof(sysCfg))
    {
        log::inf("SysCfg saved in NVS succesfully\r\n");
        m_currentCfgData = sysCfg;
        return true;
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
        log::inf("SysCfg read from NVS succesfully\r\n");
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
        .version = 0x00000007,
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
        {"CET", Last, Sun, Oct, 3, 60},  // Central European Standard Time
        {"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
        true,
        0,
        (1000*3600),
        "time.google.com",
        "pl.pool.ntp.org",
        "pool.ntp.org"}
    };
    m_defaultCfgData.time = time;

//     // WEATHER
//     WeatherSystemConfig weatherCfg;
// #ifdef DEV_CFG
//     memcpy(weatherCfg.owmAppid, DEV_CFG_WEATHER_OWM_APPID, OWM_APPID_MAXLEN);
// #endif
//     m_defaultCfgData.weather = weatherCfg;

//     // RADIO_SENSOR
//     RadioSensorSystemConfig radioSensorCfg = {1800, 3000, 1, 5};
//     m_defaultCfgData.radioSensor = radioSensorCfg;

//     // INTERNAL ENVIRONMENT DATA
//     InternalEnvDataSystemConfig intEnvCfg = {900, 1};
//     m_defaultCfgData.intEnv = intEnvCfg;

//     // DISPLAY
//     DisplaySystemConfig displayCfg = {false, 0, 30, 1};
//     m_defaultCfgData.display = displayCfg;

//     // OTHER
//     AppSystemConfig otherCfg = { I18N_POLISH };
//     m_defaultCfgData.other = otherCfg;

}

//------------------------------------------------------------------------------
void Config::printCurrentSysCfg()
{
    log::inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
    log::inf("SysCfg size: %d bytes", sizeof(getCurrent()));
    log::inf("magic: 0x%08x", getCurrent().magic);
    log::inf("version: 0x%08x", getCurrent().version);
    // log::inf(getCurrent().wifi.toStr().c_str());
    // log::inf(getCurrent().time.toStr().c_str());
    // log::inf(getCurrent().weather.toStr().c_str());
    // log::inf(getCurrent().radioSensor.toStr().c_str());
    // log::inf(getCurrent().intEnv.toStr().c_str());
    // log::inf(getCurrent().display.toStr().c_str());
    // log::inf(getCurrent().other.toStr().c_str());
    log::inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
}

//------------------------------------------------------------------------------
