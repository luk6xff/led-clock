#include "ntp.h"
#include "Rtos/log.h"

//------------------------------------------------------------------------------
Ntp::Ntp(const TimeConfigData& config)
    : m_config(config)
    , m_ntpClient(m_ntpUDP, m_config.poolServerNames[0],
                    m_config.timeOffset, m_config.updateInterval)
{

}

//------------------------------------------------------------------------------
bool Ntp::updateTime()
{
    return m_ntpClient.update();
}

//------------------------------------------------------------------------------
uint64_t Ntp::getCurrentTime()
{
    return m_ntpClient.getEpochTime();
}

//------------------------------------------------------------------------------
String Ntp::getCurrentTimeString() const
{
    return m_ntpClient.getFormattedTime();
}

//------------------------------------------------------------------------------
