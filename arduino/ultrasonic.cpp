#include "ultrasonic.h"
#include <Arduino.h>
#include "constants.h"
#include "pins.h"

// Initialize ultrasonic sensor pins for HC-SR04 or compatible sensor
// Sets trigger pin as output for sending ultrasonic pulses
// Sets echo pin as input for receiving reflected pulses
// Must be called in setup() before using measureDistance()
void initUltrasonic() {
    pinMode(TRIG_PIN, OUTPUT);  // Trigger pin sends 10μs pulse to start measurement
    pinMode(ECHO_PIN, INPUT);   // Echo pin receives reflected pulse timing
}

// Measure distance using ultrasonic time-of-flight principle
// Sends 10μs trigger pulse, measures echo return time
// Calculates distance using speed of sound and travel time
// Includes timeout protection to prevent infinite blocking
// Returns distance in centimeters, or 999 if no valid reading
float measureDistance() {
  // Ensure trigger pin starts low
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send 10μs trigger pulse to initiate measurement
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long ping_travel_time;

  // Measure time for echo pulse (HIGH duration on echo pin)
  // PULSE_TIMEOUT prevents infinite blocking if no echo received
  ping_travel_time = pulseIn(ECHO_PIN, HIGH, PULSE_TIMEOUT);

  // Return error code if no pulse received (timeout occurred)
  if (ping_travel_time == 0) return 999;

  // Calculate one-way travel time (divide by 2 for round trip)
  // Convert microseconds to seconds for calculation
  float time = (ping_travel_time / 1E6) / 2;

  // Calculate distance using speed of sound
  // Distance = speed × time
  // Multiply by 100 to convert meters to centimeters
  float distance = SOUND_SPEED * time * 100;

  return distance;
}