#ifndef SD_Data_Read_h
#define SD_Data_Read_h

//headers added to pull structs 
#include <SD.h>
#include "BNO_Data_Collect.h"
#include "BMP_Data_Collect.h"
#include "ASPD_Data_Collect.h"

struct MasterLogEntry {
//master log has to match the format used in data write. this is used to parse all the bytes
  uint32_t timestamp_ms;  //milliseconds
  //qVectors imu_data;  //imu vectors
  //BaroPressure baro_data; //pressure sensor data
  airSpeed airspeed_data; //airspeed data
};

class SD_Data_Read {
 public:
  SD_Data_Read();

  bool initSD();
  void listFiles();
  bool openLogFile(const char* filename);
  bool readNextEntry(MasterLogEntry &data);
  void closeLogFile();
  bool deleteFile(const char* filename);

 private:
  File myFile;
};

#endif

