#include "SD_Data_Read.h"
#include <SPI.h>
#include <SD.h>

// Create an instance of your new SD card reader class
SD_Data_Read myReader;

// Specify the name of the file you want to read.
// Make sure this matches a file that was created by your writer sketch.
const char* FILENAME_TO_READ = "airspeedSensorLog.txt";//"flight000.bin";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n--- SD Card Reader Test ---");

  // Initialize the SD card using your class method
  if (!myReader.initSD()) {
    Serial.println("SD Card initialization failed. Halting.");
    while (1);
  }
  Serial.println("SD Card initialized.");

  // List all available files to help with debugging
  myReader.listFiles();

  // Attempt to open the specified log file
  Serial.print("\nAttempting to open ");
  Serial.print(FILENAME_TO_READ);
  Serial.println("...");
  
  if (!myReader.openLogFile(FILENAME_TO_READ)) {
    Serial.println("Failed to open log file. Does it exist? Halting.");
    while (1);
  }
  Serial.println("File opened successfully. Reading contents...");
}

void loop() {
  // 1. Create a data struct to hold the incoming data
  testData myData;

  // 2. Call the readNextEntry method. It will return 'true' as long as there's data.
  if (myReader.readNextEntry(myData)) {
    // 3. If data was read, print it.
    Serial.print("Read value: ");
    Serial.println(myData.test);
  } else {
    // 4. If readNextEntry returns 'false', we've reached the end of the file.
    Serial.println("\n--- End of File ---");
    myReader.closeLogFile();
    while (1); // Halt the program, the test is complete.
  }
  myReader.deleteFile(FILENAME_TO_READ);

  delay(100); // Read entries at a controlled pace
}
