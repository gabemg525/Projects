/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// #include <stdio.h>
// #include <inttypes.h>
// #include "sdkconfig.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_chip_info.h"
// #include "esp_flash.h"
// #include "esp_system.h"
// #include "driver/gpio.h"

// #define PIN_LED    2   // GPIO pin number for LED
// #define PIN_SWITCH 13   // GPIO pin number for Switch
// #define INPUT_PIN 13   // GPIO pin number for Switch

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
// static void IRAM_ATTR gpio_interrupt_handler(void *args){
//     int pinNumber = (int)args;
//    static RTC_RODATA_ATTR const char fmt_str[] = 
//        "Hello from %d\n";
//    esp_rom_printf( fmt_str, pinNumber );
// }

// void app_main(){
//     esp_rom_gpio_pad_select_gpio( INPUT_PIN );
//     gpio_set_direction( INPUT_PIN, GPIO_MODE_INPUT);
//     gpio_pulldown_en( INPUT_PIN );
//     gpio_pullup_dis( INPUT_PIN );
//     gpio_set_intr_type( INPUT_PIN, GPIO_INTR_POSEDGE );
//     gpio_install_isr_service( 0 );
//     gpio_isr_handler_add( INPUT_PIN, 
//       gpio_interrupt_handler, 
//       (void *)INPUT_PIN );
//     // app_main can terminate
// }

// void print_memory_report() {
//     printf("Free heap: %ld bytes\n", esp_get_free_heap_size());
//     printf("Minimum free heap ever: %d bytes\n", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
//     printf("Largest free block in heap: %d bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
//     printf("Free internal memory: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
// }

// void app_main(void)
// {
//     while (true) {
//         print_memory_report();
//         vTaskDelay(5000 / portTICK_PERIOD_MS); // Delay for 5 seconds
//     }
// }

// #include "esp_system.h"
// #include "esp_heap_caps.h"
// #include "esp_log.h"

// void app_main(void)
// {
//     ESP_LOGI("MEMORY", "Total free DRAM: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT));
//     ESP_LOGI("MEMORY", "Minimum free DRAM ever: %d bytes", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
//     ESP_LOGI("MEMORY", "Largest free block DRAM: %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

//     ESP_LOGI("MEMORY", "Total free IRAM: %d bytes", heap_caps_get_free_size(MALLOC_CAP_32BIT));
//     ESP_LOGI("MEMORY", "Minimum free IRAM ever: %d bytes", heap_caps_get_minimum_free_size(MALLOC_CAP_32BIT));
//     ESP_LOGI("MEMORY", "Largest free block IRAM: %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_32BIT));
// }





#include <stdio.h>
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>


#define STACK_SIZE 2048
static const char *TAG = "MEMORY_REPORT";

// Function to print heap memory statistics
void print_heap_memory_report(const char *message) {
    ESP_LOGI(TAG, "\n\n");
    ESP_LOGI(TAG, "%s", message);
    ESP_LOGI(TAG, "Free heap size: %ld bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum free heap ever: %d bytes", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
    ESP_LOGI(TAG, "Largest free block in heap: %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    ESP_LOGI(TAG, "Free internal memory: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    ESP_LOGI(TAG, "xPortGetFreeHeapSize %d = DRAM", xPortGetFreeHeapSize());
    
}

// Task that allocates some memory and simulates work
void memory_consuming_task(void *pvParameter) {
    ESP_LOGI(TAG, "\n\n");
    ESP_LOGI(TAG, "Memory consuming task started");

    // Allocate memory from the heap
    char *buffer = (char *)malloc(1024);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        vTaskDelete(NULL);
        return;
    }

    // Perform some operations on the allocated memory
    for (int i = 0; i < 1024; i++) {
        buffer[i] = i % 256;
    }

    // Print stack usage
    UBaseType_t stack_high_water_mark = uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI(TAG, "Stack high water mark: %d words", stack_high_water_mark);

    // Free the allocated memory
    free(buffer);

    ESP_LOGI(TAG, "Memory consuming task completed");

    // Print stack usage after the task
    stack_high_water_mark = uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI(TAG, "Stack high water mark after task: %d words", stack_high_water_mark);

    vTaskDelete(NULL);
}

static const char *TAG_2 = "example";

void simulate_error() {
    // Simulate a failure in an operation, for example, file operation
    FILE *file = fopen("/nonexistent_file.txt", "r");
    if (file == NULL) {
        int err = errno; // Capture the errno value
        ESP_LOGE(TAG_2, "Failed to open file: %s", strerror(err)); // Log the error
        // Handle the error as needed
    } else {
        // If file opened successfully, close it
        fclose(file);
    }
}

void app_main(void) {

    printf("Testing errno functionality for file system\n");
    simulate_error();


    printf("\ntesting heap memory report\n");
    print_heap_memory_report("Memory report before running task:");

    // Create a FreeRTOS task that will consume memory
    xTaskCreate(memory_consuming_task, "MemoryTask", STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

    // Wait for the task to finish
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    print_heap_memory_report("Memory report after running task:");
}
