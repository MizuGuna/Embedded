#include <Arduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "gps.h"
#include "compass.h"
#include "constants.h"
#include "pins.h"

// Global GPS parser object for processing NMEA sentences
// Handles parsing of GPS data from serial stream
TinyGPSPlus gps;

// Software serial interface for GPS module communication
// Uses pins defined in pins.h for RX/TX communication with GPS module
SoftwareSerial ss(GPS_RX, GPS_TX);

// Initialize the GPS module communication
// Sets up software serial at 9600 baud rate (standard for most GPS modules)
// Must be called in setup() before using GPS functions
void initGPS() {
    ss.begin(9600);
}

// Creates a formatted path point message with current GPS location and heading
// Combines GPS coordinates with compass heading for complete position data
// Used for logging vehicle/device path for navigation or tracking purposes
// Returns false if GPS location is invalid (no fix), true if successful
// Output format: "PATH,latitude,longitude,heading" where coordinates have 6 decimal places
bool logPathPoint(String& outMsg) {
    // Check if GPS has a valid location fix
    if(!gps.location.isValid()) {
        return false;
    }

    // Get current compass heading
    int heading = readCompass();

    // Format message as CSV: PATH,lat,lon,heading
    // Latitude and longitude are formatted to 6 decimal places for ~1 meter accuracy
    outMsg = "PATH," + String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6) + "," + String(heading);
    return true;
}