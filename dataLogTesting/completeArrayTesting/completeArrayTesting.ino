#include "ASPD_Data_Collect.h"
//#include "BMP_Data_Collect.h"
#include "BNO_Data_Collect.h"
#include "SD_Data_Write.h"

//initializing sensor object with the following names
ASPD_Data_Collect myASPD;
//BMP_Data_Collect myBMP;
BNO_Data_Collect myBNO;

//initializing sd card object with the following name
SD_Data_Write mySD;

//struct POTValues{uint16_t leftPOT, rightPOT;};

//initializing main logging struct with corresponding sensor structs
struct masterLog{
  uint32_t timestamp_ms;
  qVectors imuData;
//  BaroPressure baroData;
  airSpeed airspeedData;
//  POTValues potData;
};

//file name storage string
char fileName[16];

//const int leftPotPin=20;
//const int rightPotPin=21;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Initializing sensor array");
  Wire.begin();

/*------------------------------file creation---------------------------------------*/
  //checks if sd was initialized successfully. if not, stops initialization of system
  if (!mySD.initSD()) {
    Serial.println("HALT: SD Card initialization failed.");
    while (1);
  }

  bool fileCreated = false; //bool indicating creating of file
  //up to 256 consecutive flights
  for (uint8_t i = 0; i < 255; i++) { 
  //create completely new numbered datalog for seperate flight tracking
    sprintf(fileName, "%03u.bin", i); //flightXXX.bin
    if (!SD.exists(fileName)) { //if the file does not exist
      Serial.print("Using new log file: ");
      Serial.println(fileName);
      fileCreated = true;
      break;
    }
  }
/*--------------------------------initializations for debugging------------------------------------*/
  //checks if file was created successfully. if not, stops initialization of system
  if(!fileCreated){
    Serial.println("Process stopped. Not able to create file.");
    while(1);
  }
  //initialization of all sensors. Stopps program when sensor is unable to init
  if(!myBNO.initBNO()){
    Serial.println("Process stopped. Not able to initialize accelerometer/gyroscope sensors.");
    while(1);
  }
/*  if(!myBMP.initBMP()){
    Serial.println("Process stopped. Not able to initialize barometric pressure sensor.");
    while(1);
  }*/
  if(!myASPD.initASPD()){
    Serial.println("Process stopped. Not able to initialize airspeed sensor.");
    while(1);
  }
  
  Serial.println("All sensors initialized. data logging beginning...");
}

void loop() {
  //acts like a buffer to collect temp BNO vector data
  qVectors currentVectors;  
  
  //if getVecotor returns true, populate struct and write data to sdcard
  if(myBNO.getVectors(currentVectors)){
    masterLog currentLog={};  //creates master log instance called current log
    currentLog.timestamp_ms=millis(); //populates timestamp
    currentLog.imuData=currentVectors;  //populates vector data

    //if(!myBMP.getPressure(currentLog.baroData)) currentLog.baroData={};  //populates pressure data. if unable data = 0;
    if(!myASPD.getAirspeed(currentLog.airspeedData)) currentLog.airspeedData={}; //populates airspeed data. if unable data = 0;

    //currentLog.potData.leftPOT=analogRead(leftPotPin);
    //currentLog.potData.rightPOT=analogRead(leftPotPin);

    //writes the contents of the struct to the datalog in 8 bit intervals (1 byte interval)
    File dataFile = SD.open(fileName, FILE_WRITE);  //opens log file
    if (dataFile) { //if data is available
      dataFile.write((const uint8_t *)&currentLog, sizeof(currentLog)); //writing the bytes for the size of the whole struct
      dataFile.close(); // Close the file prevent corruption
    } else {
      // If writing fails, print an error to the serial monitor
      Serial.println("Error writing to SD card.");
    }
  }

  delay(10);  //1 second delay for testing
}
