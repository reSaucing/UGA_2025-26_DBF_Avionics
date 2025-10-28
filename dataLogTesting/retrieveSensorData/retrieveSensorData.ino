#include "SD_Data_Read.h"
#include <SPI.h>
#include <SD.h>

// Create an instance of your new SD card reader class
SD_Data_Read myReader;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n--- Avionics Data Retrieval Program ---");

  // Initialize the SD card
  if (!myReader.initSD()) {
    Serial.println("SD Card initialization failed. Halting.");
    while (1);
  }
  Serial.println("SD Card initialized.");

  // List all available files to start
  myReader.listFiles();
  
  Serial.println("\nPlease type the full name of the file you want to read and press Enter.");
}

void loop() {
  // Check if the user has typed a filename into the Serial Monitor
  if (Serial.available()) {
    // Read the filename from the Serial Monitor
    String filename = Serial.readStringUntil('\n');
    filename.trim(); // Remove any extra whitespace/newlines

    if (filename.length() > 0) {
      Serial.print("\nAttempting to read file: ");
      Serial.println(filename);

      // Attempt to open the requested file
      if (myReader.openLogFile(filename.c_str())) {
        Serial.println("File opened successfully. Reading contents...\n");
        
        MasterLogEntry logEntry;
        int entryCount = 0;
        
        // Loop through the file, reading one entry at a time
        while (myReader.readNextEntry(logEntry)) {
          entryCount++;
          Serial.println("--------------------");
          Serial.print("Entry #");
          Serial.println(entryCount);
          
          // Print the data in a human-readable format
          Serial.print("Timestamp: "); Serial.println(logEntry.timestamp_ms);
          
/*        float i = (float)logEntry.imu_data.i;
          float j = (float)logEntry.imu_data.j;
          float k = (float)logEntry.imu_data.k;
          float real = (float)logEntry.imu_data.real;
          Serial.print("IMU (Scaled): i=");
          Serial.print(i, 4); Serial.print(", j=");
          Serial.print(j, 4); Serial.print(", k=");
          Serial.print(k, 4); Serial.print(", real=");
          Serial.println(real, 4);

          Serial.print("Barometer (Raw): "); Serial.println(logEntry.baro_data.pressure);*/
          Serial.print("Airspeed (Raw): "); Serial.println(logEntry.airspeed_data.air);
        }
        
        myReader.closeLogFile();
        Serial.println("\n--- End of File ---");

        // --- Ask the user if they want to delete the file ---
        Serial.print("\nDo you want to delete '");
        Serial.print(filename);
        Serial.print("'? (y/n): ");

        while (Serial.available() == 0) { /* wait for input */ }
        char confirmation = Serial.read();

        if (confirmation == 'y' || confirmation == 'Y') {
          myReader.deleteFile(filename.c_str());
        } else {
          Serial.println("File not deleted.");
        }
        
      } else {
        Serial.print("Failed to open '");
        Serial.print(filename);
        Serial.println("'. Please check the name and try again.");
      }

      // List files again and prompt for the next command
      myReader.listFiles();
      Serial.println("\nPlease type another filename or reset the device.");
    }
  }
}
