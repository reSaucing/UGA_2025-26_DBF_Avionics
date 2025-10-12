#include <Wire.h>
#include <TinyGPS++.h>
#include <Adafruit_BMP3XX.h>

// =================================================================================
// Pin Definitions & Global Objects
// =================================================================================
const int POT1_PIN = A0;
const int POT2_PIN = A1;

TinyGPSPlus gps;
HardwareSerial& GpsSerial = Serial1;
Adafruit_BMP3XX bmp;

// =================================================================================
// I2C Device Addresses & Protocol Constants
// =================================================================================
const int BNO_ADDRESS = 0x4A;   // IMU
const int BMP_ADDRESS = 0x77;   // Barometric Pressure Sensor
const int ASPD_ADDRESS = 0x28;  // Airspeed Sensor

const byte BMP_DATA_STRT_REG = 0x04;

const byte SHTP_REPORTS_CHANNEL = 3;
const byte SHTP_GAME_ROTATION_VECTOR_ID = 0x08;

// =================================================================================
// Data Structures for Sensor Readings
// =================================================================================
struct BaroData      { uint8_t lsb, msb, xlsb; };
struct AirspeedData  { int16_t pressure; };
struct ImuData       { int16_t i, j, k, real; };
struct GpsData       { float lat, lon, alt; };

struct MasterLog {
  uint32_t timestamp_ms;
  ImuData imu;
  BaroData baro;
  AirspeedData airspeed;
  GpsData gps;
  uint16_t pot1, pot2;
};

// =================================================================================
// Function Prototypes
// =================================================================================
bool init_BMP();
BaroData getBaroData();
AirspeedData getAirspeedData();
void processAndPrintData();

// =================================================================================
// Main Setup and Loop
// =================================================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); 
  }
  Serial.println("\n--- Simple Polling Datalogger Starting ---");

  Serial.println("Initializing I2C bus...");
  Wire.begin();
  Wire.setClock(100000);

  Serial.println("Initializing GPS serial port...");
  GpsSerial.begin(9600);

  Serial.println("Initializing BMP390...");
  if (!init_BMP()) {
    Serial.println("HALT: BMP390 initialization failed.");
    while(1);
  }
  Serial.println("BMP390 initialization successful.");
  Serial.println("\n--- Setup complete. Entering main loop. ---");
}

void loop() {
  // Continuously feed the GPS parser with any available data
  while (GpsSerial.available() > 0) {
    gps.encode(GpsSerial.read());
  }
  
  // Call the main data processing function
  processAndPrintData();
  
  // Wait for one second before the next sample
  delay(1000);
}

// =================================================================================
// Sensor Helper Functions
// =================================================================================

// --- BMP390 (Barometer) Functions ---
bool init_BMP() {
  if (!bmp.begin_I2C()) {
    Serial.println(" -> BMP390 not found. Check wiring.");
    return false;
  }
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  return true;
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

// --- ASPD-4525 (Airspeed) Function ---
AirspeedData getAirspeedData() {
  Wire.requestFrom(ASPD_ADDRESS, 2);
  if (Wire.available() >= 2) {
    byte byte1 = Wire.read();
    byte byte2 = Wire.read();
    int16_t pressure = ((int16_t)(byte1 & 0x3F) << 8) | byte2;
    return {pressure};
  }
  return {0};
}

// =================================================================================
// Main Data Logging & Printing Function
// =================================================================================
void processAndPrintData() {
  MasterLog log = {};

  // --- Attempt to read BNO085 Packet (no interrupt) ---
  // This may or may not return valid data, which is fine for this test.
  byte header[4];
  Wire.requestFrom(BNO_ADDRESS, 4);
  header[0] = Wire.read(); header[1] = Wire.read(); header[2] = Wire.read(); header[3] = Wire.read();
  int packetLength = ((uint16_t)header[1] << 8) | header[0];

  if (packetLength > 0 && packetLength < 512) { // Sanity check on packet length
    byte packetData[packetLength];
    packetData[0] = header[0]; packetData[1] = header[1];
    packetData[2] = header[2]; packetData[3] = header[3];
    Wire.requestFrom(BNO_ADDRESS, packetLength - 4);
    for (int i = 4; i < packetLength; i++) packetData[i] = Wire.read();

    if (header[2] == SHTP_REPORTS_CHANNEL && packetData[4] == SHTP_GAME_ROTATION_VECTOR_ID) {
      log.imu.i    = (int16_t)(((uint16_t)packetData[14] << 8) | packetData[13]);
      log.imu.j    = (int16_t)(((uint16_t)packetData[16] << 8) | packetData[15]);
      log.imu.k    = (int16_t)(((uint16_t)packetData[18] << 8) | packetData[17]);
      log.imu.real = (int16_t)(((uint16_t)packetData[20] << 8) | packetData[19]);
    }
  }

  // --- Read All Other Sensors ---
  log.timestamp_ms = millis();
  log.baro = getBaroData();
  log.airspeed = getAirspeedData();
  log.pot1 = analogRead(POT1_PIN);
  log.pot2 = analogRead(POT2_PIN);
  log.gps.lat = gps.location.isValid() ? gps.location.lat() : 0.0f;
  log.gps.lon = gps.location.isValid() ? gps.location.lng() : 0.0f;
  log.gps.alt = gps.altitude.isValid() ? gps.altitude.meters() : 0.0f;
    
  long baro_raw = (long)log.baro.xlsb << 16 | (long)log.baro.msb << 8 | (long)log.baro.lsb;

  // --- Print Formatted Output ---
  Serial.println("\n--- NEW DATA LOG ---");
  Serial.print("Timestamp: "); Serial.println(log.timestamp_ms);
  Serial.print("IMU (i,j,k,real): ");
  Serial.print(log.imu.i); Serial.print(", "); Serial.print(log.imu.j); Serial.print(", ");
  Serial.print(log.imu.k); Serial.print(", "); Serial.println(log.imu.real);
  Serial.print("Barometer (Raw): "); Serial.println(baro_raw);
  Serial.print("Airspeed (Raw): "); Serial.println(log.airspeed.pressure);
  Serial.print("GPS (Lat,Lon,Alt): ");
  Serial.print(log.gps.lat, 6); Serial.print(", ");
  Serial.print(log.gps.lon, 6); Serial.print(", "); Serial.println(log.gps.alt, 2);
  Serial.print("Pots (1,2): ");
  Serial.print(log.pot1); Serial.print(", "); Serial.println(log.pot2);
}

