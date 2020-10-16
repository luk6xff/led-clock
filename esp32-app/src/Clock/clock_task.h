#pragma once
#include "App/task.h"
#include "system_rtc.h"
#include "Display/display_task.h"

class ClockTask : public Task
{
public:
    explicit ClockTask(DisplayTask& disp);

private:
    virtual void run() override;

private:
    SystemRtc m_rtc;
    DisplayTask& m_disp;
};