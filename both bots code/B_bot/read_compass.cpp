#include "read_compass.h"
#include <stdint.h>
#include <QMC5883L.h>
#include <Wire.h>


QMC5883L compass;

void setupCompass() {
  Wire.begin(21, 22);  // SDA, SCL for ESP32
  compass.init();
  compass.setSamplingRate(50);
}

float readCompassHeading() {
  float heading = compass.readHeading();

  if (heading == 0) {
    return -1.0;  // Compass not calibrated or still initializing
  }

  return heading;  // Already in degrees
}

