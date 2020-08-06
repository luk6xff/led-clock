
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
{
    // m_mx.setZone(ZONE_0, 0, ZONE_SIZE);
    m_mx.begin(MAX_CLK_ZONES);
    m_mx.setIntensity(0);
    m_mx.setFont(dig_6x8_fonts);
    m_mx.displayZoneText(DISPLAY_ZONE_0, (char*)m_disp_txt_buf, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
}

//------------------------------------------------------------------------------
void Display::update()
{
    m_mx.displayAnimate();
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
    return m_disp_txt_buf;
}

//------------------------------------------------------------------------------
MD_Parola* Display::getDispObject()
{
    return &m_mx;
}
