
/**
 * @brief Class describing a complete application
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-15
 */
#include "esp_system.h"
#include "Wifi/wifi_task.h"
#include "Display/display_task.h"
#include "Clock/clock_task.h"
#include "Clock/ntp_task.h"
#include "Weather/weather_task.h"
#include "Radio_ExtEnvData/radio_sensor_task.h"
#include "IntEnvData/int_env_data_task.h"
#include <memory>


class App
{

public:
    static App& instance();
    void setup();
    void run();

private:
    App();
    ~App();
    void createTasks();
    void runTasks();
    void printMotd();

private:
    std::unique_ptr<WifiTask> m_wifiTask;
    std::unique_ptr<DisplayTask> m_dispTask;
    std::unique_ptr<ClockTask> m_clockTask;
    std::unique_ptr<NtpTask> m_ntpTask;
    std::unique_ptr<WeatherTask> m_weatherTask;
    std::unique_ptr<RadioSensorTask> m_radioSensorTask;
    std::unique_ptr<IntEnvDataTask> m_intEnvDataTask;
};