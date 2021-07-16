#include "day_info_runner.h"
#include "app_context.h"


const DayInfoRunner::DayInfoSchedule DayInfoRunner::k_dayInfoScheduleTimeTable[] = {
    {  0, 0, 1 },
    { 12, 0, 1 },
};

//------------------------------------------------------------------------------
DayInfoRunner::DayInfoRunner(const DateTime& dt)
    : m_dinfo(dt)
{

}

//------------------------------------------------------------------------------
void DayInfoRunner::process()
{
    String dayInfoMsg;
    for (auto dayInfSched : k_dayInfoScheduleTimeTable)
    {
        if (m_dinfo.dt().hour() == dayInfSched.hour && \
            m_dinfo.dt().minute() == dayInfSched.minute && \
            m_dinfo.dt().second() == dayInfSched.seconds)
        {
            String msg = m_dinfo.createMsg();
            AppCtx.putDisplayMsg(msg.c_str(), msg.length(), 1);
            break;
        }
    }
}

//------------------------------------------------------------------------------
