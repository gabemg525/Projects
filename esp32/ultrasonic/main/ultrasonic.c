#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"


#define TRIGGER_GPIO 0

#define ECHO_GPIO 1
#define MAX_DISTANCE_CM 400
#define TIMEOUT_US (MAX_DISTANCE_CM*58)

#define SHTC3_I2C_ADDRESS 0x70 // SHTC3 I2C address

#define SHTC3_MEASURE_CMD 0x7866 // Command to start a temperature and humidity measurement
#define SHTC3_MEASURE_DELAY_MS 1000 // Delay between measurement start and result read

#define SHTC3_TEMPERATURE_FACTOR 175.0 // Temperature conversion factor
#define SHTC3_TEMPERATURE_OFFSET -45.0 // Temperature conversion offset
#define SHTC3_HUMIDITY_FACTOR 100.0 // Humidity conversion factor

#define I2C_MASTER_SCL_IO 8 // I2C clock pin
#define I2C_MASTER_SDA_IO 10 // I2C data pin
#define I2C_MASTER_NUM I2C_NUM_0 // I2C port number
#define I2C_MASTER_FREQ_HZ 100000 // I2C clock frequency

static const char *TAG = "SHTC3";

float celsius_to_fahrenheit(float celsius) {
    return celsius * 1.8f + 32.0f;
}
static float calculate_humidity(uint16_t raw_humidity){
    return 100.0 * (float)raw_humidity / 65535.0;
}
static float calculate_temp(uint16_t raw_temp){
    return (175.0 * (float)raw_temp / 65535.0) - 45;
}


void delay_us(int usecs) {
    uint32_t end_time = esp_timer_get_time() + usecs;
    while(esp_timer_get_time()<end_time)
    ;
}
void app_main() {
    // Configure I2C interface
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
    };
    
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    uint16_t raw_humidity, raw_temp;
    float humidity, temperature;
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TRIGGER_GPIO,
    };
    gpio_config(&io_conf);
    io_conf.intr_type = GPIO_INTR_POSEDGE, io_conf.mode = GPIO_MODE_INPUT,
    io_conf.pin_bit_mask = 1ULL<<ECHO_GPIO, gpio_config(&io_conf);

    while (true) {
        

        // Start a temperature and humidity measurement
        uint8_t cmd[2] = {SHTC3_MEASURE_CMD >> 8, SHTC3_MEASURE_CMD & 0xFF};
        i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
        i2c_master_start(cmd_handle);
        i2c_master_write_byte(cmd_handle, SHTC3_I2C_ADDRESS << 1 | I2C_MASTER_WRITE, true);
        i2c_master_write(cmd_handle, cmd, sizeof(cmd), true);
        i2c_master_stop(cmd_handle);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle, pdMS_TO_TICKS(SHTC3_MEASURE_DELAY_MS));
        i2c_cmd_link_delete(cmd_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start measurement: %d", ret);
            continue;
        }
        vTaskDelay(20);
        // Read temperature and humidity data
        uint8_t data[6];
        cmd_handle = i2c_cmd_link_create();
        i2c_master_start(cmd_handle);
        i2c_master_write_byte(cmd_handle, SHTC3_I2C_ADDRESS << 1 | I2C_MASTER_READ, true);
        i2c_master_read(cmd_handle, data, sizeof(data), I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd_handle);
        ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle, pdMS_TO_TICKS(SHTC3_MEASURE_DELAY_MS));
        i2c_cmd_link_delete(cmd_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read measurement data: %d", ret);
            continue;
        }
        // Convert raw data to temperature and humidity values
        raw_temp = (data[0]<<8) | data[1];

        temperature = calculate_temp(raw_temp);
        raw_humidity = (data[3]<<8) | data[4];
        humidity = calculate_humidity(raw_humidity);

        // Print temperature in Celsius and Fahrenheit
        float fahrenheit = celsius_to_fahrenheit(temperature);
        ESP_LOGI(TAG, "Temperature: %.2f C (%.2f F)", temperature, fahrenheit);

        // Print humidity
        ESP_LOGI(TAG, "Humidity: %.2f %%", humidity);

        gpio_set_level(TRIGGER_GPIO, 0);
        esp_rom_delay_us(2);
        gpio_set_level(TRIGGER_GPIO, 1);
        esp_rom_delay_us(10);
        gpio_set_level(TRIGGER_GPIO, 0);
        uint32_t start = esp_timer_get_time();
        while (gpio_get_level(ECHO_GPIO)==0 && 
            esp_timer_get_time() - start < TIMEOUT_US)
        ;
        start = esp_timer_get_time();
        while (gpio_get_level(ECHO_GPIO)==1 && 
            esp_timer_get_time() - start < TIMEOUT_US)
        ;

        uint32_t useconds_sound_travel = (esp_timer_get_time()-start);
        float distance_cm =   ((331.4 + 0.6 * temperature) * (float)useconds_sound_travel / 1000000 ) *50;
        printf("Distance in cm: %.2f cm\n", distance_cm);
        printf("useconds_sound_travel: %d us\n", (int)useconds_sound_travel);

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}