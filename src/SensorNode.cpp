#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_AHRS.h>
#include <Adafruit_Sensor_Calibration.h>
#include <TinyGPS++.h>
#include <esp_task_wdt.h> 
#include <SD.h>
#include <FS.h>

#define SD_CS_PIN 21

bool sdFound = false;
String logFileName = "/temp_flight.csv"; // Temporary name to catch early data
bool isFileRenamed = false;              // Tracks if GPS has locked and renamed the file

// --- E32 Radio Configuration ---
#define RXD2 12
#define TXD2 13
#define RADIO_BAUD 9600

// --- Configuration ---
#define LED_PIN 2 
#define I2C_SDA_PIN 27
#define I2C_SCL_PIN 25
#define GPS_BAUD 9600
#define RECOVERY_INTERVAL 500 // 0.5 seconds

// --- Global Objects ---
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
Adafruit_NXPSensorFusion filter;
Adafruit_Sensor_Calibration_EEPROM cal;

// --- State Variables ---
bool imuFound = false;
bool gpsFound = false;
unsigned long lastHeartbeat = 0;
bool ledState = LOW;
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000;
unsigned long lastRecoveryAttempt = 0;

float linearAccelX, linearAccelY, linearAccelZ;


// --- Recovery Logic ---
void attemptModuleRecovery() {
    if (millis() - lastRecoveryAttempt < RECOVERY_INTERVAL) return;
    lastRecoveryAttempt = millis();

    if (!imuFound) {
        Serial.println("[RECOVERY] Trying IMU...");
        Wire.end(); 
        Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
        Wire.setTimeOut(50);
        if (lsm.begin()) {
            lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
            lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
            lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
            imuFound = true;
            Serial.println("[RECOVERY] IMU Success!");
        }
    }

    if (!gpsFound) {
        if (SerialGPS.available() > 0) {
            gpsFound = true;
            Serial.println("[RECOVERY] GPS Success!");
        }
    }

    if (!sdFound) {
        if (SD.begin(SD_CS_PIN)) {
            sdFound = true;
            Serial.println("[RECOVERY] SD Card Success!");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(RADIO_BAUD, SERIAL_8N1, RXD2, TXD2);
    pinMode(LED_PIN, OUTPUT);

    // Classic Watchdog Setup (for recovery)
    esp_task_wdt_init(5, true); 
    esp_task_wdt_add(NULL);

    // I2C Setup
    Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.begin();
    Wire.setTimeOut(50);

    // Sensor Init
    Serial.print("IMU Init... ");
    if (lsm.begin()) {
        lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
        lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
        lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
        imuFound = true;
        Serial.println("OK");
    } else { Serial.println("FAIL"); }

    cal.begin();
    cal.loadCalibration();
    filter.begin(10); // 10Hz filter

    SerialGPS.begin(GPS_BAUD, SERIAL_8N1, 4, 5);

    // SD Card Setup
    Serial.print("SD Card Init... ");
    if (SD.begin(SD_CS_PIN)) {
        sdFound = true;
        Serial.println("OK");
        
        // Create CSV Header if the temporary file is new
        File file = SD.open(logFileName, FILE_APPEND);
        if (file && file.size() == 0) {
            file.println("TimeMs,Roll,Pitch,Yaw,AccX,AccY,AccZ,Lat,Lng,Knots,Course,Sats,GPSTime");
        }
        if (file) file.close();
    } else { 
        Serial.println("FAIL - Running without local logging"); 
    }
    
    // Boot Indicator
    for(int i=0; i<3; i++) {
        digitalWrite(LED_PIN, HIGH); delay(100);
        digitalWrite(LED_PIN, LOW);  delay(100);
    }
}

void loop() {
    esp_task_wdt_reset(); // Watchdog setup

    // Heartbeat
    if (millis() - lastHeartbeat >= 500) {
        lastHeartbeat = millis();
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }

    // Recovery if needed
    attemptModuleRecovery();

    // Process IMU
    if (imuFound) {
        sensors_event_t a, m, g, t;
        if (lsm.getEvent(&a, &m, &g, &t)) {
            cal.calibrate(a); cal.calibrate(m); cal.calibrate(g); //added new
            filter.update(g.gyro.x * SENSORS_RADS_TO_DPS, g.gyro.y * SENSORS_RADS_TO_DPS, g.gyro.z * SENSORS_RADS_TO_DPS,
                          a.acceleration.x, a.acceleration.y, a.acceleration.z,
                          m.magnetic.x, m.magnetic.y, m.magnetic.z);
            filter.getLinearAcceleration(&linearAccelX, &linearAccelY, &linearAccelZ);
        } else {
            imuFound = false; // Mark for recovery
        }
    }

    //  Process GPS
    int maxChars = 100; 
    while (SerialGPS.available() > 0 && maxChars > 0) {
        if (gps.encode(SerialGPS.read())) {
            gpsFound = true;
        }
        maxChars--;
    }

    // Send Data
    // Send Data (Restored to the original CAN-style batch timing)
    if (millis() - lastSendTime >= sendInterval) { // sendInterval is 500
        lastSendTime = millis();

        if (imuFound) {
            // Replicates Send_CAN_IMU with 500us gaps
            Serial2.printf("IMU_X,%.2f,%.2f\n", filter.getRoll(), linearAccelX);
            Serial.printf("IMU_X,%.2f,%.2f\n", filter.getRoll(), linearAccelX);
            delayMicroseconds(500);

            Serial2.printf("IMU_Y,%.2f,%.2f\n", filter.getPitch(), linearAccelY);
            Serial.printf("IMU_Y,%.2f,%.2f\n", filter.getPitch(), linearAccelY);
            delayMicroseconds(500);

            Serial2.printf("IMU_Z,%.2f,%.2f\n", filter.getYaw(), linearAccelZ);
            Serial.printf("IMU_Z,%.2f,%.2f\n", filter.getYaw(), linearAccelZ);
            delayMicroseconds(500);
        }

        if (gpsFound) {
            // Replicates Send_CAN_GPS with 500us gaps
            Serial2.printf("GPS_POS,%.6f,%.6f\n", gps.location.lat(), gps.location.lng());
            Serial.printf("GPS_POS,%.6f,%.6f\n", gps.location.lat(), gps.location.lng());
            delayMicroseconds(500);

            Serial2.printf("GPS_MOT,%.2f,%.2f\n", gps.speed.knots(), gps.course.deg());
            Serial.printf("GPS_MOT,%.2f,%.2f\n", gps.speed.knots(), gps.course.deg());
            delayMicroseconds(500);

            Serial2.printf("GPS_INFO,%d,%d\n", gps.satellites.value(), gps.time.value());
            Serial.printf("GPS_INFO,%d,%d\n", gps.satellites.value(), gps.time.value());
        }

        // --- 2. SD Card CSV Logging (With on-the-fly Renaming) ---
        if (sdFound) {
            
            // STEP A: Rename the file the moment GPS time becomes valid
            if (!isFileRenamed && gps.date.isValid() && gps.time.isValid() && gps.date.year() > 2000) {
                char newName[32];
                // Formats name as: /YYYYMMDD_HHMM.csv
                sprintf(newName, "/%04d%02d%02d_%02d%02d.csv", 
                        gps.date.year(), gps.date.month(), gps.date.day(), 
                        gps.time.hour(), gps.time.minute());
                
                // Physically rename the file on the SD Card
                if (SD.rename(logFileName, newName)) {
                    logFileName = String(newName); // Update the variable to the new path
                    isFileRenamed = true;          // Stop checking
                    Serial.println(">>> SD Log Renamed to: " + logFileName);
                }
            }

            // STEP B: Write the data to whatever the current filename is
            File file = SD.open(logFileName, FILE_APPEND);
            if (file) {
                file.printf("%lu,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.6f,%.6f,%.2f,%.2f,%d,%d\n",
                    millis(),
                    imuFound ? filter.getRoll() : 0.0,
                    imuFound ? filter.getPitch() : 0.0,
                    imuFound ? filter.getYaw() : 0.0,
                    imuFound ? linearAccelX : 0.0,
                    imuFound ? linearAccelY : 0.0,
                    imuFound ? linearAccelZ : 0.0,
                    gpsFound ? gps.location.lat() : 0.0,
                    gpsFound ? gps.location.lng() : 0.0,
                    gpsFound ? gps.speed.knots() : 0.0,
                    gpsFound ? gps.course.deg() : 0.0,
                    gpsFound ? gps.satellites.value() : 0,
                    gpsFound ? gps.time.value() : 0
                );
                file.close(); 
            } else {
                sdFound = false; // Mark for recovery if card is shaken loose
            }
        }
    }
}