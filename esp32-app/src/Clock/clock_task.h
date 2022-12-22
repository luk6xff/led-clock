#pragma once
#include "Rtos/task.h"
#include "system_time.h"

class ClockTask : public Task
{
public:
    explicit ClockTask(TimeConfigData& timeCfg,
                        const QueueHandle_t& ntpTimeQ);

    const QueueHandle_t& getTimeQ();

private:
    virtual void run() override;
    bool pushTimeMsg(const DateTime& dt);

private:
    TimeConfigData& m_timeCfg;
    const QueueHandle_t& m_ntpTimeQ;
    const QueueHandle_t& m_extTimeQ;
    QueueHandle_t m_timeQ;
};