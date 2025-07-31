#include <ESP32Servo.h>
#include <SPI.h>
#include <LoRa.h>

// UART2 pins for ESP32 (connected to RPi)
#define RXD2 16
#define TXD2 17

// LoRa module pins
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   4
#define LORA_DIO0  12

// Servo pins
const int servoXPin = 32;
const int servoYPin = 33;

// Servo objects
Servo servoX;  // Pan
Servo servoY;  // Tilt

// Track current servo angles
int currentPan = 90;
int currentTilt = 90;

void setup() {
  Serial.begin(115200);                             // Serial monitor
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);    // UART2 for RPi

  // Set up servos
  servoX.setPeriodHertz(50);
  servoY.setPeriodHertz(50);
  servoX.attach(servoXPin, 500, 2400);
  servoY.attach(servoYPin, 500, 2400);
  servoX.write(currentPan);
  servoY.write(currentTilt);

  // Set up LoRa
  Serial.println("Initializing LoRa...");
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1);
  }
  Serial.println("LoRa init success");
  Serial.println("System ready");
}

void loop() {
  static char buffer[100];

  if (Serial2.available()) {
    size_t len = Serial2.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';
    String data = String(buffer);
    data.trim();

    if (data.length() > 0) {
      Serial.print("Received from RPi: ");
      Serial.println(data);

      // Try to parse pan and tilt
      int commaIndex = data.indexOf(',');
      if (commaIndex != -1) {
        String panStr = data.substring(0, commaIndex);
        String tiltStr = data.substring(commaIndex + 1);

        int pan = panStr.toInt();
        int tilt = tiltStr.toInt();

        pan = constrain(pan, 0, 180);
        tilt = constrain(tilt, 0, 180);

        Serial.print("Target Pan: ");
        Serial.print(pan);
        Serial.print(" | Tilt: ");
        Serial.println(tilt);

        smoothMove(servoX, currentPan, pan, 20);
        smoothMove(servoY, currentTilt, tilt, 20);

        currentPan = pan;
        currentTilt = tilt;
      }

      // Forward data over LoRa
      LoRa.beginPacket();
      LoRa.print(data);
      LoRa.endPacket();

      Serial.println("Data sent over LoRa");
    }
  }

  delay(10);  // Light delay to allow buffer time
}

void smoothMove(Servo &servo, int fromAngle, int toAngle, int stepDelay) {
  if (fromAngle == toAngle) return;

  int step = (fromAngle < toAngle) ? 1 : -1;
  for (int pos = fromAngle; pos != toAngle; pos += step) {
    servo.write(pos);
    delay(stepDelay);
  }
  servo.write(toAngle);
}
