#include "ASPD_Data_Collect.h"
#include "BMP_Data_Collect.h"
#include "BNO_Data_Collect.h"
#include "SD_Data_Write.h"
#include <Wire.h>

ASPD_Data_Collect myASPD;
BMP_Data_Collect myBMP;
BNO_Data_Collect myBNO;

SD_Data_Write mySD;

airspeed currentSpeed;
BaroPressure currentPressure;
qVectors currentVectors;

struct masterLog{
  uint32_t timestamp;
  currentSpeed;
  currentPressure;
  currentVectors;
}

void setup() {

  Wire.begin();
  delay(2);

  mySD.initSD();
  myASPD.initASPD();
  myBMP.initBMP();
  myBNO.initBNO();

}

void loop() {
  // put your main code here, to run repeatedly:

}
