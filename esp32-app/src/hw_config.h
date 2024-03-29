/**
 * @brief ESP-32 (ESP-WROOM-32D) Hardware Pin configuration config FILE
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-20
 */

// DISPLAY - MAX7219
#define DISPLAY_CLK_PIN     GPIO_NUM_18
#define DISPLAY_CS_PIN      GPIO_NUM_5
#define DISPLAY_DIN_PIN     GPIO_NUM_23

// RTC - DS3231
#define DS3231_I2C_ADDR     0x68
#define DS3231_SDA_PIN      GPIO_NUM_21
#define DS3231_SCL_PIN      GPIO_NUM_22
#define DS3231_VDD_PIN      GPIO_NUM_4

// INTERNAL DATA SENSOR - BME280
#define BME280_I2C_ADDR     0x76 //0xEC
#define BME280_SDA_PIN      GPIO_NUM_33
#define BME280_SCL_PIN      GPIO_NUM_32

// LIGHT ILLUMINANCE SENSOR - BH1750
#define BH1750_I2C_ADDR     0x23 //ADDR PIN is conected to GND
#define BH1750_SDA_PIN      GPIO_NUM_33
#define BH1750_SCL_PIN      GPIO_NUM_32

// LORA RADIO - SX1278 (RA-01)
#define RADIO_MOSI_PIN      GPIO_NUM_13
#define RADIO_MISO_PIN      GPIO_NUM_12
#define RADIO_SCK_PIN       GPIO_NUM_14
#define RADIO_NSS_PIN       GPIO_NUM_15
#define RADIO_RST_PIN       GPIO_NUM_27
#define RADIO_DI0_PIN       GPIO_NUM_26
