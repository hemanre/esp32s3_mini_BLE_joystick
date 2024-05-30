/*
 * joystick_drvr.h
 *
 *  Created on: 30 May 2024
 *      Author: hema
 */

#ifndef MAIN_DRIVER_JOYSTICK_DRVR_H_
#define MAIN_DRIVER_JOYSTICK_DRVR_H_

//// J1 connector
//#define BUTTON_1      3
//
//QueueHandle_t * joystick_buttons_init(void);
//
//typedef struct {
//    uint8_t pin;
//} button_t;
//
//typedef struct {
//    uint16_t state;
//} joystick_buttons_event_t;

void joystick_drvr_init(void);

#endif /* MAIN_DRIVER_JOYSTICK_DRVR_H_ */
