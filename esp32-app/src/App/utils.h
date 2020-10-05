#pragma once

#include "DebugUtils.h"

namespace utils
{

    /**
     * @brief Debug macros
     */
    #define dbg(fmt, ...) \
        do { \
            Debug.print(DBG_VERBOSE, fmt,  ##__VA_ARGS__); \
        } while(0)

    #define inf(fmt, ...) \
        do { \
            Debug.print(DBG_INFO, fmt,  ##__VA_ARGS__); \
        } while(0)

    #define wrn(fmt, ...) \
        do { \
            Debug.print(DBG_WARNING, fmt,  ##__VA_ARGS__); \
        } while(0)

    #define err(fmt, ...) \
        do { \
            Debug.print(DBG_ERROR, fmt,  ##__VA_ARGS__); \
        } while(0)


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
    void util_i2c_scanner();
};