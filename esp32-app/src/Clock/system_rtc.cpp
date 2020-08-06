#include "system_rtc.h"
#include "hw_config.h"


//------------------------------------------------------------------------------
SystemRtc& SystemRtc::instance()
{
    static SystemRtc rtc;
    return rtc;
}

//------------------------------------------------------------------------------
SystemRtc::SystemRtc()
{
    arduino_dev.i2c = &Wire; // (DS3231_SDA, DS3231_SCL)
    dev.i2c_addr = DS3231_I2C_ADDR;
    ds3231_arduino_init(&dev, &arduino_dev);
}

//------------------------------------------------------------------------------
int SystemRtc::set_time(const SystemTime& time)
{

    ds3231_time_t _time;
    _time.seconds = time.seconds;
    _time.minutes = time.minutes;
    _time.hours   = time.hours;
    _time.mode    = 0;
    _time.am_pm   = 0; // 24H

    ds3231_calendar_t _cal;
    _cal.day   = time.day;
    _cal.date  = time.date;
    _cal.month = time.month;
    _cal.year  = time.year;

    if (ds3231_set_time(&dev, _time) != 0)
    {
        return 1; // error
    }
        if (ds3231_set_calendar(&dev, _cal) != 0)
    {
        return 2; // error
    }
    return 0;
}

//------------------------------------------------------------------------------
int SystemRtc::get_time(SystemTime& time)
{
    ds3231_time_t _time;
    ds3231_calendar_t _cal;

    if (ds3231_get_time(&dev, &_time) != 0)
    {
        return 1; // error
    }
    if (ds3231_get_calendar(&dev, &_cal) != 0)
    {
        return 2; // error
    }
    time.seconds   = _time.seconds;
    time.minutes   = _time.minutes;
    time.hours     = _time.hours;
    time.day       = _cal.day;
    time.date      = _cal.date;
    time.month     = _cal.month;
    time.year      = _cal.year;
    return 0;
}

//------------------------------------------------------------------------------
float SystemRtc::get_temperature()
{
    uint16_t temp = ds3231_get_temperature(&dev);
    int sign = ((temp & 0x8000) == 0) ? 1 : -1;
    uint8_t msb = ((temp & 0x7F00) >> 8) & 0xFF;
    uint8_t lsb = ((temp & 0x00C0) >> 6) & 0xF;
    float result = (float)msb + ((lsb * 0.25) * sign);
    return result;
}

//------------------------------------------------------------------------------
char* SystemRtc::time_string(const SystemTime& time)
{
  static char str[] = "xxxxxxxx";
  if (time.hours < 10)
  {
    str[0] = '0';
  }
  else
  {
    str[0] = char((time.hours / 10) + '0');
  }

  str[1] = char((time.hours % 10) + '0');
  str[2] = 58;

  if (time.minutes < 10)
  {
    str[3] = '0';
  }
  else
  {
    str[3]=char((time.minutes/ 10) + '0');
  }

  str[4]=char((time.minutes % 10) + '0');
  str[5]=58;

  if (time.seconds<10)
  {
      str[6] = '0';
  }
  else
  {
      str[6]=char((time.seconds / 10) + '0');
  }
  str[7]=char((time.seconds % 10) + '0');
  str[8]='\0';

  return (char*)&str;
}

//------------------------------------------------------------------------------
char *SystemRtc::date_string(const SystemTime& time)
{
	static char str[] = "xxxxxxxxxx";
    const char divider = '-';
    const uint8_t offset = 2;

    uint16_t yr = time.year;
    str[0]=char((yr / 1000) + '0');
    str[1]=char(((yr % 1000) / 100) + '0');
    str[2]=char(((yr % 100) / 10) + '0');
    str[3]=char((yr % 10) + '0');
    str[4]=divider;
    if (time.month < 10)
    {
        str[3+offset] = '0';
    }
    else
    {
        str[3+offset] = char((time.month/ 10) + '0');
    }
    str[4+offset] = char((time.month % 10) + '0');
    str[5+offset] = divider;
    if (time.date < 10)
    {
        str[6+offset] = '0';
    }
    else
    {
        str[6+offset] = char((time.date / 10) + '0');
    }
    str[7+offset]=char((time.date % 10) + '0');
    str[8+offset]='\0';
	return (char*)&str;
}

//------------------------------------------------------------------------------
char *SystemRtc::time_date_string(const SystemTime& time)
{
    static char strTimeDate[30] = {' '};
    const int timeStrLen = 8;
    const int dateStrLen = 10;
    memcpy(strTimeDate, time_string(time), timeStrLen);
    memcpy(&strTimeDate[timeStrLen+2], date_string(time), dateStrLen);
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
const char* SystemRtc::get_day_of_week_string(const SystemTime& time)
{
	static const char *days[]  = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
	return days[time.day - 1];
}

//------------------------------------------------------------------------------
const char *SystemRtc::get_month_string(const SystemTime& time)
{
	static const char *months[]  = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	return months[time.month - 1];
}

//------------------------------------------------------------------------------
uint8_t SystemRtc::calculate_day_of_week(uint8_t d, uint8_t m, int y)
{
  int dow;
  uint8_t array[12] = {6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  dow = (y % 100);
  dow = dow*1.25;
  dow += d;
  dow += array[m-1];
  if (((y % 4)==0) && (m<3))
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
uint8_t SystemRtc::validate_date(uint8_t d, uint8_t m, uint16_t y)
{
  uint8_t array[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
  uint8_t od;

  if (m==2)
  {
    if ((y % 4)==0)
    {
      if (d==30)
        od=1;
      else if (d==0)
        od=29;
      else
        od=d;
    }
    else
    {
      if (d==29)
        od=1;
      else if (d==0)
        od=28;
      else
        od=d;
    }
  }
  else
  {
    if (d==0)
      od=array[m-1];
    else if (d==(array[m-1]+1))
      od=1;
    else
      od=d;
  }

  return od;
}
