#ifndef SENSOR_SYSTEM_H
#define SENSOR_SYSTEM_H

#include <Arduino.h>

// Header file for water quality sensor system
// Provides interface for reading temperature, pH, TDS, and ORP sensors
// Includes calibration functions for ORP sensor accuracy
// Designed for aquatic monitoring applications

// Function to initialize all water quality sensors
// Sets up OneWire temperature sensors and loads ORP calibration
// Must be called in setup() before using other sensor functions
void initSensorSystem();

// Function to get formatted string with all sensor readings
// Returns pipe-delimited string containing all sensor values
// Format: "Temp:XX.XX | pH:X.XX | TDS:XXX.X | ORP:XXX"
// Used for data logging and wireless transmission
String getSensorDataString();

// Function to calibrate ORP sensor to known reference value
// Performs single-point calibration using standard solution
// Parameter: value - ORP of reference solution in millivolts
// Calibration is stored in EEPROM for persistence across power cycles
void calibrateORP(int value);

// Function to clear ORP sensor calibration
// Removes stored calibration offset from EEPROM
// Sensor returns to raw, uncalibrated readings
// Use before performing new calibration procedure
void clearORPCalibration();

#endif