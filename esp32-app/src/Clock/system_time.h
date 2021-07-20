#ifndef __SYSTEM_TIME_H__
#define __SYSTEM_TIME_H__

#include <RTClib.h>
#include "system_time_settings.h"



class SystemTime final
{

public:
    explicit SystemTime(SystemTimeSettings &timeSettings);
    bool init();
    void setTime(const DateTime& dt);
    void setUtcTime(const DateTime& dt);
    const DateTime getTime();
    float getTemperature();

    static char* timeToStr(const DateTime& dt);
    static char* dateToStr(const DateTime& dt);
    static char* timeDateToStr(const DateTime& dt);
    static const char* weekdayToStr(const DateTime& dt);
    static const char* monthToStr(const DateTime& dt);
    static uint8_t calculateWeekday(const DateTime& dt);
    static uint8_t validateDate(const DateTime& dt);

private:
    DateTime checkTimezoneChange(const DateTime& dt);

private:
    RTC_DS3231 m_rtc;
    SystemTimeSettings &m_timeSettings;
    Timezone m_timezone;
    TimezoneType m_lastTimezone;
};


#endif // __SYSTEM_TIME_H__