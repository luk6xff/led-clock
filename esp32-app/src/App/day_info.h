#pragma once

/**
 * @brief A day info class, responsible for preparing a day info message
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2021-07-16
 */

#include "Clock/system_time.h"


class DayInfo
{

public:
    explicit DayInfo(const DateTime& dt);
    String createMsg();

    const DateTime& dt();

private:
    DateTime m_dt;
};
