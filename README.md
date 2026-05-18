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
* [RadioHead](https://github.com/metisvela/sailtrack-telemetry/blob/main/src/RadioHead.cpp): The node to send data from the boat to our system on our dinghy.
* [Display Node](https://github.com/metisvela/sailtrack-telemetry/blob/main/src/display.cpp): The node to display the data for the sailors on board.