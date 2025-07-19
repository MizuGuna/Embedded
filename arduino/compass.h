#ifndef COMPASS_H
#define COMPASS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

// Header file for compass (HMC5883) sensor functionality
// Provides interface for initializing and reading compass heading data
// Uses Adafruit HMC5883 library for magnetometer communication

// External declaration of global magnetometer object
// Actual object is defined in compass.cpp
extern Adafruit_HMC5883_Unified mag;

// Function to initialize the compass sensor
// Must be called before using readCompass() function
// Will halt execution if sensor is not detected
void initCompass();

// Function to read the compass heading with averaging and calibration
// Returns heading in degrees from 0-359 degrees
// 0 degrees = North, 90 = East, 180 = South, 270 = West
// Applies calibration correction and magnetic declination
int readCompass();

#endif