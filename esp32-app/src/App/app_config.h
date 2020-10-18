
#pragma once
/**
 * @brief App configuration stuff
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-18
 */
#include <Preferences.h>
#include "Clock/system_time.h"
#include "Clock/ntp.h"
#include "wifi_task.h"
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
        NtpSettings ntp;
    } Settings;

public:
    AppConfig();
    static AppConfig& instance();
    void init();
    void close();

    const Settings& getDefaults();
    Settings& getCurrent();

    // Settings options
    bool storeWifiData(const WifiSettings& ws);

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