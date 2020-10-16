#pragma once

#include <string>
#include <map>
#include <utility>

#define _ translate





typedef enum
{
    ENGLISH,
    POLISH,
    LAST_LANGUAGE
} i18n_lang;

using i18n_map = std::map<std::string, std::map<std::string, std::string>>;

class I18N {

public:

    explicit I18N(i18n_lang language);

    /**
     * @brief get text form map
     */
    std::string translate(std::string msgid);

    /**
     * @brief Load language data for language file in a map
     */
    bool loadTranslation(i18n_lang language);

private:

    i18n_lang m_lang;
    static const i18n_map k_i18n;

};

