#pragma once
#include "App/task.h"
#include "Display/display_task.h"
#include "system_time.h"

class ClockTask : public Task
{
public:
    explicit ClockTask(DisplayTask& disp,
                       SystemTimeSettings& timeCfg);

    const QueueHandle_t& extSrcTimeQueue();

private:
    virtual void run() override;

private:
    DisplayTask& m_disp;
    SystemTimeSettings& m_timeCfg;
    QueueHandle_t m_extSrcTime;
};