#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "Task1";

void task1(void *params) {
    while (1) {
        ESP_LOGI(TAG, "Tsk1-P1 <-");
        vTaskDelay(pdMS_TO_TICKS(100)); // Simulate work
        ESP_LOGI(TAG, "Tsk1-P1 ->");
        vTaskDelay(pdMS_TO_TICKS(10)); // Block for 10 milliseconds
    }
}
