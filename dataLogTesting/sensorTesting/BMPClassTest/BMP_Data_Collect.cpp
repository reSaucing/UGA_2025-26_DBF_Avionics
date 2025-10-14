#include "BMP_Data_Collect.h"
#include <Wire.h>

const int BMP_ADDRESS= 0x77;    //address of bmp on i2c
const int BMP_DATA_REG= 0x04;   //pressure data reg location

/*-----------------class constructors---------------------*/
/* different version of the constructor allow for setting custom sampling rate and oversampling settings if desired.
Sampling rate-- option 1: 1.5Hz, option 2: 3.1Hz, option 3: 25Hz, option 4: 50Hz, option 5: 100Hz, option 6: 200Hz
Oversampling setting-- option 1: 1x, option 2: 2x, option 3: 4x, option 4: 8x, option 5: 16x, option 6: 32x */

//default bmp initialization
BMP_Data_Collect::BMP_Data_Collect(){
    inputSampleRate=4; //50Hz
    inputOversample=3;  //4x
}
//bmp initialized with specific sampling freq
BMP_Data_Collect::BMP_Data_Collect(int freqRate){
    inputSampleRate=freqRate;
    inputOversample=3;
}
//bmp initialized with specified freq rate and oversampling settings.
BMP_Data_Collect::BMP_Data_Collect(int freqRate, int oversampling){
    inputSampleRate=freqRate;
    inputOversample=oversampling;
}

/*-----------------functions for class--------------------*/
/*functions for the class return true and false, rather than values to allow for error handling*/

bool BMP_Data_Collect::initBMP(){
//function to initialize a bmp. returns false if unable to initialize, true if successfull
    if(!bmp390.begin_I2C()) return false;   //begins the sensor via the adafruit library. false if unable to
    
    bmp390.setOutputDataRate(convertInputSampleRate(inputSampleRate));  //sets desired sampling rate
    bmp390.setPressureOversampling(convertInputOverSample(inputOversample));    //sets desired oversampling settings

    return true;
}
bool BMP_Data_Collect::getPressure(BaroPressure &data){
//obtaines barometric pressure reading from sensor. returns true if successful, false if error was encountered while obtaining data
    if(!bmp390.performReading()) return false;  //returns false if sensor does not acknowlege the command to preform a data reading

    Wire.beginTransmission(BMP_ADDRESS);    //start i2c communication to sensor
    Wire.write(BMP_DATA_REG);   //request the sensor to transmit the data from the pressure register
    if(Wire.endTransmission(false)!=0) return false;    //dont stop communication yet. if coms is closed or does not respond, return false to indicate error

    if(Wire.requestFrom(BMP_ADDRESS,3)==3){ // tells the sensor to sent 3 bytes. if we detect 3 bytes, we place them in correct struct component
        data.lsb= Wire.read();  //least sig byte
        data.msb= Wire.read();  //middle byte
        data.xlsb=Wire.read();  //most sig byte
        return true;
    }

    return false;   //if 3 bytes are not received, return false to indicate error
}
uint8_t BMP_Data_Collect::convertInputSampleRate(int freqRateToConvert){
//converts the optional constructor sample rate setting option to a bmp readable output
    switch (freqRateToConvert) {
        case 1: return BMP3_ODR_1_5_HZ; //1.5Hz
        case 2: return BMP3_ODR_3_1_HZ; //3.1Hz
        case 3: return BMP3_ODR_25_HZ;  //25Hz
        case 4: return BMP3_ODR_50_HZ;  //50Hz
        case 5: return BMP3_ODR_100_HZ; //100Hz
        case 6: return BMP3_ODR_200_HZ; //200Hz
        default: return BMP3_ODR_50_HZ; // Default to 50Hz
    }
}
uint8_t BMP_Data_Collect::convertInputOverSample(int oversamplingToConvert){
//converts the optional constructor oversample setting option to a bmp readable output
    switch (oversamplingToConvert) {
        case 1: return BMP3_NO_OVERSAMPLING;    //1x
        case 2: return BMP3_OVERSAMPLING_2X;    //2x
        case 3: return BMP3_OVERSAMPLING_4X;    //3x
        case 4: return BMP3_OVERSAMPLING_8X;    //8x
        case 5: return BMP3_OVERSAMPLING_16X;   //16x
        case 6: return BMP3_OVERSAMPLING_32X;   //32x
        default: return BMP3_OVERSAMPLING_4X; // Default to 4x
    }
}