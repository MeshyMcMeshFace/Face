#include "hw.h"
#include "ui.h"
bool hw_led;

void hw_init()
{
    ui_println("PIN 25/LED OUTPUT");
    pinMode(25,OUTPUT);
    ui_println("Set LED ON.");
    hw_led_on();    
    ui_println("PIN 0/PGM INPUT");
    pinMode(0,INPUT);
    /*
    ui_println("PRESS PGM FOR SHOW");
    unsigned long done = millis() + 5000;
    while(done > millis())
    {
        if(hw_is_pgm_down())
        {
            ui_diagnostics();
        }
    }
    */
    ui_println("HW_INIT DONE.");
}
bool hw_is_pgm_down()
{
    return !digitalRead(0);
}
void hw_led_on()
{
    digitalWrite(25, 1);
    hw_led = true;
}

void hw_led_off()
{
    digitalWrite(25, 0);
    hw_led = true;
}

void hw_led_toggle()
{
    if(hw_led)
        hw_led_off();
    else
        hw_led_on();
}
