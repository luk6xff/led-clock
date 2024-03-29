
#include "i2c_scanner.h"
#include <driver/i2c.h>
#include "sdkconfig.h"

namespace i2c
{

//------------------------------------------------------------------------------
void i2c_scanner(uint8_t bus_num)
{
    int SDA_PIN = GPIO_NUM_21;
    int SCL_PIN = GPIO_NUM_22;
    i2c_port_t I2C_NUM = I2C_NUM_0;
    if (bus_num == 1)
    {
        SDA_PIN = GPIO_NUM_33;
        SCL_PIN = GPIO_NUM_32;
        I2C_NUM = I2C_NUM_1;
    }

    static const char TAG[] = "i2c_scanner";
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    ESP_LOGD(TAG, ">> i2c_scanner");
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)SDA_PIN;
    conf.scl_io_num = (gpio_num_t)SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM, &conf);

    i2c_driver_install(I2C_NUM, I2C_MODE_MASTER, 0, 0, 0);

    int i;
    esp_err_t espRc;
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("00:         ");
    for (i=3; i< 0x78; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_stop(cmd);

        espRc = i2c_master_cmd_begin(I2C_NUM, cmd, 10/portTICK_PERIOD_MS);
        if (i%16 == 0) {
            printf("\n%.2x:", i);
        }
        if (espRc == 0) {
            printf(" %.2x", i);
        } else {
            printf(" --");
        }
        //ESP_LOGV(TAG, "i=%d, rc=%d (0x%x)", i, espRc, espRc);
        i2c_cmd_link_delete(cmd);
    }
    printf("\n");
}
}
