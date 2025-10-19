#include "ASPD_Data_Collect.h"
#include <Wire.h>

// Create an instance of your new class.
ASPD_Data_Collect myASPD;

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("\n--- ASPD-4525 Class Test ---");
  Wire.begin();
  Wire.setClock(100000); // Slower I2C for stability

  // Initialize the sensor using your class method.
  if(!myASPD.initASPD()){
    Serial.println("Failed to initialize ASPD-4525. Halting.");
    while(1);
  }

  Serial.println("ASPD-4525 initialized successfully. Starting measurements...");
}

void loop() {
  // Create a struct to hold the sensor data.
  airSpeed currentAirspeed;

  // Call your getAirspeed method. It will return true on success
  // and fill the 'currentAirspeed' struct with the raw data.
  if(myASPD.getAirspeed(currentAirspeed)){
    
    // Get the timestamp right after a successful reading.
    uint32_t now = millis();
    
    // Access the '.air' member of the struct to get the raw value.
    int16_t airspeed_raw = currentAirspeed.air;
    
    Serial.print("Timestamp: ");
    Serial.print(now);
    Serial.print(" ms, Airspeed (Raw): ");
    Serial.println(airspeed_raw);
  } else {
    Serial.println("Failed to read from airspeed sensor.");
  }

  delay(1000); // Sample once per second.
}

