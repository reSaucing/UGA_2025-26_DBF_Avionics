#include "SD_Data_Collect_h"
#include <SPI.h>
#include <SD.h>

File myFile;


SD_Data_Collect::SD_Data_Collect(){
}

void SD_Data_Collect::setCS(int csAssignment){

  csPin=csAssignment;
}

bool SD_Data_Collect::initSD(){

  if (!SD.begin(csPin)) {
    Serial.println("SD card initialization failed");
    while (1);
  }
}
bool SD_Data_Collect::createNewFlightLog(){

  myFile=SD.open("flightLog.bin", FILE_WRITE);
  if(!myFile) return false;
  return true;
}
bool SD_Data_Collect::writeData(mainLogEntry &data)

  if (myFile.println(getAirspeedData()) <= 0) return false;
  myFile.flush();
  return true.
}