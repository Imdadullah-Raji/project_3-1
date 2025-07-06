#include "gps_module.h"
#include <HardwareSerial.h>

TinyGPSPlus gps;

//HardwareSerial gpsSerial(2);  // UART2 on GPIO16 (RX), GPIO17 (TX)

void initGPS() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  
}

void updateGPS() {
  while (Serial2.available()) {
    gps.encode(Serial2.read());
  }
}

double getLatitude() {
  if (gps.location.isValid()) {
    //Serial2.println(gps.location.lat());
    return gps.location.lat();
  } else {
    return NAN;
  }
}

double getLongitude() {
  if (gps.location.isValid()) {
    return gps.location.lng();
  } else {
    return NAN;
  }
}

int getSatelliteCount() {
  return gps.satellites.value();
}
