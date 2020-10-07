#ifndef __SYSTEM_RTC_H__
#define __SYSTEM_RTC_H__

#include <RTClib.h>


class SystemRtc final
{

public:
    explicit SystemRtc();
    bool init();
    void setTime(const DateTime& dt);
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
    RTC_DS3231 rtc;
};


#endif // __SYSTEM_RTC_H__