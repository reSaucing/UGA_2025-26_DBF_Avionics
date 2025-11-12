#ifndef BNO_Data_Collect_h
#define BNO_Data_Collect_h

#include <Adafruit_BNO08x.h>

struct aVectors{ float i, j, k;};

class BNO_Data_Collect{
  public:
    BNO_Data_Collect();

    bool initBNO();
    bool getVectors(aVectors &data);
  private:
    Adafruit_BNO08x bno085;
    sh2_SensorValue_t sensorOutputData;

    void setBNOReports(void);
};
#endif