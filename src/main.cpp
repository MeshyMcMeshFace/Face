#include <Arduino.h>
#include <LoRa.h>

#include "ui.h"

#define LORA_IRQ    (26)
#define LORA_SS     (18)
#define LORA_RST    (14)
#define LORA_DI0    (26)
#define LORA_FREQ   (8985E5)

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Initialising.");

    Serial.println("set pin 25 to output.");
    pinMode(25,OUTPUT);
    Serial.println("set led(25) to on.");
    digitalWrite(25,1);
    Serial.println("Initialising UI.");
    ui_init();
    Serial.println("Initialising LORA #1");
    LoRa.setPins(LORA_SS,LORA_RST,LORA_DI0);
    Serial.println("Initialising LORA #2");
    if(!LoRa.begin(LORA_FREQ))
    {
        ui_fault("LoRa Init Failed."); // this never returns.
    }
    Serial.println("set led(25) to off.");
    digitalWrite(25,0);
    Serial.println("Initialised.");
}

void loop() {
    // put your main code here, to run repeatedly:
    ui_display();
   // LoRa.beginPacket();
   // LoRa.printf("HELLO: %d\n",millis());
   // LoRa.endPacket();
}