#pragma once
#include "App/task.h"
#include "system_time.h"
// Internal environment data, read from RTC
#include "IntEnvData/int_env_data_task.h"

class ClockTask : public Task
{
public:
    explicit ClockTask(SystemTimeSettings& timeCfg,
                        const QueueHandle_t& ntpTimeQ,
                        const IntEnvDataComm& intEnvComm);

    const QueueHandle_t& getTimeQ();

private:
    virtual void run() override;
    bool pushTimeMsg(const DateTime& dt);

private:
    SystemTimeSettings& m_timeCfg;
    const QueueHandle_t& m_ntpTimeQ;
    const QueueHandle_t& m_extTimeQ;
    QueueHandle_t m_timeQ;

    // Internal environment data handle
    const IntEnvDataComm& m_intEnvDataComm;
};