/*
 * joystick_drvr.c
 *
 *  Created on: 30 May 2024
 *      Author: hema
 */
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "esp_adc/adc_oneshot.h"

#define JOYSTICK_X_AXIS ADC1_CHANNEL_0 // GPIO1
#define JOYSTICK_Y_AXIS ADC1_CHANNEL_1 // GPIO2
#define JOYSTICK_BUTTON_GPIO GPIO_NUM_3 // GPIO3

#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "joy_drvr";

void joystick_drvr_init(void)
{
    // Configure ADC channels for X and Y axes
    adc1_config_width(ADC_WIDTH_BIT_12); // 12-bit width
    adc1_config_channel_atten(JOYSTICK_X_AXIS, ADC_ATTEN_DB_12); // 0-3.6V range
    adc1_config_channel_atten(JOYSTICK_Y_AXIS, ADC_ATTEN_DB_12); // 0-3.6V range

    // Configure the button GPIO
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << JOYSTICK_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&btn_conf);

    while (1) {
        // Read joystick X and Y axes
        int x_axis_value = adc1_get_raw(JOYSTICK_X_AXIS);
        int y_axis_value = adc1_get_raw(JOYSTICK_Y_AXIS);

        // Read joystick button state
        int button_state = gpio_get_level(JOYSTICK_BUTTON_GPIO);

        // Print the values
        ESP_LOGI(TAG, "X Axis: %d, Y Axis: %d, Button: %d", x_axis_value, y_axis_value, button_state);

        // Delay for a while
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
