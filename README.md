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

SailTrack Telemetry is an onboard data tracking system built by the [Metis Sailing Team](http://metisvela.dii.unipd.it), a student project of the University of Padova. The system collects, displays, and transmits real-time performance metrics for racing sailboats.

The system is designed to:
* Collect high-frequency orientation and motion data from onboard sensors.
* Display clean, high-contrast metrics to the crew via a 7.5-inch e-paper screen.
* Broadcast live data to a coach boat or ground station over long distances using a LoRa radio module.

---

## Main Nodes

The SailTrack Telemetry System is based on 3 ESP32 based parts. These nodes are connected to each other via a CAN BUS:
* [Sensor Node](https://github.com/metisvela/sailtrack-telemetry/blob/main/src/SensorNode.cpp): The main node with the IMU and GPS sensors connected to.
* [RadioHead](https://github.com/metisvela/sailtrack-telemetry/blob/main/src/RadioHead.cpp): The node to send data from the boat to our system on the dinghy.
* [Display Node](https://github.com/metisvela/sailtrack-telemetry/blob/main/src/display.cpp): The node to display the data for the sailors on board.

---

## Pinout Configuration

Below is the pin allocation for the spesific nodes. For more information, the pins are also allocated on the respected node's .cpp file: 

### Display Node
| Component | Pin Name | ESP32 GPIO | Description |
| :--- | :--- | :--- | :--- |
| **7.5" E-Paper Display** | `EPD_CS` | **GPIO 2** | SPI Chip Select |
| | `EPD_DC` | **GPIO 4** | Data / Command Control |
| | `EPD_RST` | **GPIO 16** | Hardware Reset |
| | `EPD_BUSY` | **GPIO 5** | Busy Signal Indicator |
| | `EPD_PWR` | **GPIO 17** | Display Power Rail Gate Control |
| | `EPD_SCK` | **GPIO 18** | SPI Clock |
| | `EPD_MOSI` | **GPIO 15** | SPI Master Out Slave In |
| **CAN Bus Transceiver** | `CAN_RX_PIN`| **GPIO 22** | CAN Receive Line |
| | `CAN_TX_PIN`| **GPIO 23** | CAN Transmit Line |
| **System Control** | `SLEEP_BUTTON`| **GPIO 27** | Sleep Mode Input Button |

### RadioHead
| Component | Pin Name | ESP32 GPIO | Description |
| :--- | :--- | :--- | :--- |
| **E32 868T20D LoRa Radio Module** | `RX` | **GPIO 16** | Radio Receive Line |
| | `TX` | **GPIO 17** | Radio Transmit Line |
| | `M0` | **GND** | Mode Selection(Same ground as the module's ground) |
| | `M1` | **GND** | Mode Selection(Same ground as the module's ground) |
| **CAN Bus Transceiver** | `CAN_RX_PIN`| **GPIO 25** | CAN Receive Line |
| | `CAN_TX_PIN`| **GPIO 26** | CAN Transmit Line |

### Sensor Node
| Component | Pin Name | ESP32 GPIO | Description |
| :--- | :--- | :--- | :--- |
| **IMU**  | `SCL` | **GPIO 25** | I2C SCL Pin |
| | `SDA` | **GPIO 27** | I2C SDA Pin |
| **GPS** | `TX` | **GPIO RX** | Data Transmit Line |
| | `RX` | **GPIO TX** | Data Receive Line |
| **CAN Bus Transceiver** | `CAN_RX_PIN`| **GPIO 22** | CAN Receive Line |
| | `CAN_TX_PIN`| **GPIO 23** | CAN Transmit Line |

---

## Data Protocol

Data is sent across the CAN Bus using small, optimized binary structures defined in `Protocol.h` to minimize latency.

### CAN Message IDs
* `0x101` (`ID_IMU_X`): Roll Vector
* `0x102` (`ID_IMU_Y`): Pitch Vector
* `0x103` (`ID_IMU_Z`): Yaw Vector
* `0x201` (`ID_GPS_POS`): LAT and LNG coordinates
* `0x202` (`ID_GPS_MOT`): Headding and SOG
* `0x203` (`ID_GPS_INFO`): Epoch and Time

### Struct Examples
```cpp
struct __attribute__((packed)) CAN_IMU_Frame {
    float v1; // Angle in degrees
    float v2; // Acceleration
};

struct __attribute__((packed)) CAN_GPS_MOTION {
    float knots;  // Speed Over Ground
    float headMot; // Headding
};

## Contributing

Contributors are welcome. If you are a student of the University of Padova, please apply for the Metis Sailing Team via our [official website](http://metisvela.dii.unipd.it), specifying in the application form that you are interested in contributing to the SailTrack Project. 

If you are not a student of the University of Padova, feel free to open Pull Requests and Issues directly within this repository to contribute to the telemetry system development.

---

## License

Copyright © 2026, [Metis Sailing Team](https://github.com/metisvela). SailTrack Telemetry is available under the [GPL-3.0 license](https://www.gnu.org/licenses/gpl-3.0.en.html).