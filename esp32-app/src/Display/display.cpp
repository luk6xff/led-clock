
#include "display.h"
#include "fonts.h"
#include "../App/utils.h"


#define MAX_DISPLAY_ZONES 3
#define DISPLAY_ZONE_0    0
#define DISPLAY_ZONE_1    1
#define DISPLAY_ZONE_FULL 2

#define ZONE_SIZE   1
#define SPEED_TIME  50
#define PAUSE_TIME  0


//------------------------------------------------------------------------------
Display::Display(const MAX72xxConfig& cfg)
    : m_mx(cfg.moduleType, cfg.max72xxCSpin, cfg.modulesNumber)
    , m_autoIntensityLevelStatus(true)
{
    m_mx.begin(MAX_DISPLAY_ZONES);

    m_mx.setZone(DISPLAY_ZONE_0, 0, 0);
    m_mx.setZone(DISPLAY_ZONE_1, 1, 3);
    m_mx.setZone(DISPLAY_ZONE_FULL, 0, 3);
    m_mx.setIntensity(0);
    m_mx.setFont(DISPLAY_ZONE_FULL, dig_6x8_fonts);
    m_mx.setFont(DISPLAY_ZONE_0, dig_3x5_fonts);
    m_mx.setFont(DISPLAY_ZONE_1, dig_4x8_fonts);
    m_mx.displayZoneText(DISPLAY_ZONE_FULL, m_dispFullBuf, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
                m_mx.displayZoneText(DISPLAY_ZONE_0, m_dispZone0Buf, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
                m_mx.displayZoneText(DISPLAY_ZONE_1, m_dispZone1Buf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
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
char *Display::getDispTxtBuffer()
{
    return m_dispFullBuf;
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
            if (getDispObject()->getZoneStatus(DISPLAY_ZONE_FULL))
            {
                sprintf((char *)getDispTxtBuffer(), "%02d%c%02d", dt.hour(), (flasher ? ':' : ' '), dt.minute());
                //reset();
                m_mx.displayReset(DISPLAY_ZONE_FULL);
            }
            break;
        }

        case MHS:
        {

            if (getDispObject()->getZoneStatus(DISPLAY_ZONE_0) && getDispObject()->getZoneStatus(DISPLAY_ZONE_1))
            {
                snprintf(m_dispZone0Buf, sizeof(m_dispZone0Buf), "%02d", dt.second());
                snprintf(m_dispZone1Buf, sizeof(m_dispZone1Buf), "%02d%c%02d", dt.hour(), (flasher ? ':' : ' '), dt.minute());
                m_mx.displayReset(DISPLAY_ZONE_0);
                m_mx.displayReset(DISPLAY_ZONE_1);
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