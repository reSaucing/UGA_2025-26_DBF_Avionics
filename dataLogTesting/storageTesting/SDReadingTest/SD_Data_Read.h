#ifndef SD_Data_Read_h
#define SD_Data_Read_h

#include <stdint.h>
#include <SD.h>

struct testData {int16_t test;};

class SD_Data_Read {

 public:
  SD_Data_Read();

  bool initSD();
  void listFiles();
  bool openLogFile(const char* filename);
  bool readNextEntry(testData &data);
  void closeLogFile();

 private:
  File myFile;
};

#endif
