#ifndef __SYSTEM_TIME_H__
#define __SYSTEM_TIME_H__

#include <RTClib.h>
#include <Timezone.h>


struct SystemTimeSettings
{
    TimeChangeRule dstStart; // CEST
    TimeChangeRule stdStart; // CET
    String toStr()
    {
        return String("dstStart:" + dstStart.toStr() + " stdStart:" + stdStart.toStr());
    }
};


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
    //bool checkTimezoneChange(const DateTime& dt);

private:
    RTC_DS3231 m_rtc;
    SystemTimeSettings &m_timeSettings;
    Timezone m_timezone;
};


#endif // __SYSTEM_TIME_H__