#pragma once
#include "ntp.h"
#include "Rtos/task.h"

class NtpTask : public Task
{
public:
    explicit NtpTask(TimeConfigData& ntpCfg, const EventGroupHandle_t& wifiEvtHandle);

    const QueueHandle_t& getNtpTimeQ();

private:
    virtual void run() override;

private:
    TimeConfigData& m_ntpCfg;
    const EventGroupHandle_t& m_wifiEvtHandle;
    QueueHandle_t m_ntpTimeQ;
};