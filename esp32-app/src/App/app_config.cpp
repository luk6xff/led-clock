#include "app_config.h"
#include "utils.h"



//------------------------------------------------------------------------------
AppConfig::AppConfig()
{
    setDefaults();
}

//------------------------------------------------------------------------------
AppConfig& AppConfig::instance()
{
    static AppConfig settings;
    return settings;
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
        inf("AppCfg settings read succesfully\r\n");
    }
    else
    {
        inf("AppCfg settings read failed, updating with defaults..\r\n");
        if (!saveSettings(getDefaults()))
        {
            inf("Updating AppCfg settings with defaults failed!, setting current as defaults\r\n");
            currentSettings = defaultSettings;
        }
        else
        {
            inf("Updating AppCfg settings with defaults succeed!\r\n");
        }
    }
}

//------------------------------------------------------------------------------
const AppConfig::Settings& AppConfig::getDefaults()
{
    return defaultSettings;
}

//------------------------------------------------------------------------------
AppConfig::Settings& AppConfig::getCurrent()
{
    return currentSettings;
}

//------------------------------------------------------------------------------
bool AppConfig::storeWifiData(AppConfig::WifiSettings& ws)
{
    bool ret = true;
    Settings newSettings = getCurrent();
    ret = saveSettings(newSettings);
    return ret;
}

//------------------------------------------------------------------------------
bool AppConfig::saveSettings(const Settings& settings)
{
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
    // Modify according to your application
    // MAGIC
    defaultSettings =
    {
        .magic = 0x4C554B36,  // LUK6
        .version = 0x0000002,
    };

    // WIFI
    WifiSettings wifiDefault;
    defaultSettings.wifi = wifiDefault;

    // SYSTEM TIME
    SystemTimeSettings timeConfig = {
        {"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
        {"CET ", Last, Sun, Oct, 3, 60},  // Central European Standard Time
    };
    defaultSettings.time = timeConfig;

    // NTP
    defaultSettings.ntp = NtpSettings(0, (1000*3600), "time.google.com", "pl.pool.ntp.org", "pool.ntp.org");
}

//------------------------------------------------------------------------------
void AppConfig::printCurrentSettings()
{
    inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
    inf("magic: 0x%08x", getCurrent().magic);
    inf("version: 0x%08x", getCurrent().version);
    inf("wifi.0: %s %s", getCurrent().wifi.ssid0, getCurrent().wifi.pass0);
    inf("wifi.1: %s %s", getCurrent().wifi.ssid1, getCurrent().wifi.pass1);
    inf("wifi.2: %s %s", getCurrent().wifi.ssid2, getCurrent().wifi.pass2);
    inf("systime: %s", getCurrent().time.toStr().c_str());
    inf("ntp: %s", getCurrent().ntp.toStr().c_str());
    inf("APP_CONFIG: <<CURRENT APP SETTINGS>>");
}

//------------------------------------------------------------------------------