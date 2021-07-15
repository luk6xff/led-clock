#pragma once
#include "App/task.h"

class AppStatusTask : public Task
{
public:
    explicit AppStatusTask();

private:
    virtual void run() override;
};