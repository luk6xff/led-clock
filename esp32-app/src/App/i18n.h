#pragma once

#include <string>

#define _ translate

namespace i18n {

/**
 * @brief get text form map
 */
std::string translate(std::string msgid);

/**
 * @brief Load language data for language file in a map
 */
int loadTranslation(void);

}
