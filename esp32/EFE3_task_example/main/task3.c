#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "config.h"  // Include the header file for global definitions
#include "semaphores.h"

static const char *TAG = "Task3";

void task3(void *params) {
    char input_char;
    while (1) {
        xSemaphoreTake(xSemaphoreTask3, portMAX_DELAY);
        ESP_LOGI(TAG, "Tsk3-P3 <-");

        // Simulate reading from UART
        uart_read_bytes(UART_NUM_0, &input_char, 1, portMAX_DELAY);

        if (input_char == 'L' || input_char == 'l') {
            gpio_set_level(LED_GPIO, 1); // Turn LED on
            vTaskDelay(pdMS_TO_TICKS(100)); // Simulate work
            gpio_set_level(LED_GPIO, 0); // Turn LED off
        }

        ESP_LOGI(TAG, "Tsk3-P3 ->");
        xSemaphoreGive(xSemaphoreTask3);
        vTaskDelay(pdMS_TO_TICKS(50)); // Block until next input
    }
}
