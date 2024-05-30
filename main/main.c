/*
 * main.c
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

#include "drivers/button_drvr.h"
#include "drivers/joystick_drvr.h"

void app_main(void)
{
//	button_drvr_init();

	joystick_drvr_init();
}
