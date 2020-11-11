#include "i18n.h"
#include "utils.h"
// #include <ArduinoJson.h>


// const char *I18N::translation =
// {
//     "{  \
//         \"wd_mon\": {\"en\":\"Monday\",     \"pl\":\"Poniedziałek\"}   \
//         \"wd_tue\": {\"en\":\"Tuesday\",    \"pl\":\"Wtorek\"}         \
//         \"wd_wed\": {\"en\":\"Wednesday\",  \"pl\":\"Środa\"}   \
//         \"wd_thu\": {\"en\":\"Thursday\",   \"pl\":\"Czwartek\"}         \
//         \"wd_fri\": {\"en\":\"Friday\",     \"pl\":\"Piątek\"}   \
//         \"wd_sat\": {\"en\":\"Saturday\",   \"pl\":\"Sobota\"}         \
//         \"wd_sun\": {\"en\":\"Sunday\",     \"pl\":\"Niedziela\"}         \
//     }"
// };

const i18n_map I18N::k_i18n =
{
    { "wd_mon", {{"en", "Monday"}, {"pl", "Poniedziałek"}} }
};

//------------------------------------------------------------------------------
I18N::I18N(i18n_lang language)
    : m_lang(language)
{

}

//------------------------------------------------------------------------------
std::string I18N::translate(std::string msgid)
{
    // if (k_i18n.count(msgid))
    // {
    //     if (k_i18n[msgid][""].count())
    //         return k_i18n[msgid][""];
    // }

    utils::err("Error can't find: %s msgid in language file!", msgid.c_str());
    return "";
}


//------------------------------------------------------------------------------
bool I18N::loadTranslation(i18n_lang language)
{

    // DynamicJsonDocument doc(1024);
    // deserializeJson(doc, translation);
    // if (!m_i18n.count(msgid))
    // {
    //     m_i18n.insert(std::make_pair(msgid, msgstr));
    // }

    return true;
}

//------------------------------------------------------------------------------
