
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

#define NTP_SERVER_NAME_MAXLEN 30

struct NtpSettings
{
    NtpSettings()
        : timeOffset(0)
        , updateInterval(3600)
    {
        memset(poolServerNames[0], 0, NTP_SERVER_NAME_MAXLEN);
        memset(poolServerNames[1], 0, NTP_SERVER_NAME_MAXLEN);
        memset(poolServerNames[2], 0, NTP_SERVER_NAME_MAXLEN);
    }

    NtpSettings(int32_t tO, uint32_t uI, const char *server0Name,
                const char *server1Name=nullptr, const char *server2Name=nullptr)
        : timeOffset(tO)
        , updateInterval(uI)
    {
        memset(poolServerNames[0], 0, NTP_SERVER_NAME_MAXLEN);
        memset(poolServerNames[1], 0, NTP_SERVER_NAME_MAXLEN);
        memset(poolServerNames[2], 0, NTP_SERVER_NAME_MAXLEN);
        if (server0Name)
        {
            memcpy(poolServerNames[0], server0Name, NTP_SERVER_NAME_MAXLEN);
        }
        if (server1Name)
        {
            memcpy(poolServerNames[1], server1Name, NTP_SERVER_NAME_MAXLEN);
        }
        if (server2Name)
        {
            memcpy(poolServerNames[2], server2Name, NTP_SERVER_NAME_MAXLEN);
        }
    }

    NtpSettings(const NtpSettings& other)
        : timeOffset(other.timeOffset)
        , updateInterval(other.updateInterval)
    {
        memcpy(poolServerNames[0], other.poolServerNames[0], NTP_SERVER_NAME_MAXLEN);
        memcpy(poolServerNames[1], other.poolServerNames[1], NTP_SERVER_NAME_MAXLEN);
        memcpy(poolServerNames[2], other.poolServerNames[2], NTP_SERVER_NAME_MAXLEN);
    }

    NtpSettings& operator=(const NtpSettings& other)
    {
        timeOffset = other.timeOffset;
        updateInterval = other.updateInterval;
        memcpy(this->poolServerNames[0], other.poolServerNames[0], NTP_SERVER_NAME_MAXLEN);
        memcpy(this->poolServerNames[1], other.poolServerNames[1], NTP_SERVER_NAME_MAXLEN);
        memcpy(this->poolServerNames[2], other.poolServerNames[2], NTP_SERVER_NAME_MAXLEN);
        return *this;
    }

    String toStr()
    {
        return "tO:" + String(String(timeOffset) + " uI:" + String(updateInterval) + " pSN0:" + String(poolServerNames[0]) + \
                      " pSN1:" + String(poolServerNames[1]) + " pSN2:" + String(poolServerNames[2]));
    }

    int32_t timeOffset;
    uint32_t updateInterval;
    char poolServerNames[3][30];
};


class Ntp final
{

public:
    explicit Ntp(NtpSettings& config);
    bool updateTime();
    uint64_t getCurrentTime();
    String getCurrentTimeString() const;

private:
    WiFiUDP m_ntpUDP;
    NtpSettings& m_config;
    NTPClient m_ntpClient;

};
