#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "dfrobot_c_lib.h"

#define LCD_ADDRESS     (0x7c>>1)
#define RGB_ADDRESS     (0xc0>>1)
#define SDA_PIN 10      // GPIO number for I2C master clock
#define SCL_PIN 8       // GPIO number for I2C master data
#define I2C_PORT I2C_NUM_0   // I2C port number
#define ICM_ADDR 0x68              // ICM-42670-P device address

void i2c_master_init() {
    i2c_config_t conf = {
        .mode               = I2C_MODE_MASTER,
        .sda_io_num         = SDA_PIN,
        .sda_pullup_en      = GPIO_PULLUP_ENABLE,
        .scl_io_num         = SCL_PIN,
        .scl_pullup_en      = GPIO_PULLUP_ENABLE,
        .master = {
                .clk_speed = 1000000,
        },
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));
}

