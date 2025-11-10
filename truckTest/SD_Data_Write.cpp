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