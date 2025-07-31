#ifndef HEADING_CONTROL_H
#define HEADING_CONTROL_H

#include <Arduino.h>

// You should declare these in your main file or another shared header
extern volatile float lastAlignedHeading;
float readHeadingDegrees(); // Implement this separately
void rotateRight(int speed);
void stopMotors();

bool alignToHeading(float targetHeading) {
  unsigned long startTime = millis();
  const unsigned long TIMEOUT = 15000;

  while (millis() - startTime < TIMEOUT) {
    float myHeading = readHeadingDegrees();
    Serial.print("Current Heading: ");
    Serial.println(myHeading);

    if (myHeading > targetHeading - 9 && myHeading < targetHeading + 9) {
      stopMotors();
      delay(500);
      lastAlignedHeading = myHeading;
      return true;
    }

    rotateRight(235);
    delay(100);
  }

  stopMotors();
  return false;
}

#endif
