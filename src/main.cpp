#include <Arduino.h>

#include "ui.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Hello.");
    ui_init();
}

void loop() {
    // put your main code here, to run repeatedly:
    ui_display();
}