#pragma once

#include "Rtos/task.h"
#include "App/rtos_utils.h"
#include "int_env_data_sensor.h"



class IntEnvDataTask : public Task
{
public:

    explicit IntEnvDataTask(InternalEnvDataSettings& intEnvSettings);

private:
    virtual void run() override;
    bool pushIntEnvDataMsg(const InternalEnvData& data);

private:
    InternalEnvDataSettings& m_intEnvCfg;
    InternalEnvironmentDataSensor m_intEnvDataSensor;
};

