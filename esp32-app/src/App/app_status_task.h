#pragma once

#include "Rtos/task.h"
#include "App/app_status.h"
#include <map>
#include <functional>

using AppStateAction = std::function<void(void)>;
using AppStateActionsMap = std::map<AppStatusType, AppStateAction>;

class AppStatusTask : public Task
{
public:
    explicit AppStatusTask();

private:
    virtual void run() override;

private:
    static const AppStateActionsMap k_stateActions;
};