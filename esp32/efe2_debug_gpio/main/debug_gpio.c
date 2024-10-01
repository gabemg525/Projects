/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "esp_heap_caps.h"

#include "freertos/semphr.h"


#define PIN_LED    2   // GPIO pin number for LED
#define PIN_SWITCH 13   // GPIO pin number for Switch
#define INPUT_PIN 13   // GPIO pin number for Switch
static SemaphoreHandle_t xLogSemaphore;
// void app_main(void)
// {
//     gpio_set_direction( PIN_LED, GPIO_MODE_OUTPUT );
//     gpio_set_direction( PIN_SWITCH, GPIO_MODE_INPUT );
//     gpio_pulldown_en( PIN_SWITCH);
//     gpio_pulldown_dis( PIN_SWITCH);
//     printf("GPIO_NUM_2 = %d\n", GPIO_NUM_2);
//     int isOn = 0;
//     while (true){
//         isOn = !isOn;
//         int level =  gpio_get_level(PIN_SWITCH);
//         printf("level = %d\n", level);
//         gpio_set_level( PIN_LED, level);
//         vTaskDelay(1);
//     }
// }


void log_memory_usage() {
    // Get free memory in different categories
    size_t free_iram = heap_caps_get_free_size(MALLOC_CAP_32BIT | MALLOC_CAP_EXEC);
    size_t free_dram = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t free_irom = esp_get_free_heap_size();
    size_t free_drom = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    ESP_LOGI("Memory", "Free IRAM: %d bytes", free_iram);
    ESP_LOGI("Memory", "Free DRAM: %d bytes", free_dram);
    ESP_LOGI("Memory", "Free IROM: %d bytes", free_irom);
    ESP_LOGI("Memory", "Free DROM: %d bytes", free_drom);
}

static void IRAM_ATTR gpio_interrupt_handler(void *args) {
    // Signal the logging task
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xLogSemaphore, &xHigherPriorityTaskWoken);

    // Yield if needed
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

void logging_task(void *pvParameter) {
    while (true) {
        // Wait for ISR to signal
        if (xSemaphoreTake(xLogSemaphore, portMAX_DELAY) == pdTRUE) {
            log_memory_usage();
        }
    }
}

void app_main() {
    // Create the semaphore
    xLogSemaphore = xSemaphoreCreateBinary();

    // Start the logging task
    xTaskCreate(logging_task, "Logging Task", 2048, NULL, 10, NULL);

    // Initialize GPIO for interrupt
    esp_rom_gpio_pad_select_gpio(INPUT_PIN);
    gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(INPUT_PIN);
    gpio_pullup_dis(INPUT_PIN);
    gpio_set_intr_type(INPUT_PIN, GPIO_INTR_POSEDGE);

    // Install ISR service and handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INPUT_PIN, gpio_interrupt_handler, (void *)INPUT_PIN);

    // app_main can terminate
}