
#include "display.h"
#include "fonts.h"
#include "../App/utils.h"

#define DISPLAY_ZONE_0  0
#define DISPLAY_ZONE_1  1

#define MAX_DISPLAY_ZONES 2
#define ZONE_SIZE   1
#define SPEED_TIME  1
#define PAUSE_TIME  0

//------------------------------------------------------------------------------
// Global variables

//------------------------------------------------------------------------------
Display::Display(const MAX72xxConfig& cfg)
    : m_mx(cfg.moduleType, cfg.max72xxCSpin, cfg.modulesNumber)
    , m_autoIntensityLevelStatus(true)
{
    m_mx.begin(MAX_DISPLAY_ZONES);
    m_mx.setZone(DISPLAY_ZONE_0, 0, 2);
    m_mx.setZone(DISPLAY_ZONE_1, 3, 4);
    m_mx.setIntensity(0);
    m_mx.setFont(dig_6x8_fonts);
    //m_mx.displayZoneText(DISPLAY_ZONE_0, (char*)m_dispTxtBuf, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
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
    const float lightVal = m_lightSensor.getIlluminance();
    if (lightVal == -1)
    {
        err("LightSensor - Illuminance value error!");
    }
    // Last display intensty level
    static uint8_t lastIntensityLevel = 0;

    // Logarithmic level of sensor illuminance in lux [lx]
    const uint8_t level[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}; // 0 = low, 15 = high
    const uint16_t maxLightIlluminance = 1000;

    // Calculate linear level
    int l = (lightVal * sizeof(level)) / maxLightIlluminance;

    // Get logarithmic level
    if (l >= sizeof(level))
    {
        l = sizeof(level) - 1;
    }
    l = level[l];

    if (lastIntensityLevel != l)
    {
        lastIntensityLevel = l;
        setIntensityLevel(lastIntensityLevel);
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
void Display::printTime(const DateTime& dt, TimePrintMode tpm, bool flasher)
{
    switch(tpm)
    {
        case MH:
        {
            m_mx.setFont(dig_6x8_fonts);
            if (getDispObject()->getZoneStatus(DISPLAY_ZONE_0))
            {
                sprintf((char *)getDispTxtBuffer(), "%02d%c%02d", dt.hour(), (flasher ? ':' : ' '), dt.minute());
                reset();
            }
            break;
        }

        case MHS:
        {
            if (getDispObject()->getZoneStatus(DISPLAY_ZONE_0) && getDispObject()->getZoneStatus(DISPLAY_ZONE_1))
            {
                //sprintf((char *)getDispTxtBuffer(), "%02d%c%02d%02d", dt.hour(), (flasher ? ':' : ' '), dt.minute(), dt.second());
                //reset();

                // m_mx.setFont(dig_4x8_fonts);
                // char hm[6];
                // sprintf(hm, "%02d%c%02d", dt.hour(), (flasher ? ':' : ' '), dt.minute());
                // m_mx.displayZoneText(DISPLAY_ZONE_0, hm, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_PRINT);

                m_mx.setFont(dig_3x5_fonts);
                char sec[2];
                sprintf(sec, "%02d", dt.second());
                m_mx.displayZoneText(DISPLAY_ZONE_1, sec, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_PRINT);
                reset();
            }
            break;
        }

        default:
        {
            err("Invalid Print time mode");
            break;
        }
    }

}