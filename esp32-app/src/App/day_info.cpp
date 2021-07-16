#include "day_info.h"
#include "app_context.h"


//------------------------------------------------------------------------------
DayInfo::DayInfo(const DateTime& dt)
    : m_dt(dt)
{

}

//------------------------------------------------------------------------------
String DayInfo::createMsg()
{
    const String col(":");
    const String spc(" ");
    const String com(",");
    String msg(tr(M_TODAY_IS) + spc + \
                SystemTime::dateToStr(m_dt) + com + spc + \
                SystemTime::weekdayToStr(m_dt) + com + spc + \
                String(m_dt.dayOfTheYear()) + spc + tr(M_DAY) + spc + tr(M_OF_THE_YEAR));

    // Check if new year
    if (m_dt.month() == 1 && m_dt.day() == 1)
    {
        msg += spc + spc + tr(M_HAPPY_NEW_YEAR);
    }

    return msg;
}

//------------------------------------------------------------------------------
const DateTime& DayInfo::dt()
{
    return m_dt;
}

//------------------------------------------------------------------------------
