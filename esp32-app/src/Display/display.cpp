
#include "display.h"
#include "fonts.h"



//#define MAX_DEVICES 4
#define MAX_CLK_ZONES 1
#define ZONE_SIZE 4
#define MAX_DEVICES (MAX_CLK_ZONES * ZONE_SIZE)
#define SPEED_TIME  1
#define PAUSE_TIME  0

//------------------------------------------------------------------------------
// Global variables

//------------------------------------------------------------------------------
Display::Display(const MAX72xxConfig& cfg)
    : m_mx(cfg.moduleType, cfg.max72xxCSpin, cfg.modulesNumber)
    , m_autoIntensityLevelStatus(true)
{
    // m_mx.setZone(ZONE_0, 0, ZONE_SIZE);
    m_mx.begin(MAX_CLK_ZONES);
    m_mx.setIntensity(0);
    m_mx.setFont(dig_6x8_fonts);
    m_mx.displayZoneText(DISPLAY_ZONE_0, (char*)m_dispTxtBuf, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
}

//------------------------------------------------------------------------------
void Display::update()
{
    m_mx.displayAnimate();
    processAutoIntensityLevelControl();
}

//------------------------------------------------------------------------------
void Display::reset()
{
    m_mx.displayReset();
    // Synchronise the zone start
    m_mx.synchZoneStart();
}

//------------------------------------------------------------------------------
uint8_t* Display::getDispTxtBuffer()
{
    return m_dispTxtBuf;
}

//------------------------------------------------------------------------------
MD_Parola* Display::getDispObject()
{
    return &m_mx;
}


//------------------------------------------------------------------------------
void Display::enableAutoIntensityLevelControl(bool enable)
{
    m_autoIntensityLevelStatus = enable;
}

//------------------------------------------------------------------------------
void Display::processAutoIntensityLevelControl()
{
    if (!m_autoIntensityLevelStatus)
    {
        return;
    }

    // Measure light
    const float light_val = m_lightSensor.getIlluminace();
    Serial.printf("LIGHT_VALUE: %3.2f\r\n", light_val);
    // Last display intensty level
    static uint8_t last_intensity_level = 0;

    // Logarithmic level of sensor illuminance in lux [lx]
    const uint8_t level[] = {0,1,2,4,6,8,9,10,11,12}; // 0 = low, 15 = high
    const uint16_t max_light_illuminance = 1000;

    // Calculate linear level
    int l = (light_val  * sizeof(level)) / max_light_illuminance;

    // Get logarithmic level
    l = level[l];

    if (last_intensity_level != l)
    {
        last_intensity_level = l;
        setIntensityLevel(last_intensity_level);
    }
}

//------------------------------------------------------------------------------
void Display::setIntensityLevel(uint8_t level)
{
    if (level > MAX_INTENSITY)
    {
        level = MAX_INTENSITY;
    }
    m_mx.setIntensity(level);
}

//------------------------------------------------------------------------------
