#pragma once
#include "App/task.h"
#include "Display/display_task.h"
#include "system_time.h"
#include "ntp.h"

class ClockTask : public Task
{
public:
    explicit ClockTask(DisplayTask& disp,
                       SystemTimeSettings& timeCfg,
                       NtpSettings& ntpCfg);

private:
    virtual void run() override;

private:
    DisplayTask& m_disp;
    SystemTimeSettings& m_timeCfg;
    NtpSettings& m_ntpCfg;
};