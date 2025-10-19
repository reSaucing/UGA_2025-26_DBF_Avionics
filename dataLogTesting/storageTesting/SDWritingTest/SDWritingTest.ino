#include "SD_Data_Write.h"
#include <SPI.h>
#include <SD.h>

// Create an instance of your new SD card handler class
SD_Data_Write mySD;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n--- SD Card Class Test ---");

  // Initialize the SD card using your class method
  if (!mySD.initSD()) {
    Serial.println("SD Card initialization failed. Halting.");
    while (1);
  }
  Serial.println("SD Card initialized.");

  // Find an available flight log filename
  if (!mySD.createFlightlog()) {
    Serial.println("Could not create a new flight log file. Halting.");
    while (1);
  }
  Serial.println("Flight log file created/selected.");

  Serial.println("\nStarting to write data...");
}

void loop() {
  // 1. Create a data struct and populate it with some test data
  testData myData;
  // This will make the test value increase over time
  myData.test = (int16_t)(millis() / 100); 

  // 2. Call the writetoSD method to save the data
  if (mySD.writetoSD(myData)) {
    Serial.print("Successfully wrote value: ");
    Serial.println(myData.test);
  } else {
    Serial.println("Error writing to SD card!");
  }

  delay(1000); // Write one data point per second
}