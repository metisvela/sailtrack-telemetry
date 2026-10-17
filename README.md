<p class="readme-header" align="center">
  <img src="assets/sailtrack-logo.svg" width="180">
</p>

<h3 class="readme-header" align="center">SailTrack Telemetry System</h3>
<p class="readme-header" align="center">
  <img src="https://img.shields.io/github/license/metisvela/sailtrack-telemetry">
  <img src="https://img.shields.io/github/stars/metisvela/sailtrack-telemetry">
</p>

<img src="assets/dashboard-image.png" class="readme-header">

## Overview
Information on this read me file are not correct. they are due corrections in regards to the upgrades on the system.

SailTrack Telemetry is an onboard data tracking system built by the [Metis Sailing Team](http://metisvela.dii.unipd.it), a student project of the University of Padova. The system collects, records, and transmits real-time performance metrics for racing sailboats.

The system is designed to:
* Collect high-frequency data from onboard sensors. Including:
  * 3-axis gyroscope
  * 3-axis magnetometer
  * 3-axis accelerometer
  * GNSS positioning and velocity
* Broadcast live data to a coach boat or ground station over long distances using a LoRa radio module.

---

## Hardware

The SailTrack Telemetry System is based on an ESP32-S3 with 4 modules:
* Adafruit LSM9DS1 9-DOF IMU for orrientation
* Neo M8N GPS for position and velocity
* Ebyte E32-868T20D LoRa module for live streaming
* MicroSD Card Reader for storeing the data

---

## Pinout Configuration

Below is the pin allocation for the spesific modules:

### Display Node
| Component | Pin Name | ESP32 GPIO | Description |
| :--- | :--- | :--- | :--- |
| **IMU** | `SCL` | **GPIO 25** | I2C SCL Pin |
| | `SDA` | **GPIO 27** | I2C SDA Pin |
| **GPS** | `TX` | **GPIO 4** | Data Transmit Line |
| | `RX` | **GPIO 5** | Data Receive Line |
| **Radio** | `RX` | **GPIO 12** | Data Receive Line |
| | `TX` | **GPIO 13** | Data Transmit Line |
| | `M0` | **GND** | Mode Selection(Same ground as the module's ground) |
| | `M1` | **GND** | Mode Selection(Same ground as the module's ground) |
| **SD Card Reader** | `CS` | **GPIO 21**| Data Pin |
---

## Data Protocol

The telemetry system outputs ASCII CSV-formatted messages over Serial/LoRa at 9600 baud. Each packet ends with `\n`.

### Telemetry Packets

* **IMU Roll & Accel X:** `IMU_X,<Roll_deg>,<AccelX_m/s2>`
* **IMU Pitch & Accel Y:** `IMU_Y,<Pitch_deg>,<AccelY_m/s2>`
* **IMU Yaw & Accel Z:** `IMU_Z,<Yaw_deg>,<AccelZ_m/s2>`
* **GPS Position:** `GPS_POS,<Latitude>,<Longitude>`
* **GPS Motion:** `GPS_MOT,<Speed_knots>,<Course_deg>`
* **GPS Metadata:** `GPS_INFO,<Satellites>,<GPS_Time>`

### SD Card Logging Format

Local data is saved to SD card in standard CSV format:
`TimeMs,Roll,Pitch,Yaw,AccX,AccY,AccZ,Lat,Lng,Knots,Course,Sats,GPSTime`

Log files are created as `/temp_flight.csv` and automatically renamed to `/YYYYMMDD_HHMM.csv` once a GPS lock is achieved.
--- 
## Contributing

Contributors are welcome. If you are a student of the University of Padova, please apply for the Metis Sailing Team via our [official website](http://metisvela.dii.unipd.it), specifying in the application form that you are interested in contributing to the SailTrack Project. 

If you are not a student of the University of Padova, feel free to open Pull Requests and Issues directly within this repository to contribute to the telemetry system development.

---

## License

Copyright © 2027, [Metis Sailing Team](https://github.com/metisvela). SailTrack Telemetry is available under the [GPL-3.0 license](https://www.gnu.org/licenses/gpl-3.0.en.html).
