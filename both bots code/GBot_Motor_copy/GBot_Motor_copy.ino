#include <Arduino.h>  

#include "LoRaReceiver.h"
#include "gps_module.h"
#include "compass_module.h"
#include "motor_control.h"
#include "gps_manip.h"
#include "heading_control.h"
#include "esp_now_send.h"

//global variables for intertask communications
volatile double targetLat=0;
volatile double targetLon=0;
volatile double myLat=0;
volatile double myLon=0;
volatile double myHeading;
volatile double targetHeading;
volatile float lastAlignedHeading = -1;

// synchronization flags
volatile bool gpsReady = false;
volatile bool targetReady = false;

// Optional: handles to control the tasks later (e.g. suspend, delete)
TaskHandle_t ReceiveLoRaHandle = NULL;
TaskHandle_t ReadGPSHandle= NULL;
TaskHandle_t ReadCompassHandle= NULL;
TaskHandle_t ReachDestinationHandle= NULL;

void ReadGPS( void *pvParameters){
  while(true){
    updateGPS();

    myLat= getLatitude();
    myLon= getLongitude();
    int sat_count= getSatelliteCount();

    if (sat_count > 0) {
      gpsReady = true;
    }

    Serial.println("===Bots GPS Position ===");
    Serial.print("GPS lat:");
    Serial.print(getLatitude(),7);
    Serial.print("  GPS lon: ");
    Serial.print(getLongitude(),7 );
    Serial.print("  Satellite Count: ");
    Serial.println(sat_count);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void ReceiveLoRa(void *pvParameters) {
  while (true) {
    while (1) {
      if (loopLoRaReceiver())
        break;
    }

    targetLat= getGPSLat();
    targetLon= getGPSLon();

    targetReady = true;

    Serial.println("==Target==");
    Serial.print("targetLat: ");
    Serial.print(targetLat,7);
    Serial.print("  targetLon: ");
    Serial.println(targetLon,7);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void ReadCompass(void *pvParameters){
  while (true) {
    float heading = readHeadingDegrees();
    myHeading = heading;


    Serial.println();Serial.println();
    Serial.println("==Compass Data==");
    Serial.print("Compass Heading: ");
    Serial.println(myHeading);
    Serial.println(); Serial.println();

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void ReachDestination(void *pvParameters){
  // wait until GPS and LoRa data are both available
  while (!gpsReady || !targetReady) {
    Serial.println("Waiting for GPS and Target data...");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  while(true){
    double lat1 = myLat;
    double lon1 = myLon;
    double lat2 = targetLat;
    double lon2 = targetLon;
    double thresholdDistance = 2.5;

    Serial.println();Serial.println();
    Serial.println("Global Variable Values");
    Serial.print("myLat: "); Serial.println(myLat,6);
    Serial.print("myLon: "); Serial.println(myLon,6);
    Serial.print("targetLat: ");Serial.println(targetLat,7);
    Serial.print("targetLon: ");Serial.println(targetLon,7);
    Serial.print("delLat: ");Serial.println(targetLat-myLat,6);
    Serial.print("delLon: ");Serial.println(targetLon-myLon,6);


    double distance = getDistance(lat1, lon1, lat2, lon2);


    Serial.println();Serial.println();
    Serial.print("Distance in m: ");
    Serial.println(distance, 6);
    Serial.println();Serial.println();

    // if (distance > thresholdDistance) {
    //   targetHeading = getTargetHeadingAngle(myLat, myLon,targetLat, targetLon );

    //   Serial.println();
    //   Serial.println("Target Heading Angle:");
    //   Serial.println(targetHeading);
    //   Serial.println();


    //   while (!(myHeading > targetHeading - 9 && myHeading < targetHeading + 9)) { 
    //     rotateRight(225);
    //     myHeading = readHeadingDegrees();
    //   }

    //   stopMotors();
    //   delay(200);
    //   moveForward(255);
    //   delay(7000);
    //   stopMotors();

    //   Serial.print("Target Heading: ");
    //   Serial.println(targetHeading);
    //   Serial.println();
    // } else {
    //   Serial.println("Reached Target.");  
    // }
     if (distance > thresholdDistance) {
  targetHeading = getTargetHeadingAngle(myLat, myLon, targetLat, targetLon);

  Serial.println();
  Serial.println("Target Heading Angle:");
  Serial.println(targetHeading);
  Serial.println();

  // Align to target heading
  if (alignToHeading(targetHeading)) {
    sendHeading(lastAlignedHeading);  // Send to follower only after successful alignment
    delay(1000);
    moveForward(255);
    delay(7000);
    stopMotors();

    Serial.print("Target Heading: ");
    Serial.println(targetHeading);
    Serial.println();
  } else {
    Serial.println("Alignment failed. Skipping movement.");
  }

} else {
  Serial.println("Reached Target.");
}

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  initGPS();
  initCompass();
  setupLoRaReceiver();
  initEspNowSend();
  initMotors();

  xTaskCreate(ReceiveLoRa, "ReceiveLoRa", 2048, NULL, 2, &ReceiveLoRaHandle);
  xTaskCreate(ReadGPS, "ReadGPSHandle", 2048, NULL, 3, &ReadGPSHandle);
  xTaskCreate(ReadCompass, "ReadCompassHandle", 2048, NULL, 2, &ReadCompassHandle);
  xTaskCreate(ReachDestination, "ReachDestinationHandle", 2048, NULL, 1, &ReachDestinationHandle);
}

void loop() {
}

