#pragma once
#include "Rtos/task.h"
#include "Config/DisplayConfigParam.h"
#include "display.h"


typedef struct
{
    TaskHandle_t  timerTask;
    Display       *dispHandle;
    uint32_t      timeoutMs;
} DisplayTimerTask;

class DisplayTask : public Task
{

public:
    explicit DisplayTask(const DisplayConfigData& displayCfg,
                        const QueueHandle_t& timeQ);
    DisplayTask(const DisplayTask& disp) = delete;
    DisplayTask& operator=(const DisplayTask& disp) = delete;

    static void runLightSensorCb(void* arg);

protected:
    virtual void run() override;

private:
    Display::MAX72xxConfig m_dispCfg;
    Display::DateTimePrintMode m_timeDispMode;
    const DisplayConfigData& m_displayCfg;
    const QueueHandle_t& m_timeQ;
    Display m_disp;
    DisplayTimerTask m_lightSensorTimerTask;
};
