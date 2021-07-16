
#pragma once
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
#include "../Clock/system_time.h"


#define DISPLAY_MAX72XX_HW_TYPE     MD_MAX72XX::DR1CR0RR0_HW//FC16_HW
#define DISPLAY_MAX72XX_MODULES_NUM 4
#define DISPLAY_MSG_MAX_LEN         1024

#define LIGHT_SENSOR_ENABLED 0

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
        THM, // HH:MM
        THMS,// HH:MMss
        DWYMD, // Weekday, year-month-day
    } DateTimePrintMode;


public:
    explicit Display(const MAX72xxConfig& cfg);

    void setup();

    void update();

    void reset();

    void clear();

    char *getDispTxtBuffer();

    MD_Parola *getDispObject();

    void enableAutoIntensityLevelControl(bool enable);

    bool isAutoIntensityEnabled();

    void processAutoIntensityLevelControl();

    void setIntensityLevel(uint8_t level);

    void setDisplaySpeedValue(uint32_t speed);

    void setDisplayPauseValue(uint32_t pause);

    bool printTime(const DateTime& dt, DateTimePrintMode tpm, bool timeDots=false);

    /**
     * @brief Print user message on the display
     *
     * @return Status if message has been displayed (true) or zone state busy (false)
     */
    bool printMsg(const char *msg, const size_t msgSize);

    /**
     * @brief In place conversion UTF-8 string to Extended ASCII (+ some polish chars from Latin Extended-A)
     *        The extended ASCII string is always shorter.
     */
    static String utf8Ascii(const char *s);

private:

    /**
     * @brief Returns Display zone status
     *
     * @return Status if display zones are ready for writing (true if so, false otherwise)
     */
    bool isDisplayWritable();

private:

    /**
     * @brief MAX_72xx object
     */
    MD_Parola m_mx;

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

    /**
     * @brief Display scroll speed value [in ms]
     */
    uint32_t m_dispSpeed;

    /**
     * @brief Display scroll pause value [in ms]
     */
    uint32_t m_dispPause;

    /**
     * @brief Light sensor object
     */
    LightSensor m_lightSensor;
};