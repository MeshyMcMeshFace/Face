#include <Arduino.h>
#include <LoRa.h>
#include <WiFi.h>
#include "ui.h"

#define LORA_IRQ    (26)
#define LORA_SS     (18)
#define LORA_RST    (14)
#define LORA_DI0    (26)
#define LORA_FREQ   (8985E5)

uint8_t ident[6] = { 0,0,0,0,0,0};
char    last_rx[21];
char    last_tx[9];
int last_rssi;
float last_snr;

typedef struct hw_init_s { uint8_t pin, state, value; } hw_init_t;

hw_init_t hw_init_data[] {
    { 25 , OUTPUT, HIGH }, // GPIO25 is the LED
    {  0 , INPUT, LOW }    // GPIO 0 is the PRG switch.
 };
void hw_init()
{
    char state[16];
    ui_println("Init Hardware.");
    for(int i=0;i<2;i++)
    {
        switch(hw_init_data[i].state)
        {
            case OUTPUT: strncpy(state,"OUT",16); break;
            case INPUT:  strncpy(state,"INP",16); break;
            case INPUT_PULLDOWN: strncpy(state,"INP_PD",16); break;
            case INPUT_PULLUP: strncpy(state,"INP_PU",16); break;
            default: strncpy(state,"???",16); break;
        }
        ui_printf("GPIO%02d: %s %s",
            hw_init_data[i].pin,
            state,
            hw_init_data[i].state == OUTPUT ? 
                hw_init_data[i].value ? "ON" : "OFF" : ""
        );
        pinMode(hw_init_data[i].pin,hw_init_data[i].state);
        if(hw_init_data[i].state == OUTPUT)
            digitalWrite(hw_init_data[i].pin, hw_init_data[i].value);
    }
}
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Initialising.");
    Serial.println("Initialising UI.");
    ui_init();
    hw_init();
    ui_println("Init WiFi");
    WiFi.begin();
    ui_println("Get MAC ADDR");
    ui_println(WiFi.macAddress().c_str());
    ui_println("Init LORA #1");
    LoRa.setPins(LORA_SS,LORA_RST,LORA_DI0);
    ui_println("Init LORA #2");
    if(!LoRa.begin(LORA_FREQ))
    {
        ui_fault("LoRa Init Failed."); // this never returns.
    }

    ui_println("Set led(25) to off.");
    digitalWrite(25,0);
    ui_println("Initialised.");

    ui_println("Ready>");
    delay(1000);
    if(!digitalRead(0))
    {
        ui_println("Running Diagnostics.");

        while(true)
        {
            ui_diagnostics();
        }
    }
    for(int i=0;i<5;i++)
        ui_println();

    WiFi.macAddress(ident);
}

void loop() {

    // put your main code here, to run repeatedly:
    //Serial.printf("digtalRead(0) = %d\n",digitalRead(0));
   
    if(!digitalRead(0))
    {
        digitalWrite(25,HIGH);
        Serial.println("TRANSMITTING.");
        memset(last_tx,0,9);
        snprintf(last_tx,8,"%08x",millis());
        LoRa.beginPacket();
        LoRa.printf("%02x%02x%02x %s",
            ident[3],ident[4],ident[5],
            last_tx);
        LoRa.endPacket();
        Serial.printf("Transmitted [%02x%02x%02x %s]\n",
                        ident[3],ident[4],ident[5],
                        last_tx);
        delay(500);
        digitalWrite(25,LOW);
    }
    int sz = LoRa.parsePacket();
    if(sz)
    {
        digitalWrite(25,HIGH);
        last_rssi = LoRa.packetRssi();
        last_snr = LoRa.packetSnr();
        Serial.printf("RSSI: %d",last_rssi);
        Serial.printf("SNR.: %f",last_snr);
        memset(last_rx,0,21);
        LoRa.readBytes(last_rx,20);
        Serial.println(last_rx);
        Serial.println();
        digitalWrite(25,LOW);
    }
    snprintf(ui_display_data.cells[0],21,"< TX  ADDR:%02x%02x%02x",
                ident[3],ident[4],ident[5]); //      01234567
    snprintf(ui_display_data.cells[1],21,"Last TX..: %s",last_tx);
    snprintf(ui_display_data.cells[2],21,"Last RX..:         ");
    snprintf(ui_display_data.cells[3],21,"%s",last_rx);
    snprintf(ui_display_data.cells[4],21,"RSSI:%d SNR:%0.3f",last_rssi,last_snr);
    //....................................01234567890123456789

    ui_display();
}