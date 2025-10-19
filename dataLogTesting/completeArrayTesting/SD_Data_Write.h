#ifndef SD_Data_Write_h
#define SD_Data_Write_h

#include <stdint.h>
#include <SD.h>

struct testData{int16_t test;};

class SD_Data_Write{

  public:
    SD_Data_Write();

    bool initSD();
    bool createFlightlog();
    bool writetoSD(testData &data);
  private:
  char fileName[16];
  File myFile;
};
#endif