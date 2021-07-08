#include "system_time.h"
#include "hw_config.h"
#include "App/rtos_utils.h"
#include "App/utils.h"
#include "App/app_context.h"

//------------------------------------------------------------------------------
SystemTime::SystemTime(SystemTimeSettings& timeSettings)
    : m_timeSettings(timeSettings)
    , m_timezone(m_timeSettings.stdStart, m_timeSettings.dstStart)
{
    if (m_timeSettings.timezoneNum == 1)
    {
        m_timezone.setRules(m_timeSettings.stdStart, m_timeSettings.stdStart);
    }

    // Enable VDD Pin on RTC
    pinMode(DS3231_VDD_PIN, OUTPUT);
    digitalWrite(DS3231_VDD_PIN, HIGH);

    rtos::LockGuard<rtos::Mutex> lock(g_i2c0Mutex);
    if (!m_rtc.begin())
    {
        utils::err("Couldn't find RTC");
    }

    if (m_rtc.lostPower())
    {
        utils::err("RTC lost power, let set the default time!");
        m_rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

//------------------------------------------------------------------------------
void SystemTime::setTime(const DateTime& dt)
{
    rtos::LockGuard<rtos::Mutex> lock(g_i2c0Mutex);
    m_rtc.adjust(dt);
}

//------------------------------------------------------------------------------
void SystemTime::setUtcTime(const DateTime& dt)
{
    time_t utc = dt.unixtime();
    time_t local = m_timezone.toLocal(utc);
    DateTime localDt(local);
    setTime(localDt);
}

//------------------------------------------------------------------------------
const DateTime SystemTime::getTime()
{
    rtos::LockGuard<rtos::Mutex> lock(g_i2c0Mutex);
    return m_rtc.now();
}

//------------------------------------------------------------------------------
float SystemTime::getTemperature()
{
    rtos::LockGuard<rtos::Mutex> lock(g_i2c0Mutex);
    return m_rtc.getTemperature();
}

// //------------------------------------------------------------------------------
// bool SystemTime::checkTimezoneChange(const DateTime& dt)
// {
//     const time_t time = dt.unixtime();
//     if (!m_timezone.locIsDST(time) && !stdTimeChangeOccured)
//     {
//         stdTimeChangeOccured = true;
//     }
// }

//------------------------------------------------------------------------------
char* SystemTime::timeToStr(const DateTime& dt)
{
    static char str[] = "xxxxxxxx";
    if (dt.hour() < 10)
    {
      str[0] = '0';
    }
    else
    {
      str[0] = char((dt.hour() / 10) + '0');
    }

    str[1] = char((dt.hour() % 10) + '0');
    str[2] = 58;

    if (dt.minute() < 10)
    {
      str[3] = '0';
    }
    else
    {
      str[3]=char((dt.minute()/ 10) + '0');
    }

    str[4]=char((dt.minute() % 10) + '0');
    str[5]=58;

    if (dt.second()<10)
    {
        str[6] = '0';
    }
    else
    {
        str[6]=char((dt.second() / 10) + '0');
    }
    str[7]=char((dt.second() % 10) + '0');
    str[8]='\0';

    return (char*)&str;
}

//------------------------------------------------------------------------------
char *SystemTime::dateToStr(const DateTime& dt)
{
    static char str[11];
    const char divider = '-';

    memset(str, 0, sizeof(str));

    if (dt.day() < 10)
    {
        str[0] = '0';
    }
    else
    {
        str[0] = char((dt.day() / 10) + '0');
    }
    str[1]=char((dt.day() % 10) + '0');
    str[2] = divider;

    if (dt.month() < 10)
    {
        str[3] = '0';
    }
    else
    {
        str[3] = char((dt.month()/ 10) + '0');
    }
    str[4] = char((dt.month() % 10) + '0');
    str[5] = divider;

    uint16_t yr = dt.year();
    str[6] = char((yr / 1000) + '0');
    str[7] = char(((yr % 1000) / 100) + '0');
    str[8] = char(((yr % 100) / 10) + '0');
    str[9] = char((yr % 10) + '0');
    str[10]='\0';
	return (char*)&str;
}

//------------------------------------------------------------------------------
char *SystemTime::timeDateToStr(const DateTime& dt)
{
    static char strTimeDate[30] = {' '};
    const int timeStrLen = 8;
    const int dateStrLen = 10;
    memcpy(strTimeDate, timeToStr(dt), timeStrLen);
    memcpy(&strTimeDate[timeStrLen+2], dateToStr(dt), dateStrLen);
    // Remove nulls
    for (int i = 0; i < 30; ++i)
    {
        if ( i < 20 && strTimeDate[i] == '\0')
        {
            strTimeDate[i] = ' ';
        }
    }
    return (char*)&strTimeDate;
}

//------------------------------------------------------------------------------
const char* SystemTime::weekdayToStr(const DateTime& dt)
{
    // TODO - i18n
    static const char *dow[7] = { tr(M_DOW_SUN), tr(M_DOW_MON), tr(M_DOW_TUE), tr(M_DOW_WED), tr(M_DOW_THU), tr(M_DOW_FRI), tr(M_DOW_SAT) };
	return dow[dt.dayOfTheWeek()];
}

//------------------------------------------------------------------------------
const char *SystemTime::monthToStr(const DateTime& dt)
{
	static const char *months[12]  = { tr(M_MONTH_JAN), tr(M_MONTH_FEB), tr(M_MONTH_MAR), tr(M_MONTH_APR), tr(M_MONTH_MAY), tr(M_MONTH_JUN),
                                       tr(M_MONTH_JUL), tr(M_MONTH_AUG), tr(M_MONTH_SEP), tr(M_MONTH_OCT), tr(M_MONTH_NOV), tr(M_MONTH_DEC) };
	return months[dt.month()-1];
}

//------------------------------------------------------------------------------
uint8_t SystemTime::calculateWeekday(const DateTime& dt)
{
    int dow;
    uint8_t array[12] = {6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    dow = (dt.year() % 100);
    dow = dow * 1.25;
    dow += dt.day();
    dow += array[dt.month()-1];
    if (((dt.year() % 4) ==0) && (dt.month() < 3))
    {
        dow -= 1;
    }
    while (dow>7)
    {
        dow -= 7;
    }

    return dow;
}

//------------------------------------------------------------------------------
uint8_t SystemTime::validateDate(const DateTime& dt)
{
    uint8_t array[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
    uint8_t od;

    if (dt.month() == 2)
    {
        if ((dt.year() % 4)==0)
        {
            if (dt.day()==30)
            {
                od = 1;
            }
            else if (dt.day()==0)
            {
                od = 29;
            }
            else
            {
                od = dt.day();
            }
        }
        else
        {
            if (dt.day()==29)
            {
                od = 1;
            }
            else if (dt.day()==0)
            {
                od = 28;
            }
            else
            {
                od=dt.day();
            }
        }
    }
    else
    {
        if (dt.day() == 0)
        {
            od=array[dt.month()-1];
        }

        else if (dt.day()==(array[dt.month()-1]+1))
        {
        od = 1;
        }
        else
        {
            od=dt.day();
        }
    }

    return od;
}
