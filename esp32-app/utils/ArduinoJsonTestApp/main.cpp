
#include <bits/stdc++.h>

/// ArduinoJson-v6.17.2.h, taken from:  https://github.com/bblanchon/ArduinoJson/releases/tag/v6.17.2
#include "ArduinoJson-v6.17.2.h"


//------------------------------------------------------------------------------
static void test1()
{
    std::cout << "<<< TEST 1 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "{\"sensor\":\"gps\",\"time\":1234567890,\"data\":[{\"lo\":48.756080, \"la\":2.302038}]}";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values
    const char* sensor = doc["sensor"];
    long time = doc["time"];
    JsonArray arr = doc["data"].as<JsonArray>();
    double latitude = arr[0]["lo"];
    double longitude = doc["data"][0]["lo"];

    // Print values.
    std::cout << sensor << std::endl;
    std::cout << time << std::endl;
    std::cout << latitude << std::endl;
    std::cout << longitude << std::endl;

    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test2()
{
    std::cout << "<<< TEST 2 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "[{\"lo\":48.756080, \"la\":2.302038}]";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values
    JsonArray arr = doc.as<JsonArray>();
    double latitude = arr[0]["lo"];
    double longitude = doc[0]["la"];

    // Print values.
    std::cout << latitude << std::endl;
    std::cout << longitude << std::endl;

    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test3()
{
    std::cout << "<<< TEST 3 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "[{\"lo\":48.756080, \"la\":2.302038}, {\"lo\":12.7, \"la\":34.30}]";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values and print values.
    JsonArray arr = doc.as<JsonArray>();
    for (const auto& v : arr)
    {
        double latitude = v["lo"];
        double longitude = v["la"];
        std::cout << latitude << std::endl;
        std::cout << longitude << std::endl;
    }
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test4()
{
    std::cout << "<<< TEST 4 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "{\"dev-cfg-wifi\" : [{\"lo\":48.756080, \"la\":2.302038}, {\"lo\":12.7, \"la\":34.30}]}";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values and print values.
    JsonArray arr = doc["dev-cfg-wifi"].as<JsonArray>();
    for (const auto& v : arr)
    {
        double latitude = v["lo"];
        double longitude = v["la"];
        std::cout << latitude << std::endl;
        std::cout << longitude << std::endl;
    }

    JsonVariant var = doc.as<JsonVariant>();
    JsonObject jsonObj = var.as<JsonObject>();
    const char* key = "dev-cfg-wifi";
    if (jsonObj.containsKey(key))
    {
        std::cout << "It contains the key: "<< key << std::endl;
    }
    else
    {
        std::cout << "No key: " << "found" << std::endl;
    }
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test5()
{
    std::cout << "<<< TEST 5 >>>" << std::endl;

    struct WifiSettings
    {
        #define WIFI_CFG_KEY            "dev-cfg-wifi"
        #define WIFI_CFG_VAL_SSID       "wifi-ssid"
        #define WIFI_CFG_VAL_PASS       "wifi-pass"
        #define WIFI_CFG_VAL_AP_PASS    "wifi-ap-pass"

        #define WIFI_SETTINGS_LEN 32

        WifiSettings()
        {
            const char *admin = "admin";
            memset(this, 0, sizeof(*this));
            memcpy(ssid, admin, WIFI_SETTINGS_LEN);
            memcpy(pass, admin, WIFI_SETTINGS_LEN);
            memcpy(ap_pass, admin, WIFI_SETTINGS_LEN);
        }

        std::string toStr()
        {
            return "ssid :" + std::string(std::string(ssid) + " pass:" + std::string(pass) + " ap-pass:" + std::string(ap_pass));
        }

        std::string toJson()
        {
            StaticJsonDocument<256> doc;
            std::string json;
            JsonArray arr = doc.createNestedArray(WIFI_CFG_KEY);
            JsonObject obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_SSID] = ssid;
            obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_PASS] = pass;
            obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_AP_PASS] = ap_pass;
            serializeJson(doc, json);
            return json;
        }

        void fromJson(const JsonObject& json)
        {
            JsonArray arr = json[WIFI_CFG_KEY].as<JsonArray>();
            for (const auto& v : arr)
            {
                if (v[WIFI_CFG_VAL_SSID])
                {
                    memset(ssid, 0, WIFI_SETTINGS_LEN);
                    memcpy(ssid, v[WIFI_CFG_VAL_SSID].as<const char*>(), WIFI_SETTINGS_LEN);
                }
                else if (v[WIFI_CFG_VAL_PASS])
                {
                    memset(pass, 0, WIFI_SETTINGS_LEN);
                    memcpy(pass, v[WIFI_CFG_VAL_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
                }
                else if (v[WIFI_CFG_VAL_AP_PASS])
                {
                    memset(ap_pass, 0, WIFI_SETTINGS_LEN);
                    memcpy(ap_pass, v[WIFI_CFG_VAL_AP_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
                }
            }
        }

        char ssid[WIFI_SETTINGS_LEN];
        char pass[WIFI_SETTINGS_LEN];
        char ap_pass[WIFI_SETTINGS_LEN];
    };

    WifiSettings cfg;

    std::cout << "toJson(): "<< cfg.toJson() << std::endl;

    StaticJsonDocument<300> doc;


    DeserializationError error = deserializeJson(doc, cfg.toJson().c_str());
    //char json[] = "{\"dev-cfg-wifi\":[{\"wifi-ssid\":\"INTEHNExx\"},{\"wifi-pass\":\"Faza19xxxx\"},{\"wifi-ap-pass\":\"admin123\"}]}";
    //DeserializationError error = deserializeJson(doc, json);

    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    std::cout << "BEFORE fromJson(): "<< cfg.toStr() << std::endl;
    cfg.fromJson(doc.as<JsonObject>());
    std::cout << "AFTER fromJson(): "<< cfg.toStr() << std::endl;


    JsonVariant var = doc.as<JsonVariant>();
    JsonObject jsonObj = var.as<JsonObject>();
    const char* key = "dev-cfg-wifi";
    if (jsonObj.containsKey(key))
    {
        std::cout << "It contains the key: "<< key << std::endl;
    }
    else
    {
        std::cout << "No key: " << "found" << std::endl;
    }
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test6()
{
    std::cout << "<<< TEST 6 >>>" << std::endl;

///> WIFI SETTINGS
    #define WIFI_CFG_KEY                "dev-cfg-wifi"
    #define WIFI_CFG_VAL_SSID           "wifi-ssid"
    #define WIFI_CFG_VAL_PASS           "wifi-pass"
    #define WIFI_CFG_VAL_AP_HOSTNAME    "wifi-ap-hostname"
    #define WIFI_CFG_VAL_AP_PASS        "wifi-ap-pass"

    struct WifiSettings
    {

        #define WIFI_SETTINGS_LEN 32

        WifiSettings()
        {
            const char *admin = "admin";
            memset(this, 0, sizeof(*this));
            memcpy(ssid, admin, WIFI_SETTINGS_LEN);
            memcpy(pass, admin, WIFI_SETTINGS_LEN);
            memcpy(ap_hostname, "ledclock", WIFI_SETTINGS_LEN);
            memcpy(ap_pass, admin, WIFI_SETTINGS_LEN);
        }

        std::string toStr()
        {
            const std::string colon = ":";
            const std::string comma =", ";
            return std::string(WIFI_CFG_KEY)+colon+comma + \
                    std::string(WIFI_CFG_VAL_SSID)+colon+std::string(ssid)+comma + \
                    std::string(WIFI_CFG_VAL_PASS)+colon+std::string(pass)+comma  + \
                    std::string(WIFI_CFG_VAL_AP_HOSTNAME)+colon+std::string(ap_hostname)+comma  + \
                    std::string(WIFI_CFG_VAL_AP_PASS)+colon+std::string(ap_pass);
        }

        std::string toJson()
        {
            StaticJsonDocument<512> doc;
            std::string json;
            JsonArray arr = doc.createNestedArray(WIFI_CFG_KEY);
            JsonObject obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_SSID] = ssid;
            obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_PASS] = pass;
            obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_AP_HOSTNAME] = ap_hostname;
            obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_AP_PASS] = ap_pass;
            serializeJson(doc, json);
            return json;
        }

        void fromJson(const JsonObject& json)
        {
            JsonArray arr = json[WIFI_CFG_KEY].as<JsonArray>();
            for (const auto& v : arr)
            {
                if (v[WIFI_CFG_VAL_SSID])
                {
                    memset(ssid, 0, WIFI_SETTINGS_LEN);
                    memcpy(ssid, v[WIFI_CFG_VAL_SSID].as<const char*>(), WIFI_SETTINGS_LEN);
                }
                else if (v[WIFI_CFG_VAL_PASS])
                {
                    memset(pass, 0, WIFI_SETTINGS_LEN);
                    memcpy(pass, v[WIFI_CFG_VAL_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
                }
                else if (v[WIFI_CFG_VAL_AP_HOSTNAME])
                {
                    memset(ap_pass, 0, WIFI_SETTINGS_LEN);
                    memcpy(ap_pass, v[WIFI_CFG_VAL_AP_HOSTNAME].as<const char*>(), WIFI_SETTINGS_LEN);
                }
                else if (v[WIFI_CFG_VAL_AP_PASS])
                {
                    memset(ap_pass, 0, WIFI_SETTINGS_LEN);
                    memcpy(ap_pass, v[WIFI_CFG_VAL_AP_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
                }
            }
        }

        char ssid[WIFI_SETTINGS_LEN];
        char pass[WIFI_SETTINGS_LEN];
        char ap_hostname[WIFI_SETTINGS_LEN];
        char ap_pass[WIFI_SETTINGS_LEN];
    };


///> RADIO SETTINGS
    #define RADIO_CFG_KEY                   "dev-cfg-radio"
    #define RADIO_CFG_VAL_UPDATE_INTERVAL   "radio-update-interval"
    #define RADIO_CFG_VAL_CRIT_VBATT        "radio-crit-vbatt"

    /**
     * @brief RadiSensor global settings structure
     */
    struct RadioSensorSettings
    {
        std::string toJson()
        {
            StaticJsonDocument<256> doc;
            std::string json;
            JsonArray arr = doc.createNestedArray(RADIO_CFG_KEY);
            JsonObject obj = arr.createNestedObject();
            obj[RADIO_CFG_VAL_UPDATE_INTERVAL] = update_data_interval / 60; // Convert from seconds to minutes on server.
            obj = arr.createNestedObject();
            obj[RADIO_CFG_VAL_CRIT_VBATT] = crit_vbatt_level;
            serializeJson(doc, json);
            return json;
        }

        void fromJson(const JsonObject& json)
        {
            JsonArray arr = json[RADIO_CFG_KEY].as<JsonArray>();
            for (const auto& v : arr)
            {
                if (v[RADIO_CFG_VAL_UPDATE_INTERVAL])
                {
                    update_data_interval = v[RADIO_CFG_VAL_UPDATE_INTERVAL].as<uint32_t>();
                    // Convert from minutes on server to seconds
                    update_data_interval = update_data_interval * 60;
                }
                else if (v[RADIO_CFG_VAL_CRIT_VBATT])
                {
                    crit_vbatt_level = v[RADIO_CFG_VAL_CRIT_VBATT].as<uint32_t>();
                }
            }
        }

        std::string toStr()
        {
            const std::string colon = ":";
            const std::string comma =", ";
            return std::string(RADIO_CFG_KEY)+colon+comma + \
                    std::string(RADIO_CFG_VAL_UPDATE_INTERVAL)+colon+std::to_string(update_data_interval)+comma + \
                    std::string(RADIO_CFG_VAL_CRIT_VBATT)+colon+std::to_string(crit_vbatt_level);
        }


        uint32_t update_data_interval; //[s] in seconds
        uint32_t crit_vbatt_level;     //[mV]
    };


///> TIME_SETTINGS
    #define TIME_CFG_KEY                    "dev-cfg-time"
    #define TIME_CFG_VAL_DATE               "time-date"
    #define TIME_CFG_VAL_CLK                "time-clock"
    #define TIME_CFG_VAL_TZ_NUM             "time-timezone-num"
    #define TIME_CFG_VAL_TZ_1               "time-timezone-1"
    #define TIME_CFG_VAL_TZ_2               "time-timezone-2"
    #define TIME_CFG_VAL_NTP_ON             "time-ntp-enable"
    #define TIME_CFG_VAL_NTP_TIME_OFFSET    "time-ntp-time-offset"
    #define TIME_CFG_VAL_NTP_SYNC_INT       "time-ntp-sync-interval"
    #define TIME_CFG_VAL_NTP_SERVER_1       "time-ntp-server-1"
    #define TIME_CFG_VAL_NTP_SERVER_2       "time-ntp-server-2"
    #define TIME_CFG_VAL_NTP_SERVER_3       "time-ntp-server-3"

    struct NtpSettings
    {
        #define NTP_SERVER_NAME_MAXLEN 32

        NtpSettings()
            : ntpEnabled(true)
            , timeOffset(0)
            , updateInterval(1000*3600) // 1h (in miliseconds)
        {
            memcpy(poolServerNames[0], "time.google.com", NTP_SERVER_NAME_MAXLEN);
            memcpy(poolServerNames[1], "pl.pool.ntp.org", NTP_SERVER_NAME_MAXLEN);
            memcpy(poolServerNames[2], "pool.ntp.org",    NTP_SERVER_NAME_MAXLEN);
        }

        NtpSettings(bool enabled, int32_t tO, uint32_t uI, const char *server0Name,
                    const char *server1Name=nullptr, const char *server2Name=nullptr)
            : ntpEnabled(enabled)
            , timeOffset(tO)
            , updateInterval(uI)
        {
            memset(poolServerNames[0], 0, NTP_SERVER_NAME_MAXLEN);
            memset(poolServerNames[1], 0, NTP_SERVER_NAME_MAXLEN);
            memset(poolServerNames[2], 0, NTP_SERVER_NAME_MAXLEN);
            if (server0Name)
            {
                memcpy(poolServerNames[0], server0Name, strlen(server0Name));
            }
            if (server1Name)
            {
                memcpy(poolServerNames[1], server1Name, NTP_SERVER_NAME_MAXLEN);
            }
            if (server2Name)
            {
                memcpy(poolServerNames[2], server2Name, NTP_SERVER_NAME_MAXLEN);
            }
        }

        NtpSettings(const NtpSettings& other)
            : ntpEnabled(true)
            , timeOffset(other.timeOffset)
            , updateInterval(other.updateInterval)
        {
            memcpy(poolServerNames[0], other.poolServerNames[0], NTP_SERVER_NAME_MAXLEN);
            memcpy(poolServerNames[1], other.poolServerNames[1], NTP_SERVER_NAME_MAXLEN);
            memcpy(poolServerNames[2], other.poolServerNames[2], NTP_SERVER_NAME_MAXLEN);
        }

        NtpSettings& operator=(const NtpSettings& other)
        {
            timeOffset = other.timeOffset;
            updateInterval = other.updateInterval;
            memcpy(this->poolServerNames[0], other.poolServerNames[0], NTP_SERVER_NAME_MAXLEN);
            memcpy(this->poolServerNames[1], other.poolServerNames[1], NTP_SERVER_NAME_MAXLEN);
            memcpy(this->poolServerNames[2], other.poolServerNames[2], NTP_SERVER_NAME_MAXLEN);
            return *this;
        }

        std::string toStr()
        {
            const std::string colon = ":";
            const std::string comma =", ";
            return std::string("NtpCfg")+colon+comma + \
                    std::string(TIME_CFG_VAL_NTP_ON)+colon+std::to_string(ntpEnabled)+comma + \
                    std::string(TIME_CFG_VAL_NTP_TIME_OFFSET)+colon+std::to_string(timeOffset)+comma  + \
                    std::string(TIME_CFG_VAL_NTP_SYNC_INT)+colon+std::to_string(updateInterval)+comma  + \
                    std::string(TIME_CFG_VAL_NTP_SERVER_1)+colon+std::string(poolServerNames[0])+comma + \
                    std::string(TIME_CFG_VAL_NTP_SERVER_2)+colon+std::string(poolServerNames[1])+comma + \
                    std::string(TIME_CFG_VAL_NTP_SERVER_3)+colon+std::string(poolServerNames[2]);
        }

        uint32_t ntpEnabled;
        int32_t timeOffset;      // in [min]
        uint32_t updateInterval; // in [ms]
        char poolServerNames[3][NTP_SERVER_NAME_MAXLEN];
    };


    // convenient constants for TimeChangeRules
    enum week_t {Last, First, Second, Third, Fourth};
    enum dow_t {Sun=1, Mon, Tue, Wed, Thu, Fri, Sat};
    enum month_t {Jan=1, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec};
    // structure to describe rules for when daylight/summer time begins,
    // or when standard time begins.
    struct TimeChangeRule
    {
        char abbrev[6];    // five chars max
        uint8_t week;      // First, Second, Third, Fourth, or Last week of the month
        uint8_t dow;       // day of week, 1=Sun, 2=Mon, ... 7=Sat
        uint8_t month;     // 1=Jan, 2=Feb, ... 12=Dec
        uint8_t hour;      // 0-23
        int offset;        // offset from UTC in minutes

        std::string toStr()
        {
            return std::string("abbrev:") + std::string(abbrev) + std::string(" week:") + std::to_string(week) + std::string(" dow:") + std::to_string(dow) + \
                        std::string(" month:") + std::to_string(month) + std::string(" hour:") + std::to_string(hour) + std::string(" offset:") + std::to_string(offset);
        }
    };

    /**
     * @brief System Time settings
     */
    struct SystemTimeSettings
    {
        std::string toJson()
        {
            StaticJsonDocument<512+128> doc;
            std::string json;
            JsonArray arr = doc.createNestedArray(TIME_CFG_KEY);
            JsonObject obj = arr.createNestedObject();
            obj[TIME_CFG_VAL_DATE] = "2020-11-20";
            obj = arr.createNestedObject();
            obj[TIME_CFG_VAL_CLK] = "11:20";
            obj = arr.createNestedObject();
            obj[TIME_CFG_VAL_TZ_NUM] = timezoneNum;
            obj = arr.createNestedObject();
            const std::string tz1 = timeChangeRuleToServerStr(stdStart);
            obj[TIME_CFG_VAL_TZ_1] = tz1.c_str();
            obj = arr.createNestedObject();
            const std::string tz2 = timeChangeRuleToServerStr(dstStart);
            obj[TIME_CFG_VAL_TZ_2] = tz2.c_str();
            obj = arr.createNestedObject();
            obj[TIME_CFG_VAL_NTP_ON] = ntp.ntpEnabled;
            obj = arr.createNestedObject();
            obj[TIME_CFG_VAL_NTP_TIME_OFFSET] = ntp.timeOffset;
            obj = arr.createNestedObject();
            obj[TIME_CFG_VAL_NTP_SYNC_INT] = ntp.updateInterval / 1000 / 60; // Convert from ms to minutes on server
            serializeJson(doc, json);
            return json;
        }

        void fromJson(const JsonObject& json)
        {
            JsonArray arr = json[TIME_CFG_KEY].as<JsonArray>();
            for (const auto& v : arr)
            {
                if (v[TIME_CFG_VAL_TZ_NUM])
                {
                    timezoneNum =  v[TIME_CFG_VAL_TZ_NUM].as<uint32_t>();
                }
                else if (v[TIME_CFG_VAL_TZ_1])
                {
                    std::string tz(v[TIME_CFG_VAL_TZ_1].as<const char*>());
                    // Validate
                    TimeChangeRule tzRule;
                    if (validateAndParseTzCfgStr(tz, tzRule))
                    {
                        stdStart = tzRule;
                    }
                    else
                    {
                        std::cout <<("Invalid settings for Timezone1");
                    }
                }
                else if (v[TIME_CFG_VAL_TZ_2])
                {
                    std::string tz(v[TIME_CFG_VAL_TZ_2].as<const char*>());
                    // Validate
                    TimeChangeRule tzRule;
                    if (validateAndParseTzCfgStr(tz, tzRule))
                    {
                        dstStart = tzRule;
                    }
                    else
                    {
                        std::cout <<("Invalid settings for Timezone2");
                    }
                }
                else if (v[TIME_CFG_VAL_NTP_ON])
                {
                    ntp.ntpEnabled = v[TIME_CFG_VAL_NTP_ON].as<uint32_t>();
                }
                else if (v[TIME_CFG_VAL_NTP_TIME_OFFSET])
                {
                    ntp.timeOffset = v[TIME_CFG_VAL_NTP_TIME_OFFSET].as<int32_t>();
                }
                else if (v[TIME_CFG_VAL_NTP_SYNC_INT])
                {
                    ntp.updateInterval = v[TIME_CFG_VAL_NTP_SYNC_INT].as<uint32_t>();
                    // Convert from minutes on server to ms
                    ntp.updateInterval = ntp.updateInterval * 60 * 1000;
                }
            }
        }

        bool validateAndParseTzCfgStr(const std::string& s, TimeChangeRule& tz)
        {
            // Validate
            std::string delim = "|";
            if (s.size() == 0)
            {
                return false;
            }
            if (s[0] != '[' || s[s.size()-1] != ']')
            {
                return false;
            }

            auto start = 1U;
            auto end = s.find(delim);
            std::string str = s;
            str[0] = '|';
            str[str.size()-1] = '|';
            std::vector<std::string> data;
            while (end != std::string::npos)
            {
                data.push_back(str.substr(start, end-start));
                start = end + delim.length();
                end = str.find(delim, start);
            }
            data.push_back(str.substr(start, end));

            if (data.size() != 6)
            {
                std::cout << ("Not enough data in TZ cfg");
                return false;
            }
            std::cout <<("TZ cfg:");
            for (auto& d : data)
            {
                std::cout <<(d.c_str());
            }

            // Check values
            const int week = std::stoi(std::string(data[1].c_str()));
            if (week < Last || week > Fourth)
            {
                return false;
            }
            const int dow = std::stoi(std::string(data[2].c_str()));
            if (dow < Sun || dow > Sat)
            {
                return false;
            }
            const int month = std::stoi(std::string(data[3].c_str()));
            if (month < Jan || month > Dec)
            {
                return false;
            }
            const int hour = std::stoi(std::string(data[4].c_str()));
            if (hour < 0 || hour > 23)
            {
                return false;
            }
            const int offset = std::stoi(std::string(data[5].c_str()));
            if (offset < 0 || offset > 23)
            {
                return false;
            }

            // Set values in memory
            memcpy(tz.abbrev, data[0].c_str(), 6);
            tz.week = week;
            tz.dow = dow;
            tz.month = month;
            tz.hour = tz.hour;
            tz.offset = offset;
            return true;
        }

        std::string timeChangeRuleToServerStr(const TimeChangeRule& tz)
        {
            std::string s;
            s += '[';
            s += std::string(tz.abbrev) + '|';
            s += std::to_string(tz.week) + '|';
            s += std::to_string(tz.dow) + '|';
            s += std::to_string(tz.month) + '|';
            s += std::to_string(tz.hour) + '|';
            s += std::to_string(tz.offset) + ']';
            return s;
        }

        std::string toStr()
        {
            const std::string colon = ":";
            const std::string comma =", ";
            return std::string(TIME_CFG_KEY)+colon+comma + \
                    std::string(TIME_CFG_VAL_TZ_NUM)+colon+std::to_string(timezoneNum)+comma + \
                    std::string(TIME_CFG_VAL_TZ_1)+colon+std::string(stdStart.toStr())+comma  + \
                    std::string(TIME_CFG_VAL_TZ_2)+colon+std::string(dstStart.toStr())+comma  + \
                    ntp.toStr();
        }


        uint32_t timezoneNum;
        TimeChangeRule stdStart; // CET  - Central European Time (UTC+1)
        TimeChangeRule dstStart; // CEST - Central European daylight saving time (summer UTC+2)
        NtpSettings ntp;
    };


///> TEST - START
    WifiSettings wifi;
    RadioSensorSettings radio = {60, 3000};
    SystemTimeSettings time = {
        2,
        {"CET ", Last, Sun, Oct, 3, 60},  // Central European Standard Time
        {"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
        {true, 0, (1000*3600), "time.google.com", "pl.pool.ntp.org", "pool.ntp.org"}
    };;

    std::cout << "wifi.toJson(): "<< wifi.toJson() << std::endl;
    std::cout << "radio.toJson(): "<< radio.toJson() << std::endl;
    std::cout << "time.toJson(): "<< time.toJson() << std::endl;

/// MERGE JSONS
    std::function<void(JsonObject, JsonObjectConst)> mergeJson = [](JsonObject dest, JsonObjectConst src)
    {
        for (auto kvp : src) {
            dest[kvp.key()] = kvp.value();
        }
    };
    StaticJsonDocument<1024> appCfg, dw, dr, dt;
    //char json[] = "{\"dev-cfg-wifi\":[{\"wifi-ssid\":\"INTEHNExx\"},{\"wifi-pass\":\"Faza19xxxx\"},{\"wifi-ap-pass\":\"admin123\"}]}";
    deserializeJson(dw, wifi.toJson().c_str());
    deserializeJson(dr, radio.toJson().c_str());
    deserializeJson(dt, time.toJson().c_str());
    mergeJson(appCfg.as<JsonObject>(), dw.as<JsonObject>());
    mergeJson(appCfg.as<JsonObject>(), dr.as<JsonObject>());
    mergeJson(appCfg.as<JsonObject>(), dt.as<JsonObject>());
    serializeJsonPretty(appCfg, std::cout);
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 0;
}
