// #include <stdio.h>
// #include "driver/i2c.h"
// #include "driver/ledc.h"
// #include "esp_log.h"
// #include "esp_system.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #define I2C_MASTER_SCL_IO           10    // SCL pin for ESP32-S3
// #define I2C_MASTER_SDA_IO           8     // SDA pin for ESP32-S3
// #define I2C_MASTER_NUM              I2C_NUM_0
// #define I2C_MASTER_FREQ_HZ          100000
// #define I2C_MASTER_TIMEOUT_MS       1000
// #define LCD_ADDR                    0x27  // I2C address of the LCD

// #define LEDC_CHANNEL_R              LEDC_CHANNEL_0
// #define LEDC_CHANNEL_G              LEDC_CHANNEL_1
// #define LEDC_CHANNEL_B              LEDC_CHANNEL_2
// #define LEDC_TIMER                  LEDC_TIMER_0
// #define LEDC_FREQUENCY              5000 // 5 kHz
// #define LEDC_RESOLUTION             LEDC_TIMER_8_BIT // 8-bit resolution

// static const char *TAG = "LCD_Init";

// void lcd_init() {
//     // Define your initialization sequence here
//     uint8_t init_sequence[] = {
//         0x03, 0x03, 0x03, 0x02, 0x02, 0x08, 0x00, 0x0C, 0x00, 0x06
//     };

//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (LCD_ADDR << 1) | I2C_MASTER_WRITE, true);
//     i2c_master_write(cmd, init_sequence, sizeof(init_sequence), true);
//     i2c_master_stop(cmd);
//     esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
//     i2c_cmd_link_delete(cmd);

//     if (err == ESP_OK) {
//         ESP_LOGI(TAG, "LCD initialized successfully");
//     } else {
//         ESP_LOGE(TAG, "Failed to initialize LCD");
//     }

//     // Delay to ensure LCD initialization completes
//     vTaskDelay(200 / portTICK_PERIOD_MS);
// }

// void pwm_init() {
//     ledc_timer_config_t timer_conf = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .timer_num = LEDC_TIMER,
//         .duty_resolution = LEDC_RESOLUTION,
//         .freq_hz = LEDC_FREQUENCY,
//         .clk_cfg = LEDC_AUTO_CLK
//     };
//     ledc_timer_config(&timer_conf);

//     ledc_channel_config_t ch_conf_r = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .channel = LEDC_CHANNEL_R,
//         .timer_sel = LEDC_TIMER,
//         .intr_type = LEDC_INTR_DISABLE,
//         .gpio_num = 12, // GPIO for Red LED
//         .duty = 0,
//         .hpoint = 0
//     };
//     ledc_channel_config(&ch_conf_r);

//     ledc_channel_config_t ch_conf_g = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .channel = LEDC_CHANNEL_G,
//         .timer_sel = LEDC_TIMER,
//         .intr_type = LEDC_INTR_DISABLE,
//         .gpio_num = 13, // GPIO for Green LED
//         .duty = 0,
//         .hpoint = 0
//     };
//     ledc_channel_config(&ch_conf_g);

//     ledc_channel_config_t ch_conf_b = {
//         .speed_mode = LEDC_LOW_SPEED_MODE,
//         .channel = LEDC_CHANNEL_B,
//         .timer_sel = LEDC_TIMER,
//         .intr_type = LEDC_INTR_DISABLE,
//         .gpio_num = 14, // GPIO for Blue LED
//         .duty = 0,
//         .hpoint = 0
//     };
//     ledc_channel_config(&ch_conf_b);

//     // Delay to ensure PWM setup completes
//     vTaskDelay(100 / portTICK_PERIOD_MS);

//     // Set green color (full green, no red, no blue)
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_R, 0); // Red
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_G, 255); // Green
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_B, 0); // Blue

//     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_R);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_G);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_B);
// }

// void app_main(void)
// {
//     // Configure I2C master interface
//     i2c_config_t conf = {
//         .mode = I2C_MODE_MASTER,
//         .sda_io_num = I2C_MASTER_SDA_IO,
//         .sda_pullup_en = GPIO_PULLUP_ENABLE,
//         .scl_io_num = I2C_MASTER_SCL_IO,
//         .scl_pullup_en = GPIO_PULLUP_ENABLE,
//         .master.clk_speed = I2C_MASTER_FREQ_HZ,
//     };
//     ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
//     ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
//     vTaskDelay(2000 / portTICK_PERIOD_MS);
//     lcd_init();
//     pwm_init();

//     // Your code to use the LCD
// }

#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO    10    // Change to your SCL pin
#define I2C_MASTER_SDA_IO    8     // Change to your SDA pin
#define I2C_MASTER_NUM       I2C_NUM_0
#define I2C_MASTER_FREQ_HZ   100000
#define I2C_MASTER_TIMEOUT_MS 1000

static const char *TAG = "I2C_Scanner";

void i2c_scan() {
    esp_err_t err;
    printf("Starting I2C scan...\n");

    for (int addr = 0x03; addr < 0x77; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (err == ESP_OK) {
            printf("I2C device found at address 0x%02X\n", addr);
        } else if (err == ESP_ERR_INVALID_STATE) {
            printf("No I2C device found at address 0x%02X\n", addr);
        }
    }
}

void app_main(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    i2c_scan();
}
