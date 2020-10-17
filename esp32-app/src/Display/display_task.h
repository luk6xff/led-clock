#pragma once
#include "App/task.h"
#include "display.h"

class DisplayTask : public Task
{

public:
    DisplayTask();
    DisplayTask(const DisplayTask& disp) = delete;
    DisplayTask& operator=(const DisplayTask& disp) = delete;

    bool addTimeMsg(const DateTime& dt);

protected:
    virtual void run() override;

private:
    Display::MAX72xxConfig m_dispCfg;
    Display m_disp;
    Display::DateTimePrintMode m_timeDispMode;
    QueueHandle_t m_timeQ;
};
