#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>

/*-----------------------------Sensor pinouts. edit to current gpio assignment--------------------------*/
const int POT1_ADC_PIN;
const int POT2_ADC_PIN;
const int IMU_INT_PIN;
const int IMU_RST_PIN;
const int SDCARD_CS_PIN;

TinyGPSPlus gps;
HardwareSerial& GPS_SERIAL_CONFIG;

/*--------------------------------------I2C setup constants---------------------------------------------*/

//Sensor I2C addresses
const int BNO_ADDRESS= 0x44; //IMU
const int BMP_ADDRESS= 0x77; //barrometric pressure sensor
const int ASPD_ADDRESS= 0x28; //pitot airspeed sensor

//BMP390 module register addresses for setup
const byte BMP_PWR_CTRL_REG= 0x1B;  //power constrol
const byte BMP_CMD_REG= 0x7E; //commands
const byte BMP_OSR_REG= 0x1C; //over sampling
const byte BMP_DATA_STRT_REG= 0x04; //pressure data start

//SHTP Constants for BNO085
const byte SHTP_CTRL_CHANNEL=2; //sensor hub control
const byte SHTP_REPORTS_CHANNEL=3;  //input sensor reports
const byte SHTP_GAME_ROTATION_VECTOR_ID=0x08;  //accelerometer and gyroscope vectoring report identifyer

/*---------------------------------------Log Entry config-----------------------------------------------*/

//individual sensor data structering
struct barometricData{uint8_t lsb, msb, xlsb;}; //3 unsigned bytes storing least, middle, and most sig data packets
struct gameVectoingData{int16_t i, j, k, w;}; //4 signed 2-byte datastruct for i, j, k, and w imu vectoring
struct gpsLocation{int32_t lat, lon, alt;}; //3 long ints to store lat long and altitude

//complete data entry configured in set order
struct completeLogEntry{
  uint32_t timestamp_ms;  //timestamp in milliseconds
  gameVectoringData currentVector;
  barometricData currentBarAlt;
  int16_t currentAirspeed;
  gpsLocation currentPOS;
}
/*------------------------------------------End of Configs----------------------------------------------*/

char fileName[16];  //stores filename for logged data
volatile bool writeReady=false;

void dataReady(){
  writeReady=true;
}

barometricData getBaroData(){
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_CMD_REG);
  Wire.write(0b00010001)  //obtain data in forced mode. basically get data and go to sleep
  Wire.endTransmission();
  delay(10);  //wait for sensor to collect data
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_DATA_STRT_REG);  //I want data stored at this register address
  Wire.endTransmission();
  Wire.requestFrom(BMP_ADDRESS,3);  //requesting sensor to write 3 bytes of data at prevously indicated address
  if(Wire.available()>=3){  //if received all 3 bytes
    return {Wire.read(), Wire.read(),Wire.read()};  // assigns lsb msb and xlsb and exits from function
  }
  return {0,0,0};  //records 0 as reading to indicate error obtaining data when processing data. exits function
}



void setup() {

  //initialize BMP390 and only enable pressure sensing with indicated oversampling resolution
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_PWR_CTRL_REG);
  Wire.write(0b00000001); //enable pressure data readings
  Wire.endTransmission();
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_OSR_REG);
  Wire.write(0b00000010); //sets pressure oversample resolution to standard 
  Wire.endTransmission();
}

void loop() {
  // put your main code here, to run repeatedly:

}
