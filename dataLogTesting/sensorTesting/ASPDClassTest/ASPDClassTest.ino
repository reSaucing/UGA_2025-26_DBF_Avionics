#include "ASPD_Data_Collect.h"
#include <Wire.h>
#include <SD.h>

ASPD_Data_Collect myASPD;

const int ASPD_ADDRESS = 0x28;
const int chipSelect = 10;

File myFile;

/**
 * Helper function to delete old log file.
 */
void deleteBinFiles(File dir) {
  while (File entry = dir.openNextFile()) {
    String entryName = entry.name();
    if (entryName.endsWith(".bin")) {
      entry.close();
        if (SD.remove(entryName.c_str())) {
          Serial.print("Removed: ");
          Serial.println(entryName);
        } else {
          Serial.print("Failed to remove: ");
          Serial.println(entryName);
        }
    }
  }
}

void setup() {

  Serial.begin(115200);
  while(!Serial);

  Serial.print("\nASPD test");
  Wire.begin();

  if(!myASPD.initASPD()){
    Serial.println("Failed to init ASPD");
    while(1);
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card init failed");
    while (1);
  }

  // clear old logs
  File root = SD.open("/");
  deleteBinFiles(root);
  root.close();

  // create new log file
  myFile = SD.open("airspeedSensorLog.bin", FILE_WRITE);

  Serial.println("ASPD init successfully.");
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

    myFile.print(currentAirspeed);
    myFile.flush();
    
  }
  delay(1000);
}
