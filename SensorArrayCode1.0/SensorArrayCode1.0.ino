#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>

/*-----------------------------Sensor pinouts. edit to current gpio assignment--------------------------*/
// CORRECTED: All const variables must be initialized with a value.
const int POT1_ADC_PIN = A0;
const int POT2_ADC_PIN = A1;
const int BNO_INT_PIN = 7;
const int BNO_RST_PIN = 8;
const int SDCARD_CS_PIN = 10;

TinyGPSPlus gps;
// CORRECTED: Initialized the reference to point to a specific hardware serial port.
// On Teensy 4.0, Serial1 uses Pin 0 (RX) and Pin 1 (TX).
HardwareSerial& GPS_SERIAL_CONFIG = Serial1;

/*--------------------------------------I2C setup constants---------------------------------------------*/

//Sensor I2C addresses
const int BNO_ADDRESS = 0x4A;       //IMU // CORRECTED: Default I2C address is 0x4A. Yours was 0x44. Double-check your board.
const int BMP_ADDRESS = 0x77;       //barrometric pressure sensor
const int ASPD_ADDRESS = 0x28;      //pitot airspeed sensor

//BMP390 module register addresses for setup
const byte BMP_PWR_CTRL_REG = 0x1B; //power constrol
const byte BMP_CMD_REG = 0x7E;      //commands
const byte BMP_OSR_REG = 0x1C;      //over sampling
const byte BMP_DATA_STRT_REG = 0x04;  //pressure data start

//SHTP Constants for BNO085
const byte SHTP_CTRL_CHANNEL = 2; //sensor hub control
const byte SHTP_REPORTS_CHANNEL = 3;  //input sensor reports
const byte SHTP_GAME_ROTATION_VECTOR_ID = 0x08;  //accelerometer and gyroscope vectoring report identifyer

/*---------------------------------------Log Entry config-----------------------------------------------*/

//individual sensor data structering
struct barometricData { uint8_t lsb, msb, xlsb; }; //3 unsigned bytes storing least, middle, and most sig data packets
// CORRECTED: The ASPD-4525 returns a single 14-bit value. Storing it as one int16_t is much more efficient.
struct airspeedData { int16_t pressure; };
struct gameVectoringData { int16_t i, j, k, real; }; //4 signed 2-byte datastruct for i, j, k, and w imu vectoring // CORRECTED: Changed 'w' to 'real' for clarity, as it's the standard term.

struct gpsLocation { float lat, lon, alt; };

//complete data entry configured in set order
struct completeLogEntry {
  uint32_t timestamp_ms;  //timestamp in milliseconds
  gameVectoringData currentVector;
  barometricData currentBarAlt;
  airspeedData currentAirspeed;
  gpsLocation currentPOS;
  uint16_t pot1_raw;
  uint16_t pot2_raw;
};
/*------------------------------------------End of Configs----------------------------------------------*/

char fileName[16];  //stores filename for logged data

volatile bool writeReady = false;

void dataReadyISR() {
  writeReady = true;
}

barometricData getBaroData() {
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_CMD_REG);
  Wire.write(0b00010001);  //obtain data in forced mode. basically get data and go to sleep // CORRECTED: Added missing semicolon
  Wire.endTransmission();
  delay(10);  //wait for sensor to collect data
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_DATA_STRT_REG);  //I want data stored at this register address
  Wire.endTransmission();
  Wire.requestFrom(BMP_ADDRESS, 3); //requesting sensor to write 3 bytes of data at prevously indicated address
  if (Wire.available() >= 3) { //if received all 3 bytes
    return {Wire.read(), Wire.read(), Wire.read()}; // assigns lsb msb and xlsb and exits from function
  }
  return {0, 0, 0}; //records 0 as reading to indicate error obtaining data when processing data. exits function
}

airspeedData getAirspeedData() {
  Wire.requestFrom(ASPD_ADDRESS, 2);  //requesting 2 bytes of data from airspeed sensor
  if (Wire.available() >= 2) { //if all bytes are received
    byte byte1 = Wire.read();
    byte byte2 = Wire.read();

    // The sensor returns 2 status bits + 14 data bits. they get parse together and stored under respective struct
    int16_t pressure = ((int16_t)(byte1 & 0x3F) << 8) | byte2;
    return {pressure}; //assigns bytes accordingly
  }
  return {0}; //else return 0 as error code
}

void logSensorData() {
  /*---------------------------------------------create current master log entry------------------------*/
  completeLogEntry currentLog;  //init master entry

  /*again, got stuck on this part for the imu. code from gemini. something along the lines of requesting data.
    sensor sends the header indicating information about the remaining incoming bytes.
    Collects remaining incoming bytes and saves them into temp variables.
    if the header has correct report channel and identifer packet has the right data id, we store into appropriate logging bytes*/
  // --- 1. Get BNO085 Data (from packet) ---
  byte header[4];
  Wire.requestFrom(BNO_ADDRESS, 4);
  header[0] = Wire.read(); header[1] = Wire.read(); header[2] = Wire.read(); header[3] = Wire.read();
  int packetLength = ((uint16_t)header[1] << 8) | header[0];
  if (packetLength == 0) return;

  byte packetData[packetLength];
  packetData[0] = header[0]; packetData[1] = header[1];
  packetData[2] = header[2]; packetData[3] = header[3];
  Wire.requestFrom(BNO_ADDRESS, packetLength - 4);
  for (int i = 4; i < packetLength; i++) packetData[i] = Wire.read();

  if (header[2] == SHTP_REPORTS_CHANNEL && packetData[4] == SHTP_GAME_ROTATION_VECTOR_ID) {
    currentLog.currentVector.i = (int16_t)(((uint16_t)packetData[14] << 8) | packetData[13]);
    currentLog.currentVector.j = (int16_t)(((uint16_t)packetData[16] << 8) | packetData[15]);
    currentLog.currentVector.k = (int16_t)(((uint16_t)packetData[18] << 8) | packetData[17]);
    currentLog.currentVector.real = (int16_t)(((uint16_t)packetData[20] << 8) | packetData[19]);
  }

  //saves current timestamp and data from bmp and airspeed sensor
  currentLog.timestamp_ms = millis();
  currentLog.currentBarAlt = getBaroData();
  currentLog.currentAirspeed = getAirspeedData();
  currentLog.pot1_raw = analogRead(POT1_ADC_PIN);
  currentLog.pot2_raw = analogRead(POT2_ADC_PIN);


  /*obtains gps readings. isValid checks if data we are requesting is complete.
    if it is we log the coordinate.
    otherwise, we return a 0 */
  currentLog.currentPOS.lat = gps.location.isValid() ? gps.location.lat() : 0.0f;
  currentLog.currentPOS.lon = gps.location.isValid() ? gps.location.lng() : 0.0f;
  currentLog.currentPOS.alt = gps.altitude.isValid() ? gps.altitude.meters() : 0.0f;

  /*----------------------------------------------write logged data onto SD card------------------------*/

  File datalog = SD.open(fileName, FILE_WRITE); //open file for the currrent flight log

  //writes data onto storage if open is successful
  if (datalog) {
    datalog.write((byte*)&currentLog, sizeof(currentLog));
    datalog.close();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  GPS_SERIAL_CONFIG.begin(9600);

  //reset bno and wait for initialization
  pinMode(BNO_RST_PIN, OUTPUT);
  digitalWrite(BNO_RST_PIN, LOW); delay(10);
  digitalWrite(BNO_RST_PIN, HIGH); delay(1000);

  //initialize BMP390 and only enable pressure sensing with indicated oversampling resolution
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_PWR_CTRL_REG);
  Wire.write(0b00000001); //enable pressure data readings
  Wire.endTransmission();
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_OSR_REG);
  Wire.write(0b00000010); //sets pressure oversample resolution to standard
  Wire.endTransmission();

  //inti sd card indicators for debugging
  if (!SD.begin(SDCARD_CS_PIN)) {
    Serial.println("SD Card init failed.");
    while (1);
  }
  Serial.println("SD card initialized.");

  //creates a numbered flight log file. checks if it exists. If it does, incrememnts the file number. otherwise we use it
  for (uint8_t i = 0; i < 255; i++) {
    sprintf(fileName, "flight%03u.bin", i); //creates a binary file called datalogX.bin. X being a number
    if (!SD.exists(fileName)) {  //if the filename does not exist, we use this as the current flight log
      Serial.print("Created flight log: ");
      Serial.println(fileName);
      break;
    }
  }
  //initialize BNO085
  /*dont ask me about this part. i got stuck on this and got help.
    Thank Gemini for this part.
    My understanding so far:
    data sent out follows SHTP protocal. data is sent in 21 byte packets.
    first 4 is the header and subsequent includes instructions, data locations, etc.
    https://www.ceva-ip.com/wp-content/uploads/SH-2-Reference-Manual.pdf
    sending out packet configuring sensor settings and what not i think.
    I might look at this later.*/
  long interval_us = 50000;
  byte packet[21];

  packet[0] = 21; packet[1] = 0; packet[2] = SHTP_CTRL_CHANNEL; packet[3] = 0;
  packet[4] = 0xFC; packet[5] = SHTP_GAME_ROTATION_VECTOR_ID;
  for (int i = a6; i < 9; i++) packet[i] = 0;
  packet[9] = interval_us & 0xFF; packet[10] = (interval_us >> 8) & 0xFF;
  packet[11] = (interval_us >> 16) & 0xFF; packet[12] = (interval_us >> 24) & 0xFF;
  for (int i = 13; i < 21; i++) packet[i] = 0;

  //send formatted packet, from above, to the IMU. this part i understood
  Wire.beginTransmission(BNO_ADDRESS);
  Wire.write(packet, 21);
  Wire.endTransmission();

  //begin lack of understanding here again. not super sure what this is for
  pinMode(BNO_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BNO_INT_PIN), dataReadyISR, FALLING);
}

void loop() {
  while (GPS_SERIAL_CONFIG.available() > 0) {
    gps.encode(GPS_SERIAL_CONFIG.read());
  }

  if (writeReady) { //will write log when bno has indicated it is ready to send data
    writeReady = false;
    logSensorData();
  }
}
