#include "ASPD_Data_Collect.h"
#include <Wire.h>

ASPD_Data_Collect myASPD;

void setup() {

  Serial.begin(115200);
  while(!Serial);

  Serial.print("\nASPD test");
  Wire.begin();

  if(!myASPD.initASPD()){
    Serial.println("Failed to init ASPD");
    while(1);
  }

  Serial.println("ASPD init successfully.");
}

void loop() {

  airSpeed currentAirspeed;

  if(myASPD.getAirspeed(currentAirspeed)){

    float airspeed=(float)currentAirspeed.air;
    Serial.print("Timestamp: ");
    Serial.print(millis());
    Serial.print(" ms, Airspeed: ");
    Serial.println(airspeed);
  }
  delay(1000);
}
