#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "Task2";

void task2(void *params) {
    while (1) {
        ESP_LOGI(TAG, "Tsk2-P2 <-");
        vTaskDelay(pdMS_TO_TICKS(10)); // Simulate work
        ESP_LOGI(TAG, "Tsk2-P2 ->");
        vTaskDelay(pdMS_TO_TICKS(250)); // Block for 250 milliseconds
    }
}
