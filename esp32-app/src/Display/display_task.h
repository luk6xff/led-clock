#pragma once
#include "App/task.h"
#include "display_settings.h"
#include "display.h"

class DisplayTask : public Task
{

public:
    explicit DisplayTask(const DisplaySettings& displayCfg,
                        const QueueHandle_t& timeQ);
    DisplayTask(const DisplayTask& disp) = delete;
    DisplayTask& operator=(const DisplayTask& disp) = delete;

protected:
    virtual void run() override;

private:
    Display::MAX72xxConfig m_dispCfg;
    Display::DateTimePrintMode m_timeDispMode;
    const DisplaySettings& m_displayCfg;
    const QueueHandle_t& m_timeQ;
};
