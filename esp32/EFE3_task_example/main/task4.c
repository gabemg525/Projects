#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "config.h"  // Include the header file for global definitions

static const char *TAG = "Task4";

extern SemaphoreHandle_t xSemaphoreTask4;

void task4(void *params) {
    while (1) {
        xSemaphoreTake(xSemaphoreTask4, portMAX_DELAY);
        ESP_LOGI(TAG, "Tsk4-P4 <-");

        // Simulate switch press detection
        if (gpio_get_level(SWITCH_GPIO) == 1) {
            vTaskDelay(pdMS_TO_TICKS(10)); // Simulate work
        }

        ESP_LOGI(TAG, "Tsk4-P4 ->");
        xSemaphoreGive(xSemaphoreTask4);
        vTaskDelay(pdMS_TO_TICKS(100)); // Block until switch is pressed again
    }
}
