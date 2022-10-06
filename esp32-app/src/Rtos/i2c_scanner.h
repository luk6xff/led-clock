#pragma once

#include <cstdint>

namespace i2c
{

/**
 * @brief Searches for i2c devices on the bus
 */
void i2c_scanner(uint8_t bus_num);

}
