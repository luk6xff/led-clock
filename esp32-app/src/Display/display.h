
/**
 * @brief Class describing a MAX72xx driven LED matrix
 *
 * @author Lukasz Uszko - luk6xff
 * @date   04.08.2020
 */


#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>


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


private:

    /**
     * @brief MAX_72xx object
     */
    MD_Parola m_mx;

    uint8_t m_disp_txt_buf[DISPLAY_MSG_MAX_LEN];
};