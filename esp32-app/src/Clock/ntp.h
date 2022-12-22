
/**
 * @brief NTP current time client helper
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-16
 */


#pragma once
#include "Rtos/logger.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "Config/TimeConfigParam.h"

class Ntp final
{

public:
    explicit Ntp(const TimeConfigData& config);
    bool updateTime();
    uint64_t getCurrentTime();
    String getCurrentTimeString() const;

private:
    const TimeConfigData& m_config;
    WiFiUDP m_ntpUDP;
    NTPClient m_ntpClient;
};
