#ifndef ASPD_Data_Collect_h
#define ASPD_Data_Collect_h
#include <stdint.h>

struct airSpeed{int16_t air;};

class ASPD_Data_Collect{
  public:
    ASPD_Data_Collect();

    bool initASPD();
    bool getAirspeed(airSpeed &data);

  private:
};
#endif