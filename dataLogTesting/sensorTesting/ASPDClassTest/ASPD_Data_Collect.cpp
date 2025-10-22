#include "ASPD_Data_Collect.h"
#include <Wire.h>
#include <SD.h>

const int ASPD_ADDRESS= 0x28;

const char filename[] = "aspdlog.bin";

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

bool ASPD_Data_Collect::writetoSD(airSpeed &data){
  File myFile = SD.open(fileName, FILE_WRITE);
    if (myFile) {
      myFile.write((const uint8_t *)&data, sizeof(data));
      myFile.close();
      return true; // Success
    }
    return false; // Failed to open the file
}