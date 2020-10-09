
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
#include "../Clock/system_rtc.h"


#define DISPLAY_MAX72XX_HW_TYPE     MD_MAX72XX::DR1CR0RR0_HW//FC16_HW
#define DISPLAY_MAX72XX_MODULES_NUM 4
#define DISPLAY_MSG_MAX_LEN         1024

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

    typedef enum
    {
        MH, // HH:MM
        MHS,// HH:MMss
    } TimePrintMode;


public:
    explicit Display(const MAX72xxConfig& cfg);

    void update();

    void reset();

    char *getDispTxtBuffer();

    MD_Parola *getDispObject();

    void enableAutoIntensityLevelControl(bool enable);

    void processAutoIntensityLevelControl();

    void setIntensityLevel(uint8_t level);

    void printTime(const DateTime& dt, TimePrintMode tpm, bool flasher);

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
     * @brief Display data buffer, ZONE_FULL
     */
    char m_dispFullBuf[DISPLAY_MSG_MAX_LEN];

    /**
     * @brief Display data buffer - ZONE_0
     */
    char m_dispZone0Buf[8];

    /**
     * @brief Display data buffer - ZONE_1
     */
    char m_dispZone1Buf[8];

    /**
     * @brief Sensor controlled display intensity level
     */
    bool m_autoIntensityLevelStatus;
};