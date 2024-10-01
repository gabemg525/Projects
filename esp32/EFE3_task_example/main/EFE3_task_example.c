#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "config.h"  // Include the header file for global definitions
#include "tasks.h"  // Include the header file for task declarations

SemaphoreHandle_t xSemaphoreTask3;
SemaphoreHandle_t xSemaphoreTask4;

void app_main(void) {
    // Initialize hardware
    esp_rom_gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(SWITCH_GPIO);
    gpio_set_direction(SWITCH_GPIO, GPIO_MODE_INPUT);

    // Create semaphores
    xSemaphoreTask3 = xSemaphoreCreateBinary();
    xSemaphoreTask4 = xSemaphoreCreateBinary();


    if (xSemaphoreTask3 == NULL || xSemaphoreTask4 == NULL) {
        ESP_LOGE("SEM", "Failed to create semaphores");
        return;
    }
    // Create tasks
    xTaskCreate(task1, "Task1", 2048, NULL, 1, NULL);
    xTaskCreate(task2, "Task2", 2048, NULL, 2, NULL);
    xTaskCreate(task3, "Task3", 2048, NULL, 3, NULL);
    xTaskCreate(task4, "Task4", 2048, NULL, 4, NULL);
}
