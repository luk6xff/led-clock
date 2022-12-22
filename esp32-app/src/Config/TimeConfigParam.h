#pragma once

#include "config_param.h"
#include "config_types.h"
#include "config.h"
#include <Timezone.h>


#define NTP_SERVER_NAME_MAXLEN 32


struct TimeConfigData
{
    uint32_t timezoneNum;
    int32_t stdTimezone1UtcOffset;  // Standard time, difference in hours. eg. CET  - Central European Time (UTC+1)
    int32_t dstTimezone2UtcOffset;  // Summer time, difference in hours. eg. CEST - Central European daylight saving time (summer UTC+2)
    uint32_t ntpEnabled;
    int32_t ntpTimeOffset;          // in [min]
    uint32_t ntpUpdateInterval;     // in [ms]
    char ntpPoolServerNames[3][NTP_SERVER_NAME_MAXLEN];
};


class TimeConfigParam : public ConfigParam<TimeConfigData, Config>
{
public:

    enum TimeKeys : ParamsKey
    {
        TIME_TIMEZONES_NUMBER,
        TIME_TIMEZONE_1_UTC_OFFSET,
        TIME_TIMEZONE_2_UTC_OFFSET,
        TIME_NTP_ENABLED,
        TIME_NTP_TIME_OFFSET,
        TIME_NTP_SYNC_INTERVAL,
        TIME_NTP_SERVER_1,
        TIME_NTP_SERVER_2,
        TIME_NTP_SERVER_3,
    };


    TimeConfigParam();
    bool setConfig(const JsonObject& json) override;
    void getConfig(String& configPayload) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(TimeConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const TimeConfigData& data) override;
};