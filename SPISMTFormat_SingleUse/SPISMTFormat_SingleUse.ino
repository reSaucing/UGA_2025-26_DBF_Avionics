#include <SPI.h>
#include <Adafruit_SPIFlash.h>
#include "SdFat.h"

// Pin for the Flash Chip Select
const int FLASH_CS_PIN = 10;

// --- OBJECTS FOR SPI FLASH AND SDFAT ---
Adafruit_FlashTransport_SPI flashTransport(FLASH_CS_PIN, SPI);
Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem sd;
File file;

void setup() {
  Serial.begin(115200);
  // Wait up to 5 seconds for the Serial Monitor to open
  for(int i=0; i<50 && !Serial; ++i) delay(100);

  Serial.println("\n--- Adafruit SPI Flash Data Reader ---");

  // Initialize the file system
  if (!sd.begin(&flash)) {
    Serial.println("Error initializing file system with SdFat.");
    while (1);
  }

  // List all the files on the flash chip
  Serial.println("Files found on the flash chip:");
  sd.ls(LS_R | LS_DATE | LS_SIZE);
  
  Serial.println("\nType the full name of the file you want to download (e.g., flight001.bin) and press Enter.");
}

void loop() {
  // Check if the user has typed a filename into the Serial Monitor
  if (Serial.available()) {
    // Read the filename from the Serial Monitor
    String filename = Serial.readStringUntil('\n');
    filename.trim(); // Remove any extra whitespace

    if (filename.length() > 0) {
      Serial.print("\nAttempting to read file: ");
      Serial.println(filename);

      // Try to open the requested file in read mode
      file = sd.open(filename.c_str(), FILE_READ);
      if (!file) {
        Serial.println("Failed to open file. Please check the name and try again.");
        return;
      }

      Serial.println("File opened successfully. Streaming data...");
      
      // Send a start marker so the computer knows when the data begins.
      // A computer-side script can wait for this line before it starts saving bytes.
      Serial.println(">>>START_DATA<<<");

      // Read the file byte-by-byte and write its raw binary content to the Serial port
      uint8_t buffer[64];
      int bytesRead;
      while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
        Serial.write(buffer, bytesRead);
      }
      
      file.close();

      // Send an end marker
      Serial.println("\n>>>END_DATA<<<");
      Serial.println("--- File transfer complete. ---");
      Serial.println("\nReady for another filename.");
    }
  }
}

