#ifndef SD_Data_Write_h
#define SD_Data_Write_h

struct mainLogEntry{ uint8_t lsb, msb, xlsb;};

class SD_Data_Write{
    public:
        SD_Data_Write();
        bool setCS(int csAssignment)
        bool initSD();
        bool createCurrentFlightLog());
        bool writeData(mainLogEntry &data);

    private:
      int csPin;
};
#endif