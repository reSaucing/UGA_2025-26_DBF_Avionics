#include "BNO_Data_Collect.h"

BNO_Data_Collect myBno;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nBNO Test");

  if (!myBno.initBNO()) {
    Serial.println("Failed to init BNO085");
    while (1);
  }

  Wire.begin();

  Serial.println("BNO085 initialized successfully.");
}

void loop() {

  qVectors currentVectors;

  if (myBno.getVectors(currentVectors)) {

    Serial.print("Timestamp: ");
    Serial.print(millis());
    Serial.print(" ms, IMU (Raw): i=");
    Serial.print(currentVectors.i);
    Serial.print(", j=");
    Serial.print(currentVectors.j);
    Serial.print(", k=");
    Serial.print(currentVectors.k);
    Serial.print(", real=");
    Serial.println(currentVectors.real);
  }
  
  delay(10);
}

