#ifndef SD_Data_Read_h
#define SD_Data_Read_h

#include <SD.h>
// We must include the headers for your data classes to define the MasterLogEntry struct
#include "BNO_Data_Collect.h"
#include "BMP_Data_Collect.h"
#include "ASPD_Data_Collect.h"

// This is the master data structure that the datalogger writes to the file.
// It must match exactly.
struct MasterLogEntry {
  uint32_t timestamp_ms;
  qVectors imu_data;
  BaroPressure baro_data;
  airSpeed airspeed_data;
};

class SD_Data_Read {
 public:
  // Constructor
  SD_Data_Read();

  // Public Methods
  bool initSD();
  void listFiles();
  bool openLogFile(const char* filename);
  bool readNextEntry(MasterLogEntry &data);
  void closeLogFile();
  bool deleteFile(const char* filename);

 private:
  // The File object used for reading
  File _myFile;
};

#endif

