
#pragma once
/**
 * @brief App configuration stuff
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-18
 */
#include <Preferences.h>
#include "Wifi/wifi_settings.h"
#include "Clock/system_time_settings.h"
#include "Weather/weather_settings.h"
#include "Radio/radio_settings.h"
#include "Display/display_settings.h"
#include "app_settings.h"
#include "rtos_common.h"



// Define the version number, also used for webserver as Last-Modified header and to check version for update.
#define APP_VERSION     "0.0.1"

// LedClock App can be updated (OTA) to the latest version from a remote server.
#define APP_UPDATEHOST  "github.com"                    // Host for software updates
#define APP_BINFILE     "/firmware.bin"      // Binary file name for update software


#define AppCfg  AppConfig::instance()

class AppConfig
{

public:

    typedef struct
    {
        uint32_t magic;
        uint32_t version;
        WifiSettings wifi;
        SystemTimeSettings time;
        WeatherSettings weather;
        RadioSensorSettings radioSensor;
        DisplaySettings display;
        AppSettings other;
    } Settings;

public:
    AppConfig();
    static AppConfig& instance();
    void init();
    void close();

    const Settings& getDefaults();
    Settings& getCurrent();

    // Settings options savers
    bool saveWifiSettings(const WifiSettings& cfg);
    bool saveSystemTimeSettings(const SystemTimeSettings& cfg);
    bool saveWeatherSettings(const WeatherSettings& cfg);
    bool saveRadioSensorSettings(const RadioSensorSettings& cfg);
    bool saveDisplaySettings(const DisplaySettings& cfg);
    bool saveAppSettings(const AppSettings& cfg);

private:
    bool saveSettings(const Settings &settings);
    bool readSettings();
    void setDefaults();
    void printCurrentSettings();

private:
    Settings defaultSettings;
    Settings currentSettings;

    rtos::Mutex settingsMtx;

    // NVS preferences
    Preferences prefs;

};