#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "compass.h"
#include "constants.h"

// Global magnetometer object used for reading compass data
// Uses unique ID 12345 for sensor identification
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// Initializes the HMC5883 compass sensor
// Sets up I2C communication and verifies sensor connection
// If sensor is not detected, prints error message and enters infinite loop
void initCompass() {
    if(!mag.begin())
    {
        Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
        while(true);
    }
}

// Reads compass heading and averages over multiple samples for accuracy
// Takes 10 readings with 10ms delay between each reading
// Applies calibration normalization using min/max values from constants.h
// Applies local magnetic declination correction for geographic accuracy
// Returns heading in degrees (0-359) as integer
int readCompass() {
  sensors_event_t event;
  
  float sum_heading = 0;
  float heading;

  // Take 10 samples and average them for better accuracy
  for (int i = 0; i < 10; i++) {
    mag.getEvent(&event);

    // Normalize X and Y magnetometer readings using calibration values
    // Maps raw sensor values to -1.0 to +1.0 range
    float x_norm = ((event.magnetic.x - x_min) / (x_max - x_min)) * 2.0 - 1.0;
    float y_norm = ((event.magnetic.y - y_min) / (y_max - y_min)) * 2.0 - 1.0;

    // Calculate heading angle using arctangent of Y/X components
    heading = atan2(y_norm, x_norm);

    // Apply local magnetic declination correction (~0.009 radians)
    // This corrects for the difference between magnetic north and true north
    heading += declinationAngle;

    sum_heading += heading;
    delay(10); // Small delay between readings to allow sensor to settle
  }

  // Calculate average heading from all samples
  heading = sum_heading / 10.0;

  // Normalize heading to 0-2π range
  if (heading < 0) heading += 2 * PI;
  if (heading > 2 * PI) heading -= 2 * PI;

  // Convert from radians to degrees and return as integer
  float headingDegrees = heading * 180.0 / PI;
  return (int)headingDegrees;
}