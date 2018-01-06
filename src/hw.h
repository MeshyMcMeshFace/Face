#ifndef HW_H
#define HW_H
#include <Arduino.h>

void hw_init();

bool hw_is_pgm_down();

void hw_led_on();
void hw_led_off();
void hw_led_toggle();
#endif