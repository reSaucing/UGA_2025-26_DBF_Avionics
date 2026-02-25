#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_BNO08x.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

// --- Pin & Address Configuration ---
#define ASPD_ADDR 0x28
const int POT1_PIN = 14; 
const int POT2_PIN = 15;
const int chipSelect = 10;
const int beeperPin = 3;
const unsigned long logInterval = 50; // 20 Hz (50ms)

// --- Objects & Global Variables ---
Adafruit_BMP3XX bmp;
Adafruit_BNO08x bno;
SFE_UBLOX_GNSS myGNSS;
File logFile;
char fileName[13];
unsigned long lastLogTime = 0;
float altTare = 0;
bool tareSet = false;
/* Beep codes */
int SDbeepNumber = 1;
int BMPbeepNumber = 2;
int BNObeepNumber = 3;
int allClearBeepNumber = 4; // to indicate everything is working normally

// --- State Machine ---
enum SystemState { WAITING, MANAGEMENT_MODE, LOGGING };
SystemState state = WAITING;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C for high-speed sensors
  Serial2.begin(38400);  // Standard u-blox baud

  // Initialize SD Card
  if (!SD.begin(chipSelect)) {
    while (true) beep(SDbeepNumber);
  }

  // Initialize Sensors
  if (!bmp.begin_I2C()) {
    while (true) beep(BMPbeepNumber);
  }
  if (!bno.begin_I2C()) {
    while (true) beep(BNObeepNumber);
  }
  myGNSS.begin(Serial2);

  // Sensor Settings
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  bno.enableReport(SH2_LINEAR_ACCELERATION, 50000);
  myGNSS.setNavigationFrequency(20);

  // set mode for beeper GPIO pin
  pinmode(beeperPin, OUTPUT);

  Serial.println("--- SYSTEM BOOT ---");
  Serial.println("Type 'm' within 10s for MANAGEMENT MODE (PuTTY Export)");
  Serial.println("Otherwise, 20Hz Logging starts automatically...");
  // beeps to indicate normal functioning
  beep(allClearBeepNumber);
}

void loop() {
  unsigned long currentTime = millis();

  //STATE 1: BOOT WINDOW
  if (state == WAITING) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == 'm') {
        state = MANAGEMENT_MODE;
        printManagerMenu();
      }
    }
    if (currentTime > 10000) {
      state = LOGGING;
      Serial.println(">>> STARTING LOGGING MODE");
    }
  }

  //STATE 2: DATA EXPORT (MANAGEMENT)
  else if (state == MANAGEMENT_MODE) {
    handleManagement();
  } 

  //STATE 3: FIELD LOGGING
  else if (state == LOGGING) {
    runLoggingCycle();
  }
}

void printManagerMenu() {
  Serial.println("\n--- MANAGEMENT MODE (STATIONARY) ---");
  Serial.println("l - List files");
  Serial.println("e - Export file to PuTTY");
  Serial.println("d - Delete specific file");
  Serial.println("x - Reset Teensy (to start logging)");
  Serial.println("------------------------------------");
}

void handleManagement() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == 'l') {
      File root = SD.open("/");
      Serial.println("\nSD Card Contents:");
      while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        Serial.print(entry.name());
        Serial.print("\t\t");
        Serial.print(entry.size());
        Serial.println(" bytes");
        entry.close();
      }
      root.close();
      printManagerMenu();
    } 
    
    else if (cmd == 'e') {
      Serial.println("Enter exact filename (e.g. DATA005.CSV):");
      while(Serial.available()) Serial.read(); 
      while(!Serial.available());
      String target = Serial.readStringUntil('\n');
      target.trim();

      File dump = SD.open(target.c_str(), FILE_READ);
      if (dump) {
        Serial.println("--- START OF FILE ---");
        while (dump.available()) Serial.write(dump.read());
        dump.close();
        Serial.println("\n--- END OF FILE ---");
      } else {
        Serial.println("File not found.");
      }
      printManagerMenu();
    }

    else if (cmd == 'd') {
      Serial.println("Enter filename to DELETE:");
      while(Serial.available()) Serial.read();
      while(!Serial.available());
      String target = Serial.readStringUntil('\n');
      target.trim();

      if (SD.remove(target.c_str())) {
        Serial.println("Deleted.");
      } else {
        Serial.println("Delete Failed.");
      }
      printManagerMenu();
    }
    
    else if (cmd == 'x') {
      Serial.println("Resetting...");
      SCB_AIRCR = 0x05FA0004; // Teensy Software Reset
    }
  }
}

void runLoggingCycle() {
  myGNSS.checkUblox();
  unsigned long now = millis();

  if (now - lastLogTime >= logInterval) {
    lastLogTime = now;

    // 1. BMP Altitude & Tare
    float rawAltFt = bmp.readAltitude(997.6) * 3.28084;
/*    if (!tareSet && now > 5000) {
      altTare = rawAltFt;
      tareSet = true;
    }*/
    float alt = tareSet ? (rawAltFt - altTare) : 0;

    // 2. Airspeed (I2C)
    float aspd = 0;
    Wire.requestFrom(ASPD_ADDR, 2);
    if (Wire.available() >= 2) {
      uint16_t raw = (Wire.read() << 8) | Wire.read();
      aspd = convToPa(raw);
    }

    // 3. IMU (BNO085)
    float ax=0, ay=0, az=0;
    sh2_SensorValue_t sensorData;
    if (bno.getSensorEvent(&sensorData) && sensorData.sensorId == SH2_LINEAR_ACCELERATION) {
      ax = sensorData.un.linearAcceleration.x;
      ay = sensorData.un.linearAcceleration.y;
      az = sensorData.un.linearAcceleration.z;
    }

    // 4. File Management
    if (!logFile) {
      for (int i = 0; i < 1000; i++) {
        snprintf(fileName, sizeof(fileName), "DATA%03d.CSV", i);
        if (!SD.exists(fileName)) {
          logFile = SD.open(fileName, FILE_WRITE);
          break;
        }
      }
      if (logFile) {
        logFile.println("ms,alt_ft,aspd,ax,ay,az,lat,lon,p1,p2");
        Serial.println("Recording to: " + String(fileName));
      }
    }

    // 5. Write to Disk
    if (logFile) {
      logFile.print(now); logFile.print(",");
      logFile.print(alt, 2); logFile.print(",");
      Serial.print(rawAltFt);
      Serial.println();
      logFile.print(aspd, 2); logFile.print(",");
      Serial.print(aspd);
      Serial.println();
      logFile.print(ax, 3); logFile.print(",");
      Serial.print(ax);
      Serial.println();
      Serial.println();
      logFile.print(ay, 3); logFile.print(",");
      logFile.print(az, 3); logFile.print(",");
      logFile.print(myGNSS.getLatitude()/10000000.0, 7); logFile.print(",");
      logFile.print(myGNSS.getLongitude()/10000000.0, 7); logFile.print(",");
      logFile.print(analogRead(POT1_PIN)); logFile.print(",");
      logFile.println(analogRead(POT2_PIN));

      // Flush to SD every 2 seconds
      if (now % 2000 < 50) {
        logFile.flush();
      }
    }
  }
}

float convToPa(uint16_t airSpeed){

  float psi=(airSpeed-(.1*16383))*((2)/(.8*16383))-1;//(airSpeed-(b*pDigiCount))*((Pmax-Pmin)/(a*pDigiCount))+Pmin;
  float pa = psi* 0.45359237f * 9.80665f / 0.0254f / 0.0254f;
  float mPerSec=sqrt((2*pa)/1.196);

  return mPerSec;
}

/**
 * Function to make the beeper beep a given number of times. Each beep will be 0.25 seconds, and there will be
 * 0.25 second delay between beeps. There will be a 0.5 second delay after the last beep.
 */
void beep(int beeps) {
  for (int i = 0; i < beeps; i++) {
    digitalWrite(beeperPin, HIGH);
    delay(250);
    digitalWrite(beeperPin, LOW);
    delay(250);
  }
  delay(250);
}