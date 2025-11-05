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

  Serial.println("\nASPD airspeed test");
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

  Serial.println("ASPD init successfully.\n");

}

void loop() {

  logEntry currentLog;

  if(myASPD.getAirspeed(currentLog.airsSpeedLog)){

    // print sensor readings to serial monitor
    currentLog.timestamp_ms = millis();

    currentLog.airsSpeedLog.air+=43;
    float airspeed=(float)currentLog.airsSpeedLog.air;
    float airspeedPa= ((airspeed-(.1*16383))*((2+2)/(.8*16383))-2)*6895.0;

    float airspeedMS=sqrt(2*airspeedPa/1.22);

    Serial.print("Timestamp: ");
    Serial.print(millis());
    Serial.println(" ms");
    Serial.print("Airspeed Differential: ");
    Serial.print(airspeedPa);//assumes air density if 1.22kg/m3
    Serial.println(" Pa\n");
    Serial.print(airspeedMS);
    Serial.println(" m/s\n");

    //Serial.println("Trying to open file.");
    File myFile=SD.open(fileName, FILE_WRITE);
    if (myFile) {
      myFile.write((const uint8_t *)&currentLog, sizeof(currentLog));
      myFile.close();
    }
  } else {
    Serial.println("Error getting airspeed reading");
  }
  delay(1000);
}