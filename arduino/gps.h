#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Header file for GPS module interface
// Provides functions for GPS initialization and position logging
// Uses TinyGPS++ library for parsing NMEA GPS data
// Integrates with compass module for complete position and heading data

// External declaration of global GPS parser object
// Actual object is defined in gps.cpp
extern TinyGPSPlus gps;

// External declaration of software serial interface for GPS communication
// Configured with pins defined in pins.h
extern SoftwareSerial ss;

// Function to initialize GPS module communication
// Sets up software serial communication with GPS module
// Must be called before attempting to read GPS data
void initGPS();

// Function to create a formatted path point message
// Combines current GPS coordinates with compass heading
// Output parameter 'outMsg' contains formatted CSV string if successful
// Returns false if GPS location is invalid (no satellite fix)
// Returns true if valid location data is available and message is created
// Message format: "PATH,latitude,longitude,heading"
bool logPathPoint(String& outMsg);

#endif