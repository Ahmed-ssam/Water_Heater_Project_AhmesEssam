# Electric Water Heater Controller (AVR ATmega32) 🌡️

This project is a firmware implementation for an Electric Water Heater system based on the **ATmega32** microcontroller. It features a complete **Layered Architecture** (HAL, MCAL, LIB) and implements robust control logic including temperature sensing, hysteresis, and non-volatile data storage.

##  Key Features

* **Intelligent Temperature Sensing:**
    * Uses **ADC** with **LM35** sensor.
    * Implements a **Moving Average Filter** (buffer of 10 readings) to eliminate sensor noise and signal fluctuations.
* **User Interface:**
    * Multiplexed **7-Segment Display** for real-time temperature monitoring.
    * **"Flicker-Free" Logic:** The display refresh rate is maintained even during button debounce delays or blocking loops.
    * 3-Button Control: ON/OFF, UP, DOWN.
* **Control Logic:**
    * **Hysteresis Loop:** Maintains temperature within a ±5°C range to prevent relay chattering and extend hardware life.
    * **Cooling & Heating Elements:** Controls a Heater (Relay), Cooler (Fan), and Status LEDs.
* **Memory Persistence:**
    * Uses external **EEPROM (24C08)** over **I2C** protocol.
    * Saves the user's desired temperature so it persists after power loss.

##  Software Architecture

The code is organized into three layers for modularity and portability:

1.  **Application Layer:** `main.c` (Contains the system logic/state machine).
2.  **HAL (Hardware Abstraction Layer):**
    * `SSD`: 7-Segment Display Driver.
    * `EEPROM`: External Memory Driver (24C08).
3.  **MCAL (Microcontroller Abstraction Layer):**
    * `DIO`: Digital I/O Control.
    * `ADC`: Analog-to-Digital Converter.
    * `TWI`: Two-Wire Interface (I2C).
4.  **LIB:** Standard Types and Bit Math macros.

##  Hardware Specifications

* **MCU:** ATmega32 (8MHz Clock).
* **Sensor:** LM35 Temperature Sensor (ADC Channel 0).
* **Display:** 2-Digit Multiplexed 7-Segment (Common Cathode).
* **Input:** 3 Push Buttons (Internal/External Pull-up).
* **Output:** Relay (Heater), Motor (Cooler), LEDs.

##  Simulation
<img width="1178" height="823" alt="Protues screenshot" src="https://github.com/user-attachments/assets/6aed46f4-d2be-4d54-b7b7-bf8bcb909bf7" />


