#include "BNO_Data_Collect.h"

BNO_Data_Collect myBno;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nBNO Test");
  Wire.begin();

  if (!myBno.initBNO()) {
    Serial.println("Failed to init BNO085");
    while (1);
  }

  Serial.println("BNO085 initialized successfully.");
}

void loop() {

  qVectors currentVectors;

  if (myBno.getVectors(currentVectors)) {

    float i=(float)currentVectors.i/16384.0;
    float j=(float)currentVectors.j/16384.0;
    float k=(float)currentVectors.k/16384.0;
    float real=(float)currentVectors.real/16384.0;

    Serial.print("Timestamp: ");
    Serial.print(millis());
    Serial.print(" ms, IMU (Raw): i=");
    Serial.print(i);
    Serial.print(", j=");
    Serial.print(j);
    Serial.print(", k=");
    Serial.print(k);
    Serial.print(", real=");
    Serial.println(real);
  }else Serial.println("Failed to get reading.")
  
  delay(100);
}

