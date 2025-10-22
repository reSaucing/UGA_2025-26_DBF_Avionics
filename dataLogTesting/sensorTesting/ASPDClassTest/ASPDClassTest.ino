#include "ASPD_Data_Collect.h"
#include <Wire.h>
#include <SD.h>

ASPD_Data_Collect myASPD;

const int ASPD_ADDRESS = 0x28;
const int chipSelect = 10;

void setup() {

  Serial.begin(9600);
  while(!Serial);

  Serial.println("\nASPD test");
  Wire.begin();

  if(!myASPD.initASPD()){
    Serial.println("Failed to init ASPD");
    while(1);
  }

  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card init failed");
    while (1);
  }

  Serial.println("ASPD init successfully.");

  //test opening file on SD
  File myFile = SD.open(myASPD.fileName, FILE_WRITE);
  if (!myFile) {
    Serial.println("Could not open file. Halting");
    while (1);
  }
  myFile.close();
}

void loop() {

  airSpeed currentAirspeed;

  if(myASPD.getAirspeed(currentAirspeed)){

    // print sensor readings to serial monitor
    float airspeed=(float)currentAirspeed.air;
    Serial.print("Timestamp: ");
    Serial.print(millis());
    Serial.print(" ms, Airspeed: ");
    Serial.println(airspeed);

    // write sensor readings to log file
    currentAirspeed.timestamp_ms = millis();

    myASPD.writetoSD(currentAirspeed);
    
  } else {
    Serial.println("Error getting airspeed reading");
  }
  delay(1000);
}