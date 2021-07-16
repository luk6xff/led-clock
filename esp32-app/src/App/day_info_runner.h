#pragma once

/**
 * @brief A day info runner class
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2021-07-16
 */

#include "day_info.h"
#include <map>



class DayInfoRunner
{
    struct DayInfoSchedule
    {
        DayInfoSchedule(uint8_t h, uint8_t m, uint8_t s)
            : hour(h)
            , minute(m)
            , seconds(s)
        {
        }

        uint8_t hour;
        uint8_t minute;
        uint8_t seconds;
    };

public:
    explicit DayInfoRunner(const DateTime& dt);
    void process();

private:
    DayInfo m_dinfo;

    /**
     * @brief Day info time table
     */
    static const DayInfoSchedule k_dayInfoScheduleTimeTable[];
};
