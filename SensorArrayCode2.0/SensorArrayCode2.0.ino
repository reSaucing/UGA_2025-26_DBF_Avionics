#include <Wire.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_BNO08x.h>

// =================================================================================
// Pin Definitions & I2C Addresses
// =================================================================================
const int BNO_RST_PIN = 8;
const int BMP_ADDRESS = 0x77;   // Barometric Pressure Sensor
const int ASPD_ADDRESS = 0x28;  // Airspeed Sensor
const byte BMP_DATA_STRT_REG = 0x04;  // Start of pressure data for BMP sensor

// Per the working example, setting RESET to -1 for the library is key for I2C.
#define BNO08X_RESET -1

// =================================================================================
// Sensor Objects
// =================================================================================
Adafruit_BMP3XX bmp;
Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// =================================================================================
// Data Structures
// =================================================================================
struct BaroData{ uint8_t lsb, msb, xlsb;};
struct AirspeedData { int16_t pressure; };
struct ImuData { int16_t i, j, k, real; };

struct MasterLogEntry {
  uint32_t timestamp_ms;
  ImuData imu_reading;
  BaroData baro_reading;
  AirspeedData airspeed_reading;
};

// =================================================================================
// Function Prototypes
// =================================================================================
bool init_BMP();
bool init_BNO();
void setBNOReports();
AirspeedData getAirspeedData();
BaroData getBaroData();

// =================================================================================
// Main Setup Function
// =================================================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial Monitor

  Wire.begin();
  Wire.setClock(100000); // Use a slower I2C speed for stability

  if (!init_BNO()) {
    Serial.println("HALT: BNO init failed.");
    while (1);
  }
  
  if (!init_BMP()) {
    Serial.println("HALT: BMP init failed.");
    while (1);
  }
  
  // Optional check for airspeed sensor, does not halt on failure
  Wire.beginTransmission(ASPD_ADDRESS);
  if (Wire.endTransmission() == 0) {
    Serial.println("Airspeed sensor found on I2C bus.");
  } else {
    Serial.println("WARNING: Airspeed sensor not found.");
  }
  Serial.println("\nAll sensors initialized. Starting data stream...");
}

// =================================================================================
// Main Loop
// =================================================================================
void loop() {
  delay(10);

  if (bno08x.wasReset()) {
    setBNOReports();
  }
  
  if (bno08x.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR) {
      
      MasterLogEntry current_log = {};

      current_log.timestamp_ms = millis();
      current_log.baro_reading = getBaroData();
      current_log.airspeed_reading = getAirspeedData();

      current_log.imu_reading.i    = sensorValue.un.gameRotationVector.i;
      current_log.imu_reading.j    = sensorValue.un.gameRotationVector.j;
      current_log.imu_reading.k    = sensorValue.un.gameRotationVector.k;
      current_log.imu_reading.real = sensorValue.un.gameRotationVector.real;

      // MODIFIED: The binary write command is now commented out.
      // Serial.write((byte*)&current_log, sizeof(current_log));

      // MODIFIED: Added human-readable print statements for debugging.
      long raw_pressure = (long)current_log.baro_reading.xlsb << 16 | (long)current_log.baro_reading.msb << 8 | (long)current_log.baro_reading.lsb;
      
      Serial.println("--------------------");
      Serial.print("Timestamp: "); Serial.println(current_log.timestamp_ms);
      Serial.print("IMU (Raw): i=");
      Serial.print(current_log.imu_reading.i); Serial.print(", j=");
      Serial.print(current_log.imu_reading.j); Serial.print(", k=");
      Serial.print(current_log.imu_reading.k); Serial.print(", real=");
      Serial.println(current_log.imu_reading.real);
      Serial.print("Barometer (Raw): "); Serial.println(raw_pressure);
      Serial.print("Airspeed (Raw): "); Serial.println(current_log.airspeed_reading.pressure);
    }
  }
}

// =================================================================================
// Sensor Helper Functions
// =================================================================================

bool init_BMP() {
  if (!bmp.begin_I2C()) {
    return false;
  }
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  return true;
}

bool init_BNO() {
  if (!bno08x.begin_I2C()) {
    return false;
  }
  setBNOReports();
  return true;
}

void setBNOReports(void) {
  if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR, 50000)) { // 20Hz
    // Report could not be enabled
  }
}

AirspeedData getAirspeedData() {
  if (Wire.requestFrom(ASPD_ADDRESS, 2) == 2) {
    byte byte1 = Wire.read();
    byte byte2 = Wire.read();
    int16_t pressure = ((int16_t)(byte1 & 0x3F) << 8) | byte2;
    return {pressure};
  }
  return {0};
}

BaroData getBaroData() {
  if (!bmp.performReading()) {
    return {0, 0, 0};
  }
  Wire.beginTransmission(BMP_ADDRESS);
  Wire.write(BMP_DATA_STRT_REG);
  if (Wire.endTransmission(false) != 0) {
    return {0, 0, 0};
  }
  if (Wire.requestFrom(BMP_ADDRESS, 3) == 3) {
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    uint8_t xlsb = Wire.read();
    return {lsb, msb, xlsb};
  }
  return {0, 0, 0};
}

