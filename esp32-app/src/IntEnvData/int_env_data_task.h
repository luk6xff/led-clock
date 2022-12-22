#pragma once

#include "Rtos/task.h"
#include "Rtos/RtosUtils.h"
#include "int_env_data_sensor.h"



class IntEnvDataTask : public Task
{
public:

    explicit IntEnvDataTask(InternalEnvironmentDataConfigData& intEnvSettings);

private:
    virtual void run() override;
    bool pushIntEnvDataMsg(const InternalEnvData& data);

private:
    InternalEnvironmentDataConfigData& m_intEnvCfg;
    InternalEnvironmentDataSensor m_intEnvDataSensor;
};

