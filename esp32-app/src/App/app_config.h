
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


// Define the version number, also used for webserver as Last-Modified header and to check version for update.
#define APP_VERSION     "0.0.1"

// LedClock App can be updated (OTA) to the latest version from a remote server.
#define APP_UPDATEHOST  "github.com"                    // Host for software updates
#define APP_BINFILE     "/firmware.bin"      // Binary file name for update software


#define AppCfg  AppConfig::instance()

class AppConfig
{

public:

    struct WifiSettings
    {
        WifiSettings()
        {
            const char *admin = "admin";
            memset(this, 0, sizeof(*this));
            memcpy(ssid0, admin, strlen(admin));
            memcpy(pass0, admin, strlen(admin));
            memcpy(ssid1, admin, strlen(admin));
            memcpy(pass1, admin, strlen(admin));
            memcpy(ssid2, admin, strlen(admin));
            memcpy(pass2, admin, strlen(admin));
        }

        #define WIFI_SETTINGS_LEN 20
        #define WIFI_SETTINGS_CH_NUM 3
        char ssid0[WIFI_SETTINGS_LEN];
        char pass0[WIFI_SETTINGS_LEN];
        char ssid1[WIFI_SETTINGS_LEN];
        char pass1[WIFI_SETTINGS_LEN];
        char ssid2[WIFI_SETTINGS_LEN];
        char pass2[WIFI_SETTINGS_LEN];
    };

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

    const Settings& getDefaults();
    Settings& getCurrent();

    // Settings options
    bool storeWifiData(AppConfig::WifiSettings& ws);

private:
    bool saveSettings(const Settings &settings);
    bool readSettings();
    void setDefaults();
    void printCurrentSettings();

private:
    Settings defaultSettings;
    Settings currentSettings;

    // NVS preferences
    Preferences prefs;
};