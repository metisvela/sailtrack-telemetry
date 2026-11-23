#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  // Exact same GPS port and pins as SensorNode.cpp
  Serial2.setRxBufferSize(1024);
  Serial2.begin(9600, SERIAL_8N1, 4, 5);
  delay(250);
  // Keep 1 Hz default rate and enable GSV so you can see SNR directly
  Serial2.print("$PMTK220,1000*1F\r\n");
  Serial2.print("$PMTK314,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
}

void loop() {
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}