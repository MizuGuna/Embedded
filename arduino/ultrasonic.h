#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// Header file for ultrasonic distance sensor interface (HC-SR04 compatible)
// Provides functions for initializing and reading distance measurements
// Uses time-of-flight principle with ultrasonic sound waves
// Suitable for obstacle detection and distance ranging applications

// Function to initialize ultrasonic sensor pins
// Configures trigger pin as output and echo pin as input
// Must be called in setup() before measuring distances
// Uses pin definitions from pins.h
void initUltrasonic();

// Function to measure distance using ultrasonic sensor
// Sends ultrasonic pulse and measures echo return time
// Calculates distance based on speed of sound in air
// Includes timeout protection to prevent blocking
// Returns distance in centimeters (floating point for precision)
// Returns 999.0 if measurement fails or times out
float measureDistance();

#endif