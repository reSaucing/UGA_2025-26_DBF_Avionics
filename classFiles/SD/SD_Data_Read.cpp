#include "SD_Data_Read.h"
#include <SPI.h>
#include <SD.h>

SD_Data_Read::SD_Data_Read() {
// class constructor
}

bool SD_Data_Read::initSD() {
//initializes sd card using pin 10 as the chip select pin. False if unable to. True if init successful
  pinMode(10, OUTPUT);  //sets pin 10 as an output
  delay(5); //5ms delay for pinMode to change
  if (!SD.begin(10)) return false;  //if no init, return flase
  return true;  //return true if successful
}
void SD_Data_Read::listFiles() {
//function to list all files on the sd card
  Serial.println("\nFiles found on SD card:");
  File root = SD.open("/"); //open root directory
  while (true) {  //runs infinitly until break
    File entry = root.openNextFile(); //opens files successivley
    if (!entry) break;  //breaks if no file opened
    Serial.print(" - ");
    Serial.println(entry.name()); //prints file name
    entry.close();  //closes file
  }
  root.close(); //closes directory after printing all files
}
bool SD_Data_Read::openLogFile(const char* filename) {
//opens selected file
  myFile = SD.open(filename, FILE_READ);
  if (myFile) return true;
  return false; // Failed to open file
}
bool SD_Data_Read::readNextEntry(testData &data) {
//reads a binary entry inside the file. An entry is determined to be 8 bits long
  if (!myFile) return false;  //if unable to open return false
  int bytesRead = myFile.read((uint8_t *)&data, sizeof(data));  //sets int = to 8bits of data read from file
  if (bytesRead == sizeof(data)) return true; //returns true if found 8 bits
  return false; //returns false if indicated num of bits not found
}
void SD_Data_Read::closeLogFile() {
//closes file read from
  if (myFile) {
    myFile.close();
  }
}
