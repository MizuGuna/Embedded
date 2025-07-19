#ifndef CONSTANTS_H
#define CONSTANTS_H

// Project-wide constants for sensor calibration, physical properties, and timing intervals
// All sensor-related constants, thresholds, and configuration values are defined here
// Modify these values to adjust sensor behavior and system timing

// Physical constants for ultrasonic distance calculation
constexpr float SOUND_SPEED = 340.1;  // Speed of sound in air at room temperature (m/s)
constexpr unsigned long PULSE_TIMEOUT = 12000;  // Maximum time to wait for ultrasonic echo (microseconds)

// Compass calibration and magnetic declination settings
constexpr float declinationAngle = 0.009;  // Local magnetic declination angle in radians (~0.5 degrees)
// Compass hard iron calibration values - determined through calibration procedure
constexpr float x_min = -27.64;  // Minimum X magnetometer reading during calibration
constexpr float x_max = 43.36;   // Maximum X magnetometer reading during calibration
constexpr float y_min = -47.82;  // Minimum Y magnetometer reading during calibration
constexpr float y_max = 24.36;   // Maximum Y magnetometer reading during calibration

// Obstacle detection thresholds for ultrasonic sensor
constexpr int MIN_OBS_DISTANCE = 15;  // Minimum distance in cm to trigger obstacle detection
constexpr int MAX_OBS_DISTANCE = 30;  // Maximum distance in cm for obstacle detection range

// Communication timing intervals
constexpr unsigned long PATH_INTERVAL = 1000;  // Time between GPS path log messages (milliseconds)
constexpr unsigned long OBS_INTERVAL = 1000;   // Time between obstacle detection messages (milliseconds)

#endif