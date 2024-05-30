/*
 * button_drvr.c
 *
 *  Created on: 30 May 2024
 *      Author: hema
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define GPIO_INPUT_IO_0          3
#define GPIO_INPUT_PIN_SEL       (1ULL<<GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT    0
#define DEBOUNCE_TIME_MS         50 // 50 ms debounce time
#define DOUBLE_PRESS_INTERVAL_MS 300 // 300 ms for detecting double press
#define BUTTON_RELEASE_FLAG      1
#define BUTTON_PRESSED_FLAG      2

static QueueHandle_t gpio_evt_queue = NULL;
static uint32_t last_isr_time = 0;

typedef enum {
    SINGLE_PRESS,
    DOUBLE_PRESS,
    NO_PRESS
} press_type_t;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    uint32_t current_time = (uint32_t) (esp_timer_get_time() / 1000);

    // Ignore interrupt within the debounce period
    if (current_time - last_isr_time > DEBOUNCE_TIME_MS) {
        last_isr_time = current_time;
        xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
    }
}

static press_type_t detect_press_type()
{
    static int press_count = 0;
    static uint32_t last_press_time = 0;

    uint32_t current_time = (uint32_t) (esp_timer_get_time() / 1000);
    press_count++;

    if (press_count == BUTTON_RELEASE_FLAG) {
        last_press_time = current_time;
        return NO_PRESS; // Wait for a possible second press
    } else if (press_count == BUTTON_PRESSED_FLAG) {
        if (current_time - last_press_time < DOUBLE_PRESS_INTERVAL_MS) {
            press_count = 0;
            return DOUBLE_PRESS;
        } else {
            press_count = 1;
            last_press_time = current_time;
            return SINGLE_PRESS;
        }
    }

    return NO_PRESS;
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            int level = gpio_get_level(io_num);

            if (level == BUTTON_RELEASE_FLAG) {
                press_type_t press_type = detect_press_type();

                switch (press_type) {
                    case SINGLE_PRESS:
                        printf("GPIO[%" PRIu32 "] Single Press\n", io_num);
                        break;
                    case DOUBLE_PRESS:
                        printf("GPIO[%" PRIu32 "] Double Press\n", io_num);
                        break;
                    case NO_PRESS:
                        break;
                }
            }
        }
    }
}

void button_drvr_init(void)
{
    // Zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // Interrupt of both rising and falling edge
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    // Bit mask of the pins, use GPIO4 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // Set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // Enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // Create a queue to handle gpio event from ISR
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // Start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    // Install gpio ISR service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // Hook ISR handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    int cnt = 0;
    while (1) {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
