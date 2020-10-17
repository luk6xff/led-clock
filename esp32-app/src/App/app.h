
/**
 * @brief Class describing a complete application
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-15
 */

#include "Display/display_task.h"
#include "Clock/clock_task.h"

#include <memory>

class App
{

public:
    static App& instance();

    void setup();

    void run();

private:
    App();

    std::unique_ptr<DisplayTask> m_dispTask;
    std::unique_ptr<ClockTask> m_clockTask;
};