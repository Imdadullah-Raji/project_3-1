#include "compass_module.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// Calibration data
float faulty_angles[] = {
    282,275,265.5,259.5,250,240.5,235,230,223,215,209.5,195.5,191.5,
    181,175,160,147.5,134.5,122,105.75,90,72,55,41,28,15,6.5,0,352,
    342,334,326,319, 308,297.5,289.5,282
};

float true_angles[] = {
    0,10,20,30,40,50,60,70,80,90,100,110,
    120,130,140,150,160,170, 180, 190,
    200, 210, 220, 230, 240, 250, 260, 270,
    280, 290, 300, 310, 320, 330, 340, 350, 360};

void initCompass() {
  Wire.begin(21, 22);
  if (!mag.begin()) {
    Serial.println("No HMC5883 detected... Check wiring!");
    while (1);
  }
}

float readHeadingDegrees() {
  sensors_event_t event;
  mag.getEvent(&event);

  float heading = atan2(event.magnetic.y, event.magnetic.x);
  float declination = 0.22;  // radians
  heading += declination;

  if (heading < 0) heading += 2 * PI;
  if (heading > 2 * PI) heading -= 2 * PI;

  return calibrate_sensor_data(heading * 180 / PI);
}


// Linear interpolation function
float calibrate_sensor_data(float input) {

    
    if(input>352){
        return 270+(input-352)*10/8;
    }
    //Find the nearest two points
    for (int i = 0; i < 36; i++) {
        if (input >= faulty_angles[i+1] && input <= faulty_angles[i]) {
            //printf("%d %.2f, %.2f\n",i, faulty_angles[i], faulty_angles[i+1]);
            float x0 = faulty_angles[i];
            float x1 = faulty_angles[i + 1];
            float y0 = true_angles[i];
            float y1 = true_angles[i + 1];
            
            // Linear interpolation formula
            return y0 + (input - x0) * (y1 - y0) / (x1 - x0);
        }
    }

    // Should not reach here
    return -1;
}
