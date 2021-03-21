#pragma once

#include "App/task.h"
#include "App/rtos_utils.h"
#include "int_env_data.h"



struct IntEnvDataComm
{
    QueueHandle_t intEnvQ;
    EventGroupHandle_t intEvtH;
};

class IntEnvDataTask : public Task
{
public:
    enum IntEnvDataEvent : EventBits_t
    {
        INTERNAL_ENV_DATA_REQ = 1<<0,
        INTERNAL_ENV_DATA_READY = 1<<1,
    };


    explicit IntEnvDataTask(InternalEnvDataSettings& intEnvSettings);
    const IntEnvDataComm& getComm();

private:
    virtual void run() override;
    bool pushIntEnvDataMsg(const InternalEnvData& data);

private:
    InternalEnvDataSettings& m_intEnvCfg;
    IntEnvDataComm m_intEnvDataComm;
};

