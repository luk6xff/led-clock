#include "TimeConfigParam.h"
#include "Rtos/logger.h"
#include "Config.h"

//------------------------------------------------------------------------------
TimeConfigParam::TimeConfigParam() : ConfigParam(CFG_KEY_TIME, Cfg)
{
    setCfgParamsMap();
}

//------------------------------------------------------------------------------
bool TimeConfigParam::setConfigFromJson(const JsonObject& json)
{
    TimeConfigData dataFromServer;
    memset(&dataFromServer, 0, sizeof(dataFromServer));

    if (unpackFromJson(dataFromServer, json))
    {
        logger::dbg("TimeConfigParam::unpackFromJson - SUCCESS");
        if (std::memcmp(&m_cfgData, &dataFromServer, cfgDataSize()) != 0)
        {
            logger::dbg("Storing new TimeConfigData settings");
            return m_cfgHndl.save(key(), &dataFromServer);
        }
        logger::dbg("Storing TimeConfigData settings skipped, no change detected");
        return false;
    }

    logger::err("TimeConfigParam::unpackFromJson - ERROR");
    return false;
}

//------------------------------------------------------------------------------
String TimeConfigParam::toStr()
{
    const String colon = ":";
    const String comma = ", ";
    return key()+colon+comma + \
            cfgParamKey(TimeKeys::TIME_TIMEZONES_NUMBER)+colon+String(m_cfgData.timezoneNum)+comma + \
            cfgParamKey(TimeKeys::TIME_TIMEZONE_1_UTC_OFFSET)+colon+String(m_cfgData.stdTimezone1UtcOffset)+comma  + \
            cfgParamKey(TimeKeys::TIME_TIMEZONE_2_UTC_OFFSET)+colon+String(m_cfgData.dstTimezone2UtcOffset)+comma  + \
            cfgParamKey(TimeKeys::TIME_NTP_ENABLED)+colon+String(m_cfgData.ntpEnabled)+comma + \
            cfgParamKey(TimeKeys::TIME_NTP_TIME_OFFSET)+colon+String(m_cfgData.ntpTimeOffset)+comma  + \
            cfgParamKey(TimeKeys::TIME_NTP_SYNC_INTERVAL)+colon+String(m_cfgData.ntpUpdateInterval)+comma  + \
            cfgParamKey(TimeKeys::TIME_NTP_SERVER_1)+colon+String(m_cfgData.ntpPoolServerNames[0])+comma  + \
            cfgParamKey(TimeKeys::TIME_NTP_SERVER_2)+colon+String(m_cfgData.ntpPoolServerNames[1])+comma  + \
            cfgParamKey(TimeKeys::TIME_NTP_SERVER_3)+colon+String(m_cfgData.ntpPoolServerNames[2]);
}

//------------------------------------------------------------------------------
void TimeConfigParam::setCfgParamsMap()
{
    m_cfgParamsMap = {
        { TimeKeys::TIME_TIMEZONES_NUMBER,      "time-timezone-num" },
        { TimeKeys::TIME_TIMEZONE_1_UTC_OFFSET, "time-timezone-1-utc-offset" },
        { TimeKeys::TIME_TIMEZONE_2_UTC_OFFSET, "time-timezone-2-utc-offset" },
        { TimeKeys::TIME_NTP_ENABLED,           "time-ntp-enable" },
        { TimeKeys::TIME_NTP_TIME_OFFSET,       "time-ntp-time-offset" },
        { TimeKeys::TIME_NTP_SYNC_INTERVAL,     "time-ntp-sync-interval" },
        { TimeKeys::TIME_NTP_SERVER_1,          "time-ntp-server-1" },
        { TimeKeys::TIME_NTP_SERVER_2,          "time-ntp-server-2" },
        { TimeKeys::TIME_NTP_SERVER_3,          "time-ntp-server-3" },
    };
}

//------------------------------------------------------------------------------
bool TimeConfigParam::unpackFromJson(TimeConfigData& cfgData, const JsonObject& json)
{
    // Not used so far
    bool ret = true;

    JsonArray arr = json[key()].as<JsonArray>();
    for (const auto& v : arr)
    {
        if (v[cfgParamKey(TimeKeys::TIME_TIMEZONES_NUMBER)])
        {
            cfgData.timezoneNum = v[cfgParamKey(TimeKeys::TIME_TIMEZONES_NUMBER)].as<uint32_t>();
            if (cfgData.timezoneNum == 0 || cfgData.timezoneNum > 2)
            {
                cfgData.timezoneNum = 1;
            }
        }
        else if (v[cfgParamKey(TimeKeys::TIME_TIMEZONE_1_UTC_OFFSET)])
        {
            int tzHoursOffset = v[cfgParamKey(TimeKeys::TIME_TIMEZONE_1_UTC_OFFSET)].as<int>();
            if (tzHoursOffset > 12 || tzHoursOffset < -12)
            {
                tzHoursOffset = 0;
            }
            // TimeChangeRule tzRule;
            // memcpy(tzRule.abbrev, "STD", 6);
            // tzRule.week = 0;
            // tzRule.dow = 1;
            // tzRule.month = 10;
            // tzRule.hour = 3;
            // tzRule.offset = tzHoursOffset*60;
            // cfgData.stdTimezone1UtcOffset = tzRule;
            cfgData.stdTimezone1UtcOffset = tzHoursOffset;
        }
        else if (v[cfgParamKey(TimeKeys::TIME_TIMEZONE_2_UTC_OFFSET)])
        {
            int tzHoursOffset = v[cfgParamKey(TimeKeys::TIME_TIMEZONE_2_UTC_OFFSET)].as<int>();
            if (tzHoursOffset > 12 || tzHoursOffset < -12)
            {
                tzHoursOffset = 0;
            }
            // TimeChangeRule tzRule;
            // memcpy(tzRule.abbrev, "DST", 6);
            // tzRule.week = 0;
            // tzRule.dow = 1;
            // tzRule.month = 3;
            // tzRule.hour = 2;
            // tzRule.offset = tzHoursOffset*60;
            // cfgData.dstTimezone2UtcOffset = tzRule;
            cfgData.dstTimezone2UtcOffset = tzHoursOffset;
        }
        else if (v[cfgParamKey(TimeKeys::TIME_NTP_ENABLED)])
        {
            cfgData.ntpEnabled = v[cfgParamKey(TimeKeys::TIME_NTP_ENABLED)].as<uint32_t>();
        }
        else if (v[cfgParamKey(TimeKeys::TIME_NTP_TIME_OFFSET)])
        {
            cfgData.ntpTimeOffset = v[cfgParamKey(TimeKeys::TIME_NTP_TIME_OFFSET)].as<int32_t>();
        }
        else if (v[cfgParamKey(TimeKeys::TIME_NTP_SYNC_INTERVAL)])
        {
            cfgData.ntpUpdateInterval = v[cfgParamKey(TimeKeys::TIME_NTP_SYNC_INTERVAL)].as<uint32_t>();
        }
        else if (v[cfgParamKey(TimeKeys::TIME_NTP_SERVER_1)])
        {
            memcpy(cfgData.ntpPoolServerNames[0], v[cfgParamKey(TimeKeys::TIME_NTP_SERVER_1)].as<const char*>(), NTP_SERVER_NAME_MAXLEN);
        }
        else if (v[cfgParamKey(TimeKeys::TIME_NTP_SERVER_2)])
        {
            memcpy(cfgData.ntpPoolServerNames[1], v[cfgParamKey(TimeKeys::TIME_NTP_SERVER_2)].as<const char*>(), NTP_SERVER_NAME_MAXLEN);
        }
        else if (v[cfgParamKey(TimeKeys::TIME_NTP_SERVER_3)])
        {
            memcpy(cfgData.ntpPoolServerNames[2], v[cfgParamKey(TimeKeys::TIME_NTP_SERVER_3)].as<const char*>(), NTP_SERVER_NAME_MAXLEN);
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
String TimeConfigParam::packToJson(const TimeConfigData& data)
{
    StaticJsonDocument<1024> doc;
    String json;
    JsonArray arr = doc.createNestedArray(key());
    JsonObject obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_TIMEZONES_NUMBER)] = data.timezoneNum;
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_TIMEZONE_1_UTC_OFFSET)] = data.stdTimezone1UtcOffset;
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_TIMEZONE_2_UTC_OFFSET)] = data.dstTimezone2UtcOffset;
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_NTP_ENABLED)] = data.ntpEnabled;
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_NTP_TIME_OFFSET)] = data.ntpTimeOffset;
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_NTP_SYNC_INTERVAL)] = data.ntpUpdateInterval;
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_NTP_SERVER_1)] = data.ntpPoolServerNames[0];
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_NTP_SERVER_2)] = data.ntpPoolServerNames[1];
    obj = arr.createNestedObject();
    obj[cfgParamKey(TimeKeys::TIME_NTP_SERVER_3)] = data.ntpPoolServerNames[2];
    serializeJson(doc, json);
    return json;
}

//------------------------------------------------------------------------------
