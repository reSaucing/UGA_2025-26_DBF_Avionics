#include <Wire.h>
#include <SPI.h>
#include <SD.h>

const int BMP390_ADDRESS=0x77;  //BMP390 i2c address
const int SD_CS_PIN=10; //SD card cs pin

//register addresses for BMP390
const byte PWR_CTRL=0X1B;   //power control
const byte CMD=0x7E;    //command 
const byte OSR=0x1C;    //over sampling
const byte P_REG_BEGIN=0x04;  //start of pressure 3 byte register

//unsigned 8bit values to store raw incoming bits
struct RawPData{uint8_t lsb; uint8_t msb; uint8_t xlsb;};
//data entry struct  to write pressure data with timestamp in milliseconds
struct logEntry{RawPData pressure; uint32_t timestamp_ms;};

void obtainPReading(RawPData &data){    //gets pressure reading
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
        data.lsb = Wire.read();   //least sig byte
        data.msb = Wire.read();   //middle sig byte
        data.xlsb = Wire.read();  //most sig byte
    } else{
        Serial.println("Failed to read pressure data.");
        data={0,0,0};   //resets 3 bytes of mem to 0
    }
}

void setup() {  //initialize device and storage
    Serial.begin(115200);
    Wire.begin();
    delay(100);

    //initialize BMP390
    Wire.beginTransmission(BMP390_ADDRESS);
    Wire.write(PWR_CTRL);
    Wire.write(0b00000001); //enable pressure data readings
    if(Wire.endTransmission()!=0) //returns error if device does not acknowledge
        Serial.println("Failed to initialize BMP390.");
    Wire.beginTransmission(BMP390_ADDRESS);
    Wire.write(OSR);
    Wire.write(0b00000010); //sets pressure oversample resolution to standard 
    Wire.endTransmission();

    //Initialize file system
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("Error initializing SD card");
        while(1);
    }
    Serial.println("File system initiated");
}

void loop() {
    //begins current data entry log
    logEntry current_log;

    //Populates with current sensor data and timestamp
    RawPData current_pressure;
    obtainPReading(current_pressure); // Get the latest reading
    current_log.pressure = current_pressure;
    current_log.timestamp_ms = millis();
    
    //Open the log file in append mode
    File datalog = SD.open("/pressurelog.bin", FILE_WRITE);
    if (!datalog) {
        Serial.println("Failed to open pressurelog.bin for writing");
        return;
    }

    //Write bytes of the struct onto the file and close file
    datalog.write((byte*)&current_log, sizeof(current_log));
    datalog.close();
    delay(1000); // Log one reading per second
}