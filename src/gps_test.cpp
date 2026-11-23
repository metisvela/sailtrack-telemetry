#include <Arduino.h>
#include <TinyGPS++.h>

// Exact same port, pins, and baud rate from SensorNode.cpp
#define GPS_RX_PIN 4
#define GPS_TX_PIN 5
#define GPS_BAUD 9600

TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- Bare Minimum GPS Test (Pins 4 & 5) ---");

  SerialGPS.setRxBufferSize(1024);
  SerialGPS.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  delay(250);

  // Configure Adafruit Ultimate GPS: RMC + GGA sentences only, 1 Hz update rate
  SerialGPS.print("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
  delay(100);
  SerialGPS.print("$PMTK220,1000*1F\r\n");
}

void loop() {
  // Continuously feed incoming GPS bytes to TinyGPS++
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  // Print status once every 1000 ms
  if (millis() - lastPrintTime >= 1000) {
    lastPrintTime = millis();

    // 1. Satellites
    Serial.print("Sats: ");
    if (gps.satellites.isValid()) {
      Serial.print(gps.satellites.value());
    } else {
      Serial.print("WAIT");
    }

    // 2. Date (YYYY-MM-DD)
    Serial.print(" | Date: ");
    if (gps.date.isValid() && gps.date.year() > 2000) {
      Serial.printf("%04d-%02d-%02d", gps.date.year(), gps.date.month(), gps.date.day());
    } else {
      Serial.print("----/--/--");
    }

    // 3. Time (HH:MM:SS UTC)
    Serial.print(" | Time: ");
    if (gps.time.isValid()) {
      Serial.printf("%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    } else {
      Serial.print("--:--:--");
    }

    // 4. Coordinates (Lat, Lng)
    Serial.print(" | Coords: ");
    if (gps.location.isValid()) {
      Serial.printf("%.6f, %.6f", gps.location.lat(), gps.location.lng());
    } else {
      Serial.print("WAIT");
    }

    Serial.println();
  }
}