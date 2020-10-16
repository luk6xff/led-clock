#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Setup Test function for each device module - used during development
 */
void tests_setup();

/**
 * @brief Test function for each device module - used during development
 */
void tests_run();

#ifdef __cplusplus
}
#endif