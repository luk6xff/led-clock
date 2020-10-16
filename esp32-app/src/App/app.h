
/**
 * @brief Class describing a complete application
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-09-01
 */

#include "Display/display_task.h"
#include "Clock/clock_task.h"

class App
{

public:
    static App& instance();

    void setup();

    void run();

private:
    App();

    DisplayTask m_dispTask;
    //ClockTask   m_clockTask;
};