#include "ASPD_Data_Collect.h"
#include <Wire.h>

const int ASPD_ADDRESS= 0x28;

ASPD_Data_Collect::ASPD_Data_Collect(){
}

bool ASPD_Data_Collect::initASPD(){

  Wire.beginTransmission(ASPD_ADDRESS);
  if(Wire.endTransmission()==0) return true;
  return false;
}

bool ASPD_Data_Collect::getAirspeed(airSpeed &data){

  if (Wire.requestFrom(ASPD_ADDRESS, 2) == 2) {
    uint8_t byte1 = Wire.read();
    uint8_t byte2 = Wire.read();
      data.air= ((int16_t)(byte1 & 0x3F) << 8) | byte2;
      return true;
  }
  return false;
}