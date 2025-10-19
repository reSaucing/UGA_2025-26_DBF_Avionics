#ifndef BMP_Data_Collect_h
#define BMP_Data_Collect_h

#include <Adafruit_BMP3XX.h>

struct BaroPressure{ uint8_t lsb, msb, xlsb;};

class BMP_Data_Collect{
    public:
        BMP_Data_Collect();
        BMP_Data_Collect(int freqRate);
        BMP_Data_Collect(int freqRate, int oversampling);

        bool initBMP();
        bool getPressure(BaroPressure &data);

    private:
        int inputSampleRate;
        int inputOversample;

        Adafruit_BMP3XX bmp390;

        uint8_t convertInputSampleRate(int freqRateToConvert);
        uint8_t convertInputOverSample(int oversamplingToConvert);
};
#endif