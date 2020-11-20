#pragma once

#include "DebugUtils.h"

namespace utils
{

    /**
     * @brief Debug macros
     */
    template<typename... Args>
    void dbg(const char *fmt, const Args&... args)
    {
        do
        {
            Debug.print(DBG_VERBOSE, fmt, std::forward<const Args>(args)...);
        } while(0);
    }


    template<typename... Args>
    void inf(const char *fmt, const Args&... args)
    {
        do
        {
            Debug.print(DBG_INFO, fmt, std::forward<const Args>(args)...);
        } while(0);
    }


    template<typename... Args>
    void wrn(const char *fmt, const Args&... args)
    {
        do
        {
            Debug.print(DBG_WARNING, fmt, std::forward<const Args>(args)...);
        } while(0);
    }


    template<typename... Args>
    void err(const char *fmt, const Args&... args)
    {
        do
        {
            Debug.print(DBG_ERROR, fmt, std::forward<const Args>(args)...);
        } while(0);
    }


    /**
     * @brief Initializes, Serial and debug verbisty
     * @note Must be called in the main.cpp in the setup function
     */
    void init();


    /**
     * @brief Updates all the needed global utils state
     * @note Must be called in the main.cpp in the loop function
     */
    void update();

    /**
     * @brief Searches for i2c devices on the bus
     */
    void util_i2c_scanner(uint8_t bus_num);
};