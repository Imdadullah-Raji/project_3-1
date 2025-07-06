
#include "LoRaReceiver.h"

double gpsLat=24.5; 
double gpsLon=90.34;


void setupLoRaReceiver() {
  pinMode(LORA_LED_PIN, OUTPUT);
  digitalWrite(LORA_LED_PIN, LOW);

  Serial.begin(115200);
  delay(100);  // Short delay to stabilize power

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    while (1);  // Hang if LoRa fails
  }

  if (Serial) Serial.println("LoRa Receiver Ready");
}

bool loopLoRaReceiver() {
  int packetSize = LoRa.parsePacket();
  int commaIndex;
  if (packetSize) {
    String data = "";
    while (LoRa.available()) {
      data += (char)LoRa.read();
    }

    if (data.length() > 0) {
      digitalWrite(LORA_LED_PIN, HIGH);
      delay(100);
      digitalWrite(LORA_LED_PIN, LOW);

      if (Serial) {
        Serial.print("Received: ");
        Serial.println(data);
        Serial.print("RSSI: ");
        Serial.println(LoRa.packetRssi());

        commaIndex = data.indexOf(',');
        if (commaIndex != -1) {
        String latStr = data.substring(0, commaIndex);
        String lonStr = data.substring(commaIndex + 1);

        gpsLat = latStr.toFloat();
        gpsLon = lonStr.toFloat();
        return true;
        }
      }
      //added    
      commaIndex = data.indexOf(',');
      if (commaIndex != -1) {
        String latStr = data.substring(0, commaIndex);
        String lonStr = data.substring(commaIndex + 1);

        gpsLat = latStr.toFloat();
        gpsLon = lonStr.toFloat();

      }
    }
  }
  return false;
}



double getGPSLat() {
  return gpsLat;
}

double getGPSLon() {
  return gpsLon;
}

