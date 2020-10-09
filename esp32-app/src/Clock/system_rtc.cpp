#include "system_rtc.h"
#include "hw_config.h"
#include "../App/utils.h"

//------------------------------------------------------------------------------
SystemRtc::SystemRtc()
{
    if (!rtc.begin())
    {
        err("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower())
    {
        err("RTC lost power, lets set the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
}

//------------------------------------------------------------------------------
void SystemRtc::setTime(const DateTime& dt)
{
    rtc.adjust(dt);
}

//------------------------------------------------------------------------------
const DateTime SystemRtc::getTime()
{
    return rtc.now();
}

//------------------------------------------------------------------------------
float SystemRtc::getTemperature()
{
    return rtc.getTemperature();
}

//------------------------------------------------------------------------------
char* SystemRtc::timeToStr(const DateTime& dt)
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
char *SystemRtc::dateToStr(const DateTime& dt)
{
    static char str[] = "xxxxxxxxxx";
    const char divider = '-';
    const uint8_t offset = 2;

    uint16_t yr = dt.year();
    str[0] = char((yr / 1000) + '0');
    str[1] = char(((yr % 1000) / 100) + '0');
    str[2] = char(((yr % 100) / 10) + '0');
    str[3] = char((yr % 10) + '0');
    str[4] = divider;
    if (dt.month() < 10)
    {
        str[3+offset] = '0';
    }
    else
    {
        str[3+offset] = char((dt.month()/ 10) + '0');
    }
    str[4+offset] = char((dt.month() % 10) + '0');
    str[5+offset] = divider;
    if (dt.day() < 10)
    {
        str[6+offset] = '0';
    }
    else
    {
        str[6+offset] = char((dt.day() / 10) + '0');
    }
    str[7+offset]=char((dt.day() % 10) + '0');
    str[8+offset]='\0';
	return (char*)&str;
}

//------------------------------------------------------------------------------
char *SystemRtc::timeDateToStr(const DateTime& dt)
{
    static char strTimeDate[30] = {' '};
    const int timeStrLen = 8;
    const int dateStrLen = 10;
    memcpy(strTimeDate, timeToStr(dt), timeStrLen);
    memcpy(&strTimeDate[timeStrLen+2], dateToStr(dt), dateStrLen);
    // remove nulls
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
const char* SystemRtc::weekdayToStr(const DateTime& dt)
{
    // TODO - i18n
    static char weekday[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    static char weekdayPL[7][14] = {"Niedziela", "Poniedziałek", "Wtorek", "Środa", "Czwartek", "Piątek", "Sobota"};
	return weekdayPL[dt.dayOfTheWeek()];
}

//------------------------------------------------------------------------------
const char *SystemRtc::monthToStr(const DateTime& dt)
{
	static const char *months[]  = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	return months[dt.month() - 1];
}

//------------------------------------------------------------------------------
uint8_t SystemRtc::calculateWeekday(const DateTime& dt)
{
  int dow;
  uint8_t array[12] = {6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  dow = (dt.year() % 100);
  dow = dow*1.25;
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
uint8_t SystemRtc::validateDate(const DateTime& dt)
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
