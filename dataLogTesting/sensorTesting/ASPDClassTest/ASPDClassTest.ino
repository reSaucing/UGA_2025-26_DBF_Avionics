#include "ASPD_Data_Collect.h"
#include <Wire.h>
#include <SD.h>

ASPD_Data_Collect myASPD;

const int ASPD_ADDRESS = 0x28;
const int chipSelect = 10;

char fileName[]="aspdlog.bin";

struct logEntry{
  uint32_t timestamp_ms;
  airSpeed airsSpeedLog;
};

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

}

void loop() {

  logEntry currentLog;

  if(myASPD.getAirspeed(currentLog.airsSpeedLog)){

    // print sensor readings to serial monitor
    currentLog.timestamp_ms = millis();

    float airspeed=(float)currentLog.airsSpeedLog.air;
    Serial.print("Timestamp: ");
    Serial.print(millis());
    Serial.print(" ms, Airspeed: ");
    Serial.println(airspeed);

    //Serial.println("Trying to open file.");
    File myFile=SD.open(fileName, FILE_WRITE);
    if (myFile) {
      //Serial.println("Opened file.");
      myFile.write((const uint8_t *)&currentLog, sizeof(currentLog));
      myFile.close();
      //Serial.println("Wrote data.");
    }
  } else {
    Serial.println("Error getting airspeed reading");
  }
  delay(1000);
}