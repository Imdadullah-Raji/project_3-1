#include <Arduino.h>  

#include "LoRaReceiver.h"
#include "gps_module.h"
#include "compass_module.h"
#include "motor_control.h"
#include "gps_manip.h"

//global variables for intertask communications
volatile float targetLat;
volatile float targetLon;
volatile float myLat;
volatile float myLon;
volatile float myHeading;
volatile double targetHeading;

// Optional: handles to control the tasks later (e.g. suspend, delete)
TaskHandle_t ReceiveLoRaHandle = NULL;
TaskHandle_t ReadGPSHandle= NULL;
TaskHandle_t ReadCompassHandle= NULL;
TaskHandle_t ReachDestinationHandle= NULL;

//Global variables for inter-task comms


void ReadGPS( void *pvParameters){
  while(true){
  
    updateGPS();
    
    myLat= getGPSLat();
    myLon= getGPSLon();
    int sat_count= getSatelliteCount();

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
    // Listen Until sth is received
    while (1) {
    if (loopLoRaReceiver())
      break;

    }
    targetLat= getGPSLat();
    targetLon= getGPSLon();

    Serial.println("==Target==");
    Serial.print("targetLat: ");
    Serial.print(targetLat,6);
    Serial.print("  targetLon: ");
    Serial.println(targetLon,6);

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay without blocking CPU
  }
}
void ReadCompass(void *pvParameters){
  while (true) {
    float heading= readHeadingDegrees();
    myHeading = heading; //save in global variable

    Serial.println("==Compass Data==");
    Serial.print("Compass Heading: ");
    Serial.println(myHeading);
    
    vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay without blocking CPU
  }
}
void ReachDestination(void *pvParameters){
  while(true){

    targetHeading = getTargetHeadingAngle(targetLat, targetLon, myLat, myLon);
    float heading= readHeadingDegrees();
    while(!(myHeading >targetHeading-10 && myHeading<targetHeading +10 )){
      rotateRight(200);
      // delay(225);
      // stopMotors();
      // delay(300);
      myHeading = readHeadingDegrees();
    
    }
    stopMotors();
    delay(200);
    moveForward(200);
    delay(1500);
    stopMotors();
    Serial.println(targetHeading);
    Serial.println();
    Serial.println();

    vTaskDelay(1000/ portTICK_PERIOD_MS);
  }
}
void setup() {
  Serial.begin(115200);
  delay(1000);  // Let serial settle

  initGPS();
  initCompass();
  setupLoRaReceiver();
  initMotors();

  //Create task: func, name, stack size, params, priority, handle
  xTaskCreate(
    ReceiveLoRa,         // Task function
    "ReceiveLoRa",       // Name for debugging
    2048,           // Stack size in words
    NULL,           // Parameter
    1,              // Priority
    &ReceiveLoRaHandle  // Handle (optional)
  );
  xTaskCreate(
    ReadGPS,         
    "ReadGPSHandle",      
    2048,           
    NULL,           
    2,              
    &ReadGPSHandle  
  );
  xTaskCreate(
    ReadCompass,         
    "ReadCompassHandle",      
    2048,           
    NULL,           
    2,              
    &ReadCompassHandle 
  );
  xTaskCreate(
    ReachDestination,
    "ReachDestinationHandle",
    2048,
    NULL,
    1,
    &ReachDestinationHandle
  );
}

void loop() {
  
}

