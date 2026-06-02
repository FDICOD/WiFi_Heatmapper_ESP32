# ESP32 WiFi Signal Heatmap Mapper

# Overview

An ESP32-based WiFi signal analysis and visualization system that measures RSSI values across a physical space and displays them as a real-time color heatmap on an ST7789 TFT display.

The system allows a user to scan nearby WiFi networks, select a target SSID, and map signal strength over a 6×8 grid. Each cell in the grid represents a sampling location where the received signal strength is measured and visualized using a color-coded representation.

The project combines embedded systems, wireless networking, graphical rendering, and real-time data acquisition into a portable standalone device.

---

# Purpose

WiFi signal strength can vary significantly within indoor environments due to walls, interference, furniture placement, and access point positioning. The purpose of this project is to provide a simple and portable method for visualizing signal distribution within a room or building. By mapping RSSI values spatially, the system can help identify:

* Dead zones
* Weak coverage regions
* Areas affected by attenuation or interference

The project was also designed as an embedded systems and microcontroller-oriented engineering exercise involving:

* ESP32 WiFi functionality
* TFT graphics rendering
* State-machine based UI design
* Signal strength analysis
* Real-time data visualization

## Design Choice

The current implementation uses a manually sampled cell by cell mapping approach. This design was chosen because it is significantly simpler and more cost effective than implementing location aware positioning systems such as GPS, indoor localization, or simultaneous mapping methods. Since GPS is unreliable indoors and additional localization hardware would increase system complexity and cost, the project instead relies on manual spatial sampling across a predefined grid.


---

# Features

* WiFi network scanning
* SSID selection interface
* RSSI acquisition using ESP32 WiFi hardware
* 6×8 spatial heatmap visualization
* Color-coded signal quality mapping
* TFT graphical interface using ST7789 display
* Single-button navigation system
* Statistical summary display for measured data

---

# Hardware Components

| Component          | Description                             |
| ------------------ | --------------------------------------- |
| ESP32              | Main microcontroller and WiFi interface |
| ST7789 TFT Display | Graphical heatmap display               |
| Push Button        | User interaction and control            |
| USB Power Source   | System power supply                     |

## Additional Components (Optional)

| Component | Purpose |
|---|---|
| Li-ion Battery Pack | Portable standalone operation |
| TP4056 Charging Module | Battery charging and protection |
| SD Card Module | Logging RSSI measurements |

---

# Software Libraries

* WiFi.h
* Adafruit_GFX
* Adafruit_ST7789
* SPI.h

---

# Working Principle

The ESP32 first scans nearby WiFi networks and displays the detected SSIDs on the TFT display. The user navigates the list using a single-button interface and selects the target network.

After selection, the display switches to a 6×8 grid representing the measurement area. At each physical location, the system measures the RSSI value of the selected WiFi network and stores it in memory (Scanning takes about 1-5 seconds, depending on network density and environmental conditions).

Each measured value is converted into a color based on predefined RSSI thresholds. Stronger signals are displayed using warm colors, while weaker signals are represented using cooler colors.

Once all grid cells have been sampled, the system generates a statistical summary showing:

* Best RSSI value
* Worst RSSI value
* Average RSSI value

---

# RSSI Color Mapping

| RSSI Range         | Color   | Signal Quality |
| ------------------ | ------- | -------------- |
| > -45 dBm          | Red    | Excellent      |
| -55 dBm to -45 dBm | Orange    | Very Good      |
| -65 dBm to -55 dBm | Yellow | Good           |
| -72 dBm to -65 dBm | Magenta  | Moderate       |
| -80 dBm to -72 dBm | Blue  | Weak           |
| < -80 dBm          | Cyan     | Poor           |

## Display Calibration

The ST7789 display module used in this project exhibited inverted color behavior relative to the intended heatmap representation. To compensate for this, the RSSI-to-color mapping logic was adjusted in software so that the displayed colors correctly represented signal strength categories (tft.invertDisplay(true) which usually works, didn't).


---

# User Interface Flow

## 1. WiFi Scan

The ESP32 scans nearby wireless networks and displays available SSIDs along with their RSSI values.

## 2. Network Selection

The user selects a target SSID using short and long button presses.

## 3. Heatmap Generation

RSSI measurements are recorded for each grid cell and visualized in real time.

## 4. Summary Screen

After completing all measurements, the system displays statistical information for the collected dataset.

---

# Project Structure

```text
WiFi_Heatmapper_ESP32/
│
├── README.md
├── code/
├── docs/
├── images/
└── schematics/
```

---

# Future Improvements

* Automatic sampling intervals
* SD card data logging
* Battery-powered enclosure
* Interpolated heatmap rendering

---

# Applications

* Indoor WiFi coverage analysis
* Router placement optimization
* RF signal visualization
* Educational demonstrations for wireless networking
* Embedded systems and IoT experimentation

---

# License

This project is intended for educational and non-commercial use.
