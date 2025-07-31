#ifndef ESP_NOW_RECEIVE_H
#define ESP_NOW_RECEIVE_H

#include <esp_now.h>
#include <WiFi.h>
#include "heading_data.h"

struct_message incomingData;

void onReceive(const uint8_t *mac, const uint8_t *incomingDataRaw, int len) {
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));
  Serial.print("Received heading: ");
  Serial.println(incomingData.heading);

  // Use incomingData.heading however you like
}

void initEspNowReceive() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  Serial.println("ESP-NOW receiver initialized.");
}

#endif
