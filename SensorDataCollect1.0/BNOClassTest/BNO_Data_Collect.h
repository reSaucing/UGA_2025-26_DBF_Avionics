#ifndef BNO_Data_Collect_h
#define BNO_Data_Collect_h

#include <Adafruit_BNO08x.h>

struct qVectors{ int16_t i, j, k, real;};

class BNO_Data_Collect{
  public:
    BNO_Data_Collect();

    bool initBNO();
    bool getVectors(qVectors &data);
  private:
    Adafruit_BNO08x bno085;
    sh2_SensorValue_t sensorOutputData;

    void setReports();
};
#endif