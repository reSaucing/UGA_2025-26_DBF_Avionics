#ifndef ASPD_Data_Collect_h
#define ASPD_Data_Collect_h
#include <stdint.h>

struct airSpeed{int16_t air;};

class ASPD_Data_Collect{
  public:
    ASPD_Data_Collect();

    bool initASPD();
    bool getAirspeed(airSpeed &data);
    float convToPa(int16_t airSpeed);

  private:
    float Pmax=1;
    float Pmin=-1;
    float a=.8;
    float b=.1;
    int16_t pDigiCount=16383;

};
#endif