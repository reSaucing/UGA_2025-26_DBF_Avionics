#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

void listFiles(File dir, int numTabs) {
  dir.rewindDirectory(); // Ensure we start from the top
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    
    for (uint8_t i = 0; i < numTabs; i++) Serial.print('\t');
    
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      listFiles(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      Serial.println(" bytes");
    }
    entry.close();
  }
}

void viewFile() {
  Serial.println("\nEnter filename to view (e.g., DATA001.CSV):");
  
  // Wait for user to type filename
  while (!Serial.available());
  String target = Serial.readStringUntil('\n');
  target.trim(); // Clean up whitespace/newlines

  if (SD.exists(target.c_str())) {
    File dataFile = SD.open(target.c_str());
    if (dataFile) {
      Serial.println("--- START OF FILE ---");
      int count = 0;
      // Read file and print to serial
      while (dataFile.available() && count < 2000) { // Limit to ~2KB for safety
        Serial.write(dataFile.read());
        count++;
      }
      if (dataFile.available()) Serial.println("\n[...File truncated for Serial display...]");
      Serial.println("\n--- END OF FILE ---");
      dataFile.close();
    }
  } else {
    Serial.println("File not found. Check spelling (case sensitive).");
  }
}

void deleteAllFiles(File dir) {
  dir.rewindDirectory();
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;

    String fileName = entry.name();
    entry.close();
    
    if (SD.remove(fileName.c_str())) {
      Serial.print("Deleted: "); Serial.println(fileName);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Serial.println("--- Teensy SD Manager V2 ---");
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    return;
  }

  File root = SD.open("/");
  listFiles(root, 0);
  root.close();

  Serial.println("\n--- ACTIONS ---");
  Serial.println("Type 'l' to LIST files");
  Serial.println("Type 'v' to VIEW file content");
  Serial.println("Type 'd' to DELETE ALL files");
}

void loop() {
  if (Serial.available() > 0) {
    char incoming = Serial.read();

    if (incoming == 'l') {
      File root = SD.open("/");
      Serial.println("\n--- Current Files ---");
      listFiles(root, 0);
      root.close();
    } 
    else if (incoming == 'v') {
      viewFile();
    }
    else if (incoming == 'd') {
      Serial.println("\n!!! DELETING ALL FILES !!!");
      File root = SD.open("/");
      deleteAllFiles(root);
      root.close();
      Serial.println("Wipe complete.");
    }
  }
}