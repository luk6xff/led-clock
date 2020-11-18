#pragma once

#include <string>
#include <vector>
#include <array>
#include <utility>

#define tr translate


//------------------------------------------------------------------------------
typedef enum
{
    // App language symbol
    D_LANG = 0,
    // Days of week
    D_DOW_MON,
    D_DOW_TUE,
    D_DOW_WED,
    D_DOW_THU,
    D_DOW_FRI,
    D_DOW_SAT,
    D_DOW_SUN,
    // Months
    D_MON_JAN,
    D_MON_FEB,
    D_MON_MAR,
    D_MON_APR,
    D_MON_MAY,
    D_MON_JUN,
    D_MON_JUL,
    D_MON_AUG,
    D_MON_SEP,
    D_MON_OCT,
    D_MON_NOV,
    D_MON_DEC,
    // Weather
    D_W_NAME,
    D_W_TEMP,
    D_W_IN,
    D_W_OUT,
    D_W_TODAY,
    D_W_TOMM,
    D_W_NOW,
} i18n_msg_id;

//------------------------------------------------------------------------------
typedef enum
{
    I18N_ENGLISH,
    I18N_POLISH,
    I18N_LANGUAGES_NUM
} i18n_lang;

//------------------------------------------------------------------------------
using i18n_map = std::vector<std::array<std::string, I18N_LANGUAGES_NUM>>;

class I18N final
{

public:

    explicit I18N(i18n_lang language);

    /**
     * @brief get text form map
     */
    std::string translate(i18n_msg_id msg_id);

    /**
     * @brief Load language data for language file in a map
     */
    void loadTranslation(i18n_lang language);

private:

    i18n_lang m_lang;
    static const i18n_map k_i18n;

};

