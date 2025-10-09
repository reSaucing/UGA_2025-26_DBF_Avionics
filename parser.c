#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // Required for specific integer types like uint32_t

// --- CONFIGURATION ---
// TODO: Change this to the exact number you got from Serial.println(sizeof(completeLogEntry));
#define STRUCT_SIZE 36

// --- STRUCT DEFINITIONS ---
// These MUST EXACTLY MATCH the structs in your Arduino .ino file.

// Individual sensor data structuring
struct barometricData {
    uint8_t lsb, msb, xlsb;
};
struct airspeedData {
    int16_t pressure;
};
struct gameVectoringData {
    int16_t i, j, k, real;
};
struct gpsLocation {
    float lat, lon, alt;
};

// Complete data entry configured in set order
struct completeLogEntry {
    uint32_t timestamp_ms;
    struct gameVectoringData currentVector;
    struct barometricData currentBarAlt;
    struct airspeedData currentAirspeed;
    struct gpsLocation currentPOS;
    uint16_t pot1_raw;
    uint16_t pot2_raw;
};


void parse_log_file(const char* filename) {
    printf("--- Parsing Log File: %s ---\n", filename);
    printf("Assuming struct size: %d bytes\n\n", STRUCT_SIZE);

    // Open the file in binary read mode ("rb")
    FILE *file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL) {
        perror("Error opening file");
        return;
    }

    struct completeLogEntry entry;
    int entry_count = 0;

    // Read the file one struct at a time
    while (fread(&entry, STRUCT_SIZE, 1, file_ptr) == 1) {
        entry_count++;
        printf("--- Entry #%d ---\n", entry_count);

        // --- Post-process the raw data into more useful forms ---

        // Recombine the 3 bytes of the barometer reading
        uint32_t baro_raw = (entry.currentBarAlt.xlsb << 16) | (entry.currentBarAlt.msb << 8) | entry.currentBarAlt.lsb;

        // Scale the BNO085 quaternion values.
        // The Game Rotation Vector uses a Q-point of 14. 2^14 = 16384
        double q_i = entry.currentVector.i / 16384.0;
        double q_j = entry.currentVector.j / 16384.0;
        double q_k = entry.currentVector.k / 16384.0;
        double q_real = entry.currentVector.real / 16384.0;

        // --- Print the human-readable data ---
        printf("  Timestamp: %u ms\n", entry.timestamp_ms);
        printf("  IMU Quaternion (i, j, k, real): (%.4f, %.4f, %.4f, %.4f)\n", q_i, q_j, q_k, q_real);
        printf("  Barometer (Raw): %u\n", baro_raw);
        printf("  Airspeed (Raw): %d\n", entry.currentAirspeed.pressure);
        printf("  GPS (Lat, Lon, Alt): (%.6f, %.6f, %.2f m)\n", entry.currentPOS.lat, entry.currentPOS.lon, entry.currentPOS.alt);
        printf("  Potentiometers (Raw): Pot1=%u, Pot2=%u\n\n", entry.pot1_raw, entry.pot2_raw);
    }

    printf("--- End of File. Parsed %d total entries. ---\n", entry_count);
    fclose(file_ptr);
}

int main(int argc, char *argv[]) {
    // Check if a filename was provided as a command-line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <your_log_file.bin>\n", argv[0]);
        return 1;
    }

    parse_log_file(argv[1]);
    return 0;
}
