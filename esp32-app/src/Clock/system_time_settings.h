#pragma once

#include <ArduinoJson.h>
#include <Timezone.h>
#include "App/utils.h"
#include <cstdint>
#include <string>
#include <stdexcept>


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

    String toStr()
    {
        return "ntpEnabled:" + String(ntpEnabled) + " tO:" + String(String(timeOffset) + " uI:" + String(updateInterval) + " pSN0:" + String(poolServerNames[0]) + \
                      " pSN1:" + String(poolServerNames[1]) + " pSN2:" + String(poolServerNames[2]));
    }

    uint32_t ntpEnabled;
    int32_t timeOffset;
    uint32_t updateInterval;
    char poolServerNames[3][NTP_SERVER_NAME_MAXLEN];
};


/**
 * @brief System Time settings
 */
struct SystemTimeSettings
{
    #define TIME_CFG_KEY                    "dev-cfg-time"
    #define TIME_CFG_VAL_DATE               "time-date"
    #define TIME_CFG_VAL_CLK                "time-clock"
    #define TIME_CFG_VAL_TZ_NUM             "time-timezone-num"
    #define TIME_CFG_VAL_TZ_1               "time-timezone-1"
    #define TIME_CFG_VAL_TZ_2               "time-timezone-2"
    #define TIME_CFG_VAL_NTP_ON             "time-ntp-enable"
    #define TIME_CFG_VAL_NTP_TIME_OFFSET    "time-ntp-time-offset"
    #define TIME_CFG_VAL_NTP_SYNC_INT       "time-ntp-sync-interval"


    std::string toJson()
    {
        StaticJsonDocument<256> doc;
        std::string json;
        JsonArray arr = doc.createNestedArray(TIME_CFG_KEY);
        JsonObject obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_DATE] = "2020-11-20";
        obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_CLK] = "11:20";
        obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_TZ_NUM] = timezoneNum;
        obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_TZ_1] = timeChangeRuleToServerStr(stdStart);
        obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_TZ_2] = timeChangeRuleToServerStr(dstStart);
        obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_NTP_ON] = ntp.ntpEnabled;
        obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_NTP_TIME_OFFSET] = ntp.timeOffset;
        obj = arr.createNestedObject();
        obj[TIME_CFG_VAL_NTP_SYNC_INT] = ntp.updateInterval;
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
                    utils::err("Invalid settings for Timezone1");
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
                    utils::err("Invalid settings for Timezone1");
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
            utils::err("Not enough data in TZ cfg");
            return false;
        }
        utils::inf("TZ cfg: ");
        for (auto& d : data)
        {
            utils::inf("%s", d.c_str());
        }

        // Check values
        const int week = String(data[1].c_str()).toInt();
        if (week < Last || week > Fourth)
        {
            return false;
        }
        const int dow = String(data[2].c_str()).toInt();
        if (dow < Sun || dow > Sat)
        {
            return false;
        }
        const int month = String(data[3].c_str()).toInt();
        if (month < Jan || month > Dec)
        {
            return false;
        }
        const int hour = String(data[4].c_str()).toInt();
        if (hour < 0 || hour > 23)
        {
            return false;
        }
        const int offset = String(data[5].c_str()).toInt();
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
        s += tz.abbrev + '|';
        s += tz.week + '|';
        s += tz.dow + '|';
        s += tz.month + '|';
        s += tz.hour + '|';
        s += tz.offset + ']';
        return s;
    }

    String toStr()
    {
        return "timezoneNum:" + String(timezoneNum) + " stdTimezoneStart:" + stdStart.toStr() + \
                String(" dstTimezoneStart:" + dstStart.toStr() + "NtpCfg:" + ntp.toStr());
    }


    uint32_t timezoneNum;
    TimeChangeRule stdStart; // CET  - Central European Time (UTC+1)
    TimeChangeRule dstStart; // CEST - Central European daylight saving time (summer UTC+2)
    NtpSettings ntp;
};