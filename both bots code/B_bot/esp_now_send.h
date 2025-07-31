#ifndef ESP_NOW_SEND_H
#define ESP_NOW_SEND_H

#include <esp_now.h>
#include <WiFi.h>
#include "heading_data.h"

// Replace this with receiver's MAC address
uint8_t followerMAC[] = {0xF0, 0x24, 0xF9, 0x0E, 0x4A, 0x20};

void onSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("ESP-NOW Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void initEspNowSend() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_send_cb(onSend);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, followerMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  }
}

void sendHeading(float heading) {
  struct_message dataToSend;
  dataToSend.heading = heading;

  esp_now_send(followerMAC, (uint8_t *)&dataToSend, sizeof(dataToSend));
  Serial.print("Sent heading: ");
  Serial.println(heading);
}

#endif
