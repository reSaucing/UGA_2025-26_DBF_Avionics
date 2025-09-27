#include <Wire.h>

const int BMP390_ADDRESS=0x77;  //BMP390 i2c address

//register addresses for BMP390
const byte PWR_CTRL=0X1B;   //power control
const byte CMD=0x7E;    //command 
const byte OSR=0x1C;    //over sampling
const byte P_REG_BEGIN=0x04;  //start of pressure 3 byte register

//unsigned 8bit values to store raw incoming bits
struct RawPData{uint8_t lsb, uint8_t msb, uint8_t xlsb};
RawPData pressureData;

//initializes BMP in forced mode and only enables 
void initBMP390(){
    Wire.beginTransmission(BMP390_ADDRESS);
    Wire.write(PWR_CTRL);
    Wire.write(0b00000001);
    if(Wire.endTransmission()!=0) //returns error if device does not acknowledge
        Serial.println("Failed to initialize BMP390.");
    Wire.beginTransmission(BMP390_ADDRESS);
    Wire.write(OSR);
    Wire.write(0b00000010); //sets pressure oversample resolution to standard 
    Wire.endTransmission();
}
void obtainPReading(){
    Wire.beginTransmission(BMP390_ADDRESS);
    Wire.write(CMD);
    Wire.write(0b00010001);
    Wire.endTransmission();
    delay(10);  //waits 10 msec for data collection

    //sends req to device at address to locate data at the beginning of pressure register
    Wire.beginTransmission(BMP390_ADDRESS);
    Wire.write(P_REG_BEGIN);
    Wire.endTransmission(); //returns acknowlege bit

    Wire.requestFrom(BMP390_ADDRESS, 3);    //Request 3 bytes of data
    //writes 3 incoming bytes of pressure into variables
    if (Wire.available() >= 3) {
        pressureData.lsb = Wire.read();   //least sig byte
        pressureData.msb = Wire.read();   //middle sig byte
        pressureData.xlsb = Wire.read();  //most sig byte
    } else{
        Serial.println("Failed to read pressure data.");
        pressureData={0,0,0};   //resets 3 bytes of mem to 0
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(100);

    //initialize bmp390
    initBMP390();
}

void loop() {
    obtainPReading();

    //prints bits in in order of most sig to least sig left to right.
    Serial.print("Raw Pressure bits: ");
    Serial.print(pressureData.xlsb);
    Serial.print(pressureData.msb);
    Serial.println(pressureData.lsb);

    //waits 2secs before obtaining new data and printing
    delay(2000);
}