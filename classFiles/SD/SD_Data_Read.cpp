#include "SD_Data_Read.h"
#include <SPI.h>
#include <SD.h>

SD_Data_Read::SD_Data_Read() {
// class constructor
}

bool SD_Data_Read::initSD() {
  pinMode(10, OUTPUT);
  delay(5);
  if (!SD.begin(10)) return false;
  return true;
}
void SD_Data_Read::listFiles() {
  Serial.println("\nFiles found on SD card:");
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    Serial.print(" - ");
    Serial.println(entry.name());
    entry.close();
  }
  root.close();
}
bool SD_Data_Read::openLogFile(const char* filename) {
  myFile = SD.open(filename, FILE_READ);
  if (myFile) return true;
  return false; // Failed to open file
}
bool SD_Data_Read::readNextEntry(testData &data) {
  if (!myFile) return false;
  int bytesRead = myFile.read((uint8_t *)&data, sizeof(data));
  if (bytesRead == sizeof(data)) return true;
  return false;
}
void SD_Data_Read::closeLogFile() {
  if (myFile) {
    myFile.close();
  }
}
