#include "ASPD_Data_Collect.h"
#include <Wire.h>

const int ASPD_ADDRESS= 0x28;

ASPD_Data_Collect::ASPD_Data_Collect(){
//class constructor
}

bool ASPD_Data_Collect::initASPD(){
//function to init airspeed sensor. true if success, false if failed.
  Wire.beginTransmission(ASPD_ADDRESS); //sends ack bit from senesor at address
  if(Wire.endTransmission()==0) return true;  //if acknowledged, return true
  return false; //returns false if not acknowledged
}

bool ASPD_Data_Collect::getAirspeed(airSpeed &data){
//function to get airspeed bytes from sensor. returns true if successful
  if (Wire.requestFrom(ASPD_ADDRESS, 2) == 2) { //requests 2 bytes of data from the sensor and writes them into two temp bytes
    uint8_t byte1 = Wire.read();
    uint8_t byte2 = Wire.read();
    data.air= ((int16_t)(byte1 & 0x3F) << 8) | byte2; //concatinates them together into the 16 bit int and assignes it to struct member
    return true;
  }
  return false; //returns false if unsuccessful
}
float ASPD_Data_Collect::convToPa(int16_t airSpeed){

  float psi=(airSpeed-(b*pDigiCount))*((Pmax-Pmin)/(a*pDigiCount))+Pmin;
  //Serial.print("PSI: ");
  //Serial.println(psi);

  float pa = psi* 0.45359237f * 9.80665f / 0.0254f / 0.0254f;
  //Serial.print("Pa: ");
  //Serial.println(pa);

  float mPerSec=sqrt((2*pa)/1.196);
  //Serial.print("m/s: ");
  //Serial.println(mPerSec);

  //Serial.print("mph: ");
  //Serial.println(mPerSec*2.23694);
  //Serial.println();

  return mPerSec;
}