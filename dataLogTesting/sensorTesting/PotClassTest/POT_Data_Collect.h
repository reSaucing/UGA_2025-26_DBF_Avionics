#ifndef POT_Data_Collect_h
#define POT_Data_Collect_h

struct potValues{uint16_t leftPOT, rightPOT;};

class POT_Data_Collect{
  public:
    POT_Data_Collect();

    bool getPOTValues(potValues &data);

  private:
}
#endif