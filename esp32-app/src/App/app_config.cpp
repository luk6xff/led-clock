#include "app_config.h"
#include "utils.h"

#define DEV_CFG // FOR DEV TESTS ONLY

#ifdef DEV_CFG
    #include "app_dev_cfg.h"
#endif



//------------------------------------------------------------------------------
AppConfig::AppConfig()
{
    // Set defaults into App
    setDefaults();
}

//------------------------------------------------------------------------------
AppConfig& AppConfig::instance()
{
    static AppConfig appCfg;
    return appCfg;
}

//------------------------------------------------------------------------------
void AppConfig::init()
{
    // The begin() call is required to initialise the NVS library
    prefs.begin("ledclock", false);

    readSettings();

    // Print content
    printCurrentSettings();

    // Check if settings are valid
    if (getCurrent().magic == getDefaults().magic && \
        getCurrent().version == getDefaults().version)
    {
        utils::inf("AppCfg settings read succesfully\r\n");
    }
    else
    {
        utils::inf("AppCfg settings read failed, updating with defaults..\r\n");
        if (!saveSettings(getDefaults()))
        {
            utils::inf("Updating AppCfg settings with defaults failed!, setting current as defaults\r\n");
            currentSettings = defaultSettings;
        }
        else
        {
            utils::inf("Updating AppCfg settings with defaults succeed!\r\n");
        }
    }
}

//------------------------------------------------------------------------------
void AppConfig::close()
{
    prefs.end();
}

//------------------------------------------------------------------------------
const AppConfig::Settings& AppConfig::getDefaults()
{
    rtos::LockGuard<rtos::Mutex> lk(settingsMtx);
    return defaultSettings;
}

//------------------------------------------------------------------------------
AppConfig::Settings& AppConfig::getCurrent()
{
    rtos::LockGuard<rtos::Mutex> lk(settingsMtx);
    return currentSettings;
}

//------------------------------------------------------------------------------
bool AppConfig::storeWifiData(const WifiSettings& ws)
{
    bool ret = true;
    Settings newSettings = getCurrent();
    newSettings.wifi = ws;
    ret = saveSettings(newSettings);
    return ret;
}

//------------------------------------------------------------------------------
bool AppConfig::saveSettings(const Settings& settings)
{
    rtos::LockGuard<rtos::Mutex> lk(settingsMtx);
    // Set the NVS data ready for writing
    size_t ret = prefs.putBytes("settings", (const void*)&settings, sizeof(settings));

    // Write the data to NVS
    if (ret == sizeof(settings))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool AppConfig::readSettings()
{
    rtos::LockGuard<rtos::Mutex> lk(settingsMtx);
    size_t ret = prefs.getBytes("settings", &currentSettings, sizeof(currentSettings));
    if (ret == sizeof(currentSettings))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
void AppConfig::setDefaults()
{
    rtos::LockGuard<rtos::Mutex> lk(settingsMtx);
    // Modify according to your application
    // MAGIC
    defaultSettings =
    {
        .magic   = 0x4C554B36,  // LUK6
        .version = 0x00000000,
    };

    // WIFI
    WifiSettings wifiCfg;
#ifdef DEV_CFG
    memcpy(wifiCfg.ssid, DEV_CFG_WIFI_SSID, WIFI_SETTINGS_LEN);
    memcpy(wifiCfg.pass, DEV_CFG_WIFI_PASS, WIFI_SETTINGS_LEN);
#endif
    defaultSettings.wifi = wifiCfg;

    // SYSTEM TIME
    SystemTimeSettings timeCfg = {
        {"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
        {"CET ", Last, Sun, Oct, 3, 60},  // Central European Standard Time
    };
    defaultSettings.time = timeCfg;

    // NTP
    NtpSettings ntpCfg(0, (1000*3600), "time.google.com", "pl.pool.ntp.org", "pool.ntp.org");
    defaultSettings.ntp = ntpCfg;

    // WEATHER
    WeatherSettings weatherCfg;
#ifdef DEV_CFG
    memcpy(weatherCfg.owmAppid, DEV_CFG_WEATHER_OWM_APPID, OWM_APPID_MAXLEN);
#endif
    defaultSettings.weather = weatherCfg;

    // RADIO_SENSOR
    RadioSensorSettings radioSensorCfg = {
        .crit_vbatt_level = 3000,
        .update_data_interval = 60,
    };
    defaultSettings.radioSensor = radioSensorCfg;
}

//------------------------------------------------------------------------------
void AppConfig::printCurrentSettings()
{
    utils::inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
    utils::inf("AppCfg size: %d bytes", sizeof(getCurrent()));
    utils::inf("magic: 0x%08x", getCurrent().magic);
    utils::inf("version: 0x%08x", getCurrent().version);
    utils::inf("wifi: %s %s", getCurrent().wifi.ssid, getCurrent().wifi.pass);
    utils::inf("systime: %s", getCurrent().time.toStr().c_str());
    utils::inf("ntp: %s", getCurrent().ntp.toStr().c_str());
    utils::inf("weather: %s", getCurrent().weather.toStr().c_str());
    utils::inf("radio_sensor: %s", getCurrent().radioSensor.toStr().c_str());
    utils::inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
}

//------------------------------------------------------------------------------