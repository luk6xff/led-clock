#include "i18n.h"
#include "utils.h"


//------------------------------------------------------------------------------
const i18n_map I18N::k_i18n =
{
    // Common
    { "" },
    { "°C" },
    { "°F" },
    { " " },
    // App language symbol
    { "en", "pl"},
    // Days of week
    { "Monday", "Poniedziałek" },
    { "Tuesday", "Wtorek" },
    { "Wednesday", "Środa" },
    { "Thursday", "Czwartek" },
    { "Friday", "Piątek" },
    { "Saturday", "Sobota" },
    { "Sunday", "Niedziela" },
    // Months
    { "January", "Styczeń" },
    { "February", "Luty" },
    { "March", "Marzec" },
    { "April", "Kwiecień" },
    { "May", "Maj" },
    { "June", "Czerwiec" },
    { "July", "Lipiec" },
    { "August", "Sierpień" },
    { "September", "Wrzesień" },
    { "October", "Październik" },
    { "November", "Listopad" },
    { "December", "Grudzień" },
    // Weather
    { "Weather", "Pogoda" },
    { "Temperature", "Temperatura" },
    { "Temp", "T" },
    { "in", "wewn" },
    { "out", "zewn" },
    { "Today", "Dzisiaj" },
    { "Tommorow", "Jutro" },
    { "Now", "Teraz" },
};

//------------------------------------------------------------------------------
I18N::I18N(i18n_lang language)
    : m_lang(language)
{

}

//------------------------------------------------------------------------------
const std::string& I18N::translate(i18n_msg_id msg_id)
{
    if (k_i18n.size() <= msg_id)
    {
        utils::err("Error can't find: %d msgid in language file!", msg_id);
        return k_i18n[M_CM_EMPTY][0];
    }
    if (msg_id <= M_CM_LAST)
    {
        return k_i18n[msg_id][0];
    }
    return  k_i18n[msg_id][m_lang];
}


//------------------------------------------------------------------------------
void I18N::loadTranslation(i18n_lang language)
{
    if (language < I18N_LANGUAGES_NUM)
    {
        m_lang = language;
    }
}

//------------------------------------------------------------------------------
