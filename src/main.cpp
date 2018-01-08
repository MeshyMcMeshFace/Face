#include <Arduino.h>
#include <LoRa.h>
#include <WiFi.h>

#include "hw.h"
#include "ui.h"

#include <KissRadio.h>
#include <LoopbackRadio.h>

#define LORA_IRQ    (26)
#define LORA_SS     (18)
#define LORA_RST    (14)
#define LORA_DI0    (26)
#define LORA_FREQ   (898575E3) 

uint8_t send[514];
uint8_t recv[514];
size_t slen,rlen;
bool waitingToSend;

RadioClass *kiss, *loopback; 

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    
    ui_init();
    
    hw_init();
/*
    ui_println("LORA INIT#1");
    LoRa.setPins(LORA_SS,LORA_RST,LORA_DI0);
    ui_println("LORA INIT#2");
    if(!LoRa.begin(LORA_FREQ))
    {
        ui_fault("LoRa Init Failed."); // this never returns.
    }
    ui_println("KISS INIT");
    Kiss.begin();
*/
    ui_println("new LoopbackRadio");
    loopback = new LoopbackRadioClass();
    ui_println("new KissRadio");
    kiss = new KissRadioClass(loopback);

    ui_println("Initialised.");
    WiFi.begin();
    ui_println(WiFi.macAddress().c_str());

    memset((void *)recv,0,514);
    memset((void *)send,0,514);
    rlen = 0;
    slen = 0;
    waitingToSend = false;
}

void loop() {
    while(Serial.available() && !waitingToSend)
    {
        int ch = Serial.read();
        send[slen] = ch;

        // make sure we catch the first 0xC0
        if(slen == 0 && ch != 0xC0)
        {
            break;
        }

        slen ++;

        // make sure we catch the 'last' 0xC0
        if(slen > 3 && ch == 0xC0)
        {
            waitingToSend = true;
            break;
        }
    }
    if(waitingToSend)
    {
        if(kiss->canSend())
        {
            // we don't care about buffer overruns here. 
            kiss->send(nullptr,send,slen);
            waitingToSend = false;
            ui_printf("TX %d bytes",slen);
            memset((void *)send,0,514);
            slen = 0;
            hw_led_toggle();
            ui_printf("TEST? %d", kiss->canRecv());
        }
    }
    if(kiss->canRecv())
    {
        int rssi;
        float snr;
        kiss->recv(nullptr,nullptr,recv,&rlen,&rssi,&snr);
        ui_printf("RX %d bytes",rlen);
        ui_printf("RSSI %d, SNR %f",rssi,snr);
        hw_led_toggle();
        Serial.write(recv,rlen);
        hw_led_toggle();
        memset((void *)recv,0,514);
        rlen = 0;
    }
}