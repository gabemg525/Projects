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

#define PIN_LED    2   // GPIO pin number for LED
#define PIN_SWITCH 13   // GPIO pin number for Switch
#define INPUT_PIN 13   // GPIO pin number for Switch

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
static void IRAM_ATTR gpio_interrupt_handler(void *args){
    int pinNumber = (int)args;
   static RTC_RODATA_ATTR const char fmt_str[] = 
       "Hello from %d\n";
   esp_rom_printf( fmt_str, pinNumber );
}

void app_main(){
    esp_rom_gpio_pad_select_gpio( INPUT_PIN );
    gpio_set_direction( INPUT_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en( INPUT_PIN );
    gpio_pullup_dis( INPUT_PIN );
    gpio_set_intr_type( INPUT_PIN, GPIO_INTR_POSEDGE );
    gpio_install_isr_service( 0 );
    gpio_isr_handler_add( INPUT_PIN, 
      gpio_interrupt_handler, 
      (void *)INPUT_PIN );
    // app_main can terminate
}

