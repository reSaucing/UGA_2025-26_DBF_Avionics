#include "SD_Data_Write.h"
#include <SPI.h>
#include <SD.h>

SD_Data_Write::SD_Data_Write(){
//class constructor
}

bool SD_Data_Write::initSD(){
  //SPI.begin();
  pinMode(10, OUTPUT);
  delay(5);
  if (!SD.begin(10)) return false;
  return true;
}
bool SD_Data_Write::createFlightlog(){
  for (uint8_t i = 0; i < 255; i++) {
    sprintf(fileName, "flight%03u.bin", i);  //creates a binary file called datalogX.bin. X being a number
    if (!SD.exists(fileName)) return true;  //if the filename does not exist, we use this as the current flight log
  }
  return false;
}
bool SD_Data_Write::writetoSD(testData &data){
  myFile = SD.open(fileName, FILE_WRITE);
    if (myFile) {
      myFile.write((const uint8_t *)&data, sizeof(data));
      myFile.close();
      return true; // Success
    }
    return false; // Failed to open the file
}