#pragma once
#include "ntp.h"
#include "App/task.h"

class NtpTask : public Task
{
public:
    explicit NtpTask(NtpSettings& ntpCfg, const EventGroupHandle_t& wifiEvtHandle);

private:
    virtual void run() override;

private:
    NtpSettings& m_ntpCfg;
    const EventGroupHandle_t& m_wifiEvtHandle;
};