#include "BMP_Data_Collect.h"

BMP_Data_Collect myBMP;

void setup() {
  
  Serial.begin(115200);
  while(!Serial);

  Serial.print("\nBMP test");
  Wire.begin();

  if(!myBMP.initBMP()){
    Serial.println("Failed to init BMP");
    while(1);
  }

  Serial.println("BMP init successfully.");
}

void loop() {
  
  BaroPressure currentPressure;

  if(myBMP.getPressure(currentPressure)){

    float pressure=(long)currentPressure.xlsb<<16|(long)currentPressure.msb<<8|(long)currentPressure.lsb;

    Serial.print("Timestamp: ");
    Serial.print(millis());
    Serial.print(" ms, Raw Pressure: ");
    Serial.println(pressure);

  } else {
    Serial.println("Failed to get pressure reading.");
  }
  delay(1000);
}
