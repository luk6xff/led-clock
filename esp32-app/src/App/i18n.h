#pragma once

#include <string>
#include <vector>
#include <array>
#include <utility>




//------------------------------------------------------------------------------
typedef enum
{
    // Common
    M_CM_EMPTY = 0,
    M_CM_DEG_CELS,
    M_CM_DEG_FAHR,
    M_CM_SPACE,
    M_CM_LAST = M_CM_SPACE,
    // App language symbol
    M_LANG,
    // Days of week
    M_DOW_MON,
    M_DOW_TUE,
    M_DOW_WED,
    M_DOW_THU,
    M_DOW_FRI,
    M_DOW_SAT,
    M_DOW_SUN,
    // Months
    M_MON_JAN,
    M_MON_FEB,
    M_MON_MAR,
    M_MON_APR,
    M_MON_MAY,
    M_MON_JUN,
    M_MON_JUL,
    M_MON_AUG,
    M_MON_SEP,
    M_MON_OCT,
    M_MON_NOV,
    M_MON_DEC,
    // Weather
    M_W_NAME,
    M_W_TEMP,
    M_W_TEMP_SHORT,
    M_W_IN,
    M_W_OUT,
    M_W_TODAY,
    M_W_TOMM,
    M_W_NOW,
} i18n_msg_id;

//------------------------------------------------------------------------------
typedef enum
{
    I18N_ENGLISH,
    I18N_POLISH,
    I18N_LANGUAGES_NUM
} i18n_lang;

//------------------------------------------------------------------------------
using i18n_map = std::vector<std::array<const std::string, I18N_LANGUAGES_NUM>>;

class I18N final
{

public:

    explicit I18N(i18n_lang language);

    /**
     * @brief get text form map
     */
    const std::string& translate(i18n_msg_id msg_id);

    /**
     * @brief Load language data for language file in a map
     */
    void loadTranslation(i18n_lang language);

private:

    i18n_lang m_lang;
    static const i18n_map k_i18n;

};

