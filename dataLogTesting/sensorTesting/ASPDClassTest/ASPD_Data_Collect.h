#ifndef ASPD_Data_Collect_h
#define ASPD_Data_Collect_h
#include <stdint.h>

struct airSpeed{
  int16_t air;
  uint32_t timestamp_ms;
};

class ASPD_Data_Collect{
  public:
    ASPD_Data_Collect();

    bool initASPD();
    bool getAirspeed(airSpeed &data);
    char fileName[16];
    bool writetoSD(airSpeed &data);
  private:
};
#endif