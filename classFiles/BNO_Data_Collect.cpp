#include "BNO_Data_Collect.h"
#include <Wire.h>


BNO_Data_Collect::BNO_Data_Collect(){
//class constructor
}

bool BNO_Data_Collect::initBNO(){
//function to init bno returns true or false if able to init or not to allow for error handling
  if (!bno085.begin_I2C()) return false;
  setBNOReports();
  return true;
}
bool BNO_Data_Collect::getVectors(qVectors &data){

//function to obtain vector data. returns false if encounters error. otherwise function will return true
  if(bno085.getSensorEvent(&sensorOutputData)){ //if we have completed sensor data
    if(sensorOutputData.sensorId==SH2_GAME_ROTATION_VECTOR){  //if the sensor data id matches desired game vector label
      data.i=sensorOutputData.un.gameRotationVector.i;  //i value gets stored
      data.j=sensorOutputData.un.gameRotationVector.j;  //j value gets stored
      data.k=sensorOutputData.un.gameRotationVector.k;  //k value gets stored
      data.real=sensorOutputData.un.gameRotationVector.real;  // true value gets stored
      return true;  //returns true when compelte
    }
  }
  return false; //returns false if error or data not ready
}
void BNO_Data_Collect::setBNOReports(void) {
  if (!bno085.enableReport(SH2_GAME_ROTATION_VECTOR, 50000)) { // 20Hz
    // Report could not be enabled
  }
}