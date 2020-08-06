
/**
 * @brief Class describing a MAX72xx driven LED matrix
 *
 * @author Lukasz Uszko - luk6xff
 * @date   04.08.2020
 */


#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include "light_sensor.h"


#define DISPLAY_MAX72XX_HW_TYPE     MD_MAX72XX::DR1CR0RR0_HW//FC16_HW
#define DISPLAY_MAX72XX_MODULES_NUM 4
#define DISPLAY_MSG_MAX_LEN         1024
#define DISPLAY_ZONE_0  0
#define DISPLAY_ZONE_1  1

class Display
{

public:
    struct MAX72xxConfig
    {
        MD_MAX72XX::moduleType_t moduleType;
        uint8_t modulesNumber;
        gpio_num_t max72xxDINpin;
        gpio_num_t max72xxCLKpin;
        gpio_num_t max72xxCSpin;
    };


public:
    explicit Display(const MAX72xxConfig& cfg);

    void update();

    void reset();

    uint8_t* getDispTxtBuffer();

    MD_Parola* getDispObject();

    void enableAutoIntensityLevelControl(bool enable);

    void processAutoIntensityLevelControl();

    void setIntensityLevel(uint8_t level);

private:

    /**
     * @brief MAX_72xx object
     */
    MD_Parola m_mx;

    /**
     * @brief Light sensor object
     */
    LightSensor m_lightSensor;

    /**
     * @brief Display data buffer
     */
    uint8_t m_dispTxtBuf[DISPLAY_MSG_MAX_LEN];

    /**
     * @brief Sensor controlled display intensity level
     */
    bool m_autoIntensityLevelStatus;
};