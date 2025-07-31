#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "motor_control.h"       // Your motor functions
#include "read_compass.h"      // Your compass functions
#include "heading_data.h"        // Shared struct
#include "esp_now_receive.h"     // ESP-NOW receiver

// === CONFIG ===
const unsigned long FORWARD_TIME = 7000; // Same as Bot A
const float ANGLE_TOLERANCE = 4.0;       // ±9° heading tolerance

// Shared with esp_now_receive.h
bool newHeadingAvailable = false;
float targetHeading = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);

   setupCompass();
  initMotors();
  initEspNowReceive();

  Serial.println("Follower bot (Bot B) ready.");
}

void loop() {


  // Wait until new heading is received from Bot A
  if (newHeadingAvailable) {
    if (tryAlignAndMove(targetHeading)) {
      Serial.println("Completed one follow step.");
    } else {
      Serial.println("Alignment failed. Skipping move.");
    }

    newHeadingAvailable = false; // Reset flag
  }

  delay(10);
 }

// === ALIGN TO TARGET ANGLE AND MOVE FORWARD ===
bool tryAlignAndMove(float heading) {
  Serial.print("Trying to align to ");
  Serial.println(heading);

  bool aligned = alignToHeading(heading);

  if (aligned) {
    Serial.println("Alignment successful.");
    driveForwardFor(FORWARD_TIME);
    return true;
  } else {
    Serial.println("Alignment failed.");
    return false;
  }
}

// === ALIGN TO TARGET ANGLE ===
bool alignToHeading(float targetHeading) {
  unsigned long startTime = millis();
  const unsigned long TIMEOUT = 15000;

  while (millis() - startTime < TIMEOUT) {
    float myHeading = readCompassHeading();;

    Serial.print("Current Heading: ");
    Serial.println(myHeading);

    if (myHeading > targetHeading - ANGLE_TOLERANCE &&
        myHeading < targetHeading + ANGLE_TOLERANCE) {
      stopMotors();
      delay(500);
      return true;
    }

    rotateRight(170);  // Use same motor logic as leader
    delay(100);
  }

  stopMotors();
  return false;
}

// === FORWARD MOVEMENT ===
void driveForwardFor(unsigned long duration) {
  Serial.println("Moving forward...");
  moveForward(220);
  delay(duration);
  stopMotors();
}

