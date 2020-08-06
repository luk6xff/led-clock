/**
 * @brief Hardware Pin configuration config FILE
 *
 */

// DISPLAY - MAX7219
#define DISPLAY_CLK_PIN  GPIO_NUM_18
#define DISPLAY_CS_PIN   GPIO_NUM_5
#define DISPLAY_DIN_PIN  GPIO_NUM_23

// RTC - DS3231
#define DS3231_I2C_ADDR 0x68
#define DS3231_SDA      GPIO_NUM_21
#define DS3231_SCL      GPIO_NUM_22