
/**
 * @brief NTP current time client helper
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-16
 */


#pragma once
#include "App/utils.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "system_time_settings.h"

class Ntp final
{

public:
    Ntp();
    explicit Ntp(const NtpSettings& config);
    bool updateTime();
    uint64_t getCurrentTime();
    String getCurrentTimeString() const;

private:
    const NtpSettings& m_config;
    WiFiUDP m_ntpUDP;
    NTPClient m_ntpClient;
};
