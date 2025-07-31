#ifndef ESP_NOW_RECEIVE_H
#define ESP_NOW_RECEIVE_H

#include <esp_now.h>
#include <WiFi.h>
#include "heading_data.h"

// Declare shared flags/variables
extern bool newHeadingAvailable;
extern float targetHeading;

// Updated callback to match ESP-IDF 5.x / Arduino core v3.x
void onReceive(const esp_now_recv_info_t *recv_info, const uint8_t *incomingDataRaw, int len) {
  struct_message incomingData;
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));

  targetHeading = incomingData.heading;
  newHeadingAvailable = true;

  Serial.print("Received heading via ESP-NOW: ");
  Serial.println(targetHeading);
}

void initEspNowReceive() {
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);  // ✅ Now matches new callback signature

  Serial.println("ESP-NOW receiver initialized.");
}

#endif

