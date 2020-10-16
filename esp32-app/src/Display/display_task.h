#pragma once
#include "App/task.h"
#include "display.h"

class DisplayTask : public Task
{

public:
    DisplayTask();

    bool addTimeMsg(const DateTime& dt);

private:
    virtual void run() override;

private:
    Display::DateTimePrintMode m_timeDispMode;
    QueueHandle_t m_timeQ;
};
