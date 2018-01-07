#include <Arduino.h>
#include <LoRa.h>
#include <WiFi.h>

#include "hw.h"
#include "ui.h"

#include "kiss.h"

#define LORA_IRQ    (26)
#define LORA_SS     (18)
#define LORA_RST    (14)
#define LORA_DI0    (26)
#define LORA_FREQ   (898575E3) 

uint8_t buffer[514];
int bufflen;


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    
    ui_init();
    
    hw_init();

    ui_println("LORA INIT#1");
    LoRa.setPins(LORA_SS,LORA_RST,LORA_DI0);
    ui_println("LORA INIT#2");
    if(!LoRa.begin(LORA_FREQ))
    {
        ui_fault("LoRa Init Failed."); // this never returns.
    }
    ui_println("KISS INIT");
    Kiss.begin();
    ui_println("Initialised.");
    memset((void *)buffer,0,514);
    WiFi.begin();
    ui_println(WiFi.macAddress().c_str());
}

void loop() {
    // put your main code here, to run repeatedly:
    //ui_display();
    // LoRa.beginPacket();
    // LoRa.printf("HELLO: %d\n",millis());
    // LoRa.endPacket();
    bufflen = Kiss.available();
    if(bufflen)
    {
        bufflen=Kiss.readBytes(buffer,bufflen);
        Serial.write(buffer,bufflen);
        ui_printf("%5d TX %4d",millis()%10000,bufflen);
    }
    bufflen = Serial.available();
    if(Kiss.hasStatusChanged())
    {
        ui_println(Kiss.getStatus());
        hw_led_toggle();
    }
    if(bufflen)
    {
        bufflen=Serial.readBytes(buffer,bufflen);
        Kiss.write(buffer,bufflen);
        ui_printf("%5d RX %4d",millis()%10000,bufflen);
    }
    if(Kiss.hasStatusChanged())
    {
        ui_println(Kiss.getStatus());
        hw_led_toggle();
    }
}