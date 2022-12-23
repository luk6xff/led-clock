#pragma once

#include "ConfigTypes.h"
#include "ConfigParam.h"
#include "ConfigDataTypes.h"
#include <Timezone.h>


class TimeConfigParam : public ConfigParam<TimeConfigData>
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
    bool setConfigFromJson(const JsonObject& json) override;
    String toStr() override;

private:
    void setCfgParamsMap();
    bool unpackFromJson(TimeConfigData& cfgData, const JsonObject& json) override;
    String packToJson(const TimeConfigData& data) override;
};