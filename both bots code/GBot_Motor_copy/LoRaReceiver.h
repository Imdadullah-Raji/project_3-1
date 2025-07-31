#ifndef LORA_RECEIVER_H
#define LORA_RECEIVER_H

#include <SPI.h>
#include <LoRa.h>

// LoRa pin definitions for ESP32
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   4
#define LORA_DIO0  15

#define LORA_LED_PIN 2  // Built-in LED for feedback

void setupLoRaReceiver();
bool loopLoRaReceiver();
double getGPSLat();
double getGPSLon();

#endif