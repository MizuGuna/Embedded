#ifndef PINS_H
#define PINS_H

// Pin assignment definitions for all modules and sensors
// Centralized pin configuration to avoid conflicts and simplify wiring changes
// Modify these values to match your hardware wiring configuration

// GPS module serial communication pins
// Uses software serial for communication with GPS module
constexpr uint8_t GPS_RX = 7;  // Arduino pin connected to GPS module TX
constexpr uint8_t GPS_TX = 6;  // Arduino pin connected to GPS module RX

// LoRa module SPI and control pins
// Standard SPI communication plus additional control pins
constexpr uint8_t LoRa_SS = 10;    // Slave Select pin for SPI communication
constexpr uint8_t LoRa_RST = 9;    // Reset pin to restart LoRa module
constexpr uint8_t LoRa_DIO0 = 3;   // Digital I/O pin 0 for interrupt/status

// Ultrasonic distance sensor pins
// HC-SR04 compatible ultrasonic sensor interface
constexpr uint8_t TRIG_PIN = 5;  // Trigger pin to initiate distance measurement
constexpr uint8_t ECHO_PIN = 4;  // Echo pin to receive distance measurement pulse

// Water quality sensor system pins
// Analog and digital pins for various water quality measurements
constexpr uint8_t ONE_WIRE_BUS = 8;  // Dallas OneWire bus for temperature sensor (DS18B20)
constexpr uint8_t TDS_PIN = A0;      // Analog pin for Total Dissolved Solids sensor
constexpr uint8_t PH_PIN = A1;       // Analog pin for pH sensor
// Note: ORP sensor pin (A2) is defined in sensorSystem.cpp due to conditional compilation

#endif