#include "SD_Data_Read.h"
#include <SPI.h>
#include <SD.h>

SD_Data_Read::SD_Data_Read() {
// class constructor
}

bool SD_Data_Read::initSD() {
  pinMode(10, OUTPUT);  //sets pin 10 as the output pin to identify the sd card via spi
  delay(5); //5ms delay
  if (!SD.begin(10)) return false;  //returns false if the sd card cannot be initialized
  return true;
}

void SD_Data_Read::listFiles() {
// Lists all files in the root directory of the SD card.
  Serial.println("\nFiles found on SD card:");
  File root = SD.open("/"); //open root directory
  while (true) {
    File entry = root.openNextFile(); //sets entry equal to the name of the next file
    if (!entry) break; // no more files. exits loop if entry does not exist
    if (!entry.isDirectory()) { //lists directory name if it exists
      Serial.print(" - "); 
      Serial.println(entry.name());
    }
    entry.close();  //closes file
  }
  root.close(); //closes root directory
}
void SD_Data_Read::closeLogFile() {
// Closes the currently open file.
  if (myFile) myFile.close();
}

bool SD_Data_Read::openLogFile(const char* filename) {
// Opens a specific log file for reading.
  myFile = SD.open(filename, FILE_READ); //opens the file with read privaleges
  return myFile; // Returns true if the file was opened successfully
}
bool SD_Data_Read::readNextEntry(MasterLogEntry &data) {
// Reads the next binary MasterLogEntry struct from the opened file.
  if (!myFile) return false; // No file is open
  // read() returns the number of bytes read. If it matches the size of our struct,
  return (myFile.read((uint8_t *)&data, sizeof(data)) == sizeof(data));  //returns the number of bytes read. If it matches the size of our struct, returns true
}
bool SD_Data_Read::deleteFile(const char* filename) {
// Deletes file from the SD card.
  Serial.print("Deleting file: ");
  Serial.print(filename);
  
  if (SD.remove(filename)) return true;
  else return false;
}