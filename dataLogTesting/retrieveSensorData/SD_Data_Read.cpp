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

// Lists all files in the root directory of the SD card.
void SD_Data_Read::listFiles() {
  Serial.println("\nFiles found on SD card:");
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break; // no more files
    }
    if (!entry.isDirectory()) {
      Serial.print(" - ");
      Serial.println(entry.name());
    }
    entry.close();
  }
  root.close();
}

// Opens a specific log file for reading.
bool SD_Data_Read::openLogFile(const char* filename) {
  _myFile = SD.open(filename, FILE_READ);
  return _myFile; // Returns true if the file was opened successfully
}

// Reads the next binary MasterLogEntry struct from the opened file.
bool SD_Data_Read::readNextEntry(MasterLogEntry &data) {
  if (!_myFile) {
    return false; // No file is open
  }
  // read() returns the number of bytes read. If it matches the size of our struct,
  // it means we successfully read one full entry.
  return (_myFile.read((uint8_t *)&data, sizeof(data)) == sizeof(data));
}

// Closes the currently open file.
void SD_Data_Read::closeLogFile() {
  if (_myFile) {
    _myFile.close();
  }
}

// Deletes a specific file from the SD card.
bool SD_Data_Read::deleteFile(const char* filename) {
  Serial.print("Deleting file: ");
  Serial.print(filename);
  
  if (SD.remove(filename)) {
    Serial.println(" -> SUCCESS");
    return true;
  } else {
    Serial.println(" -> FAILED");
    return false;
  }
}