#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define SERVO_CHANNEL LEDC_CHANNEL_0
#define SERVO_TIMER LEDC_TIMER_0
#define SERVO_GPIO 18  // Change to your GPIO pin number

// PWM frequency in Hz (typically 50 Hz for servos)
#define SERVO_FREQUENCY 50
#define SERVO_RESOLUTION LEDC_TIMER_13_BIT
#define SERVO_MIN_PULSE_WIDTH 500  // Minimum pulse width in microseconds (typically 500)
#define SERVO_MAX_PULSE_WIDTH 2500  // Maximum pulse width in microseconds (typically 2500)

// Function to initialize PWM
void pwm_init() {
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = SERVO_TIMER,
        .duty_resolution = SERVO_RESOLUTION,
        .freq_hz = SERVO_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = SERVO_CHANNEL,
        .timer_sel = SERVO_TIMER,
        .gpio_num = SERVO_GPIO,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&channel_conf);
}

// Function to set servo angle
void servo_set_angle(int angle) {
    if (angle < 0 || angle > 180) {
        ESP_LOGE("SERVO", "Angle out of range. Must be between 0 and 180.");
        return;
    }

    uint32_t pulse_width = SERVO_MIN_PULSE_WIDTH + (angle * (SERVO_MAX_PULSE_WIDTH - SERVO_MIN_PULSE_WIDTH)) / 180;
    uint32_t duty = (pulse_width * (1 << SERVO_RESOLUTION)) / (1000000 / SERVO_FREQUENCY);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL);
}

void app_main() {
    pwm_init();

    while (1) {
        for (int angle = 0; angle <= 180; angle += 10) {
            servo_set_angle(angle);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        for (int angle = 180; angle >= 0; angle -= 10) {
            servo_set_angle(angle);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}
