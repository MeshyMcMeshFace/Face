// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Reorganised to run under the ESP32 OLED LoRa board by Heltec by Simon Waite.


#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include <SPI.h>

//These are the default for the ESP32 Heltec board
#define LORA_DEFAULT_SS_PIN    (18)
#define LORA_DEFAULT_RESET_PIN (14)
#define LORA_DEFAULT_DIO0_PIN  (26)
//#define LORA_DEFAULT_SS_PIN    10
//#define LORA_DEFAULT_RESET_PIN 9
//#define LORA_DEFAULT_DIO0_PIN  2

// TODO: would this be correct?
#define PA_OUTPUT_RFO_PIN      0
#define PA_OUTPUT_PA_BOOST_PIN 1

//class LoRaClass : public Stream {
//public:
//LoRaClass();
typedef struct {
  SPISettings spiSettings;
  int ss;
  int reset;
  int dio0;
  int frequency;
  int packetIndex;
  int implicitHeaderMode;
  void (*_onReceive)(int);
} LoRa;
//};

  void lora_init();
  int lora_begin(long frequency);
  void lora_end();

  int lora_beginPacket(int implicitHeader = false);
  int lora_endPacket();

  //int parsePacket(int size = 0);
  int lora_parsePacket(int size);
  int lora_packetRssi();
  float lora_packetSnr();

  // from Print
  size_t lora_write(uint8_t byte);
  size_t lora_write(const uint8_t *buffer, size_t size);

  // from Stream
  int lora_available();
  int lora_read();
  int lora_peek();
  void lora_flush();

  void lora_onReceive(void(*callback)(int));

  //void lora_receive(int size = 0);
  void lora_receive(int size);
  void lora_idle();
  void lora_sleep();

  //void lora_setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
  void lora_setTxPower_default(int level);
  void lora_setTxPower(int level, int outputPin);
  void lora_setFrequency(long frequency);
  void lora_setSpreadingFactor(int sf);
  void lora_setSignalBandwidth(long sbw);
  void lora_setCodingRate4(int denominator);
  void lora_setPreambleLength(long length);
  void lora_setSyncWord(int sw);
  void lora_enableCrc();
  void lora_disableCrc();

  byte lora_random();

  //void lora_setPins(int ss = LORA_DEFAULT_SS_PIN, int reset = LORA_DEFAULT_RESET_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);
  void lora_setPins_default();
  void lora_setPins(int ss , int reset , int dio0 );
  void lora_setSPIFrequency(uint32_t frequency);

  void lora_dumpRegisters(Stream& out);

private:
  void lora_explicitHeaderMode();
  void lora_implicitHeaderMode();

  void lora_handleDio0Rise();

  uint8_t lora_readRegister(uint8_t address);
  void    lora_writeRegister(uint8_t address, uint8_t value);
  uint8_t lora_singleTransfer(uint8_t address, uint8_t value);

  void onDio0Rise();

//private:
//extern LoRaClass LoRa;

#endif
