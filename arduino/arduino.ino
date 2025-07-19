/*
 * Water Quality Monitoring System with LoRa Communication
 *
 * This is the main Arduino sketch for a water quality monitoring system that:
 * - Reads multiple sensor values (temperature, pH, TDS, ORP)
 * - Sends encrypted data via LoRa radio
 * - Supports ORP sensor calibration via serial commands
 *
 * Hardware Components:
 * - Temperature sensor
 * - pH sensor
 * - TDS (Total Dissolved Solids) sensor
 * - ORP (Oxidation Reduction Potential) sensor
 * - LoRa radio module for wireless transmission
 *
 * Serial Commands:
 * - "CAL,xxx" - Calibrate ORP sensor to value xxx
 * - "CAL,CLEAR" - Clear ORP calibration data
 */

#include "sensorSystem.h"    // Sensor reading and management functions
#include "lora_comm.h"       // LoRa communication and encryption
#include "constants.h"       // System constants and configuration
#include "pins.h"            // Pin definitions for hardware connections

// === Serial Command Processing Variables ===
const uint8_t bufferlen = 32;           // Maximum length for serial commands
char user_data[bufferlen];              // Buffer to store incoming serial data
uint8_t user_bytes_received = 0;        // Number of bytes received from serial

// Function prototype for command parsing
void parse_cmd(char* string);

void setup() {
  // Initialize serial communication for debugging and calibration commands
  Serial.begin(9600);
  delay(200);  // Allow serial to stabilize

  // Display calibration instructions to user
  Serial.println(F("Use command \"CAL,xxx\" to calibrate ORP to value xxx"));
  Serial.println(F("\"CAL,CLEAR\" clears ORP calibration"));

  // Initialize all sensor systems (pH, TDS, ORP, temperature)
  initSensorSystem();
  Serial.println("Sensor system ready.");

  // Initialize LoRa radio communication with encryption
  initLoRa();
}

void loop() {
  // === Handle Serial Calibration Commands ===
  // Check if calibration command received via serial port
  if (Serial.available() > 0) {
    // Read command until carriage return (ASCII 13)
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));
  }

  // Process received command if available
  if (user_bytes_received) {
    parse_cmd(user_data);                    // Parse and execute calibration command
    user_bytes_received = 0;                 // Reset byte counter
    memset(user_data, 0, sizeof(user_data)); // Clear command buffer
  }

  // === Sensor Data Collection and Transmission ===
  // Read all sensor values and format as string (e.g., "Temp:25.5|pH:7.2|TDS:350|ORP:200")
  String data = getSensorDataString();

  // Encrypt and transmit sensor data via LoRa
  sendMessage(data);

  // Wait 3 seconds before next reading cycle
  // This prevents overwhelming the LoRa network and allows time for data processing
  delay(3000);
}

/*
 * Parse and execute calibration commands from serial input
 *
 * Supported commands:
 * - "CAL,xxx" where xxx is a numeric value to calibrate ORP sensor
 * - "CAL,CLEAR" to clear existing ORP calibration
 *
 * Args:
 *   string: Null-terminated command string from serial input
 */
void parse_cmd(char* string) {
  // Convert command to uppercase for consistent parsing
  strupr(string);
  String cmd = String(string);

  // Check if command starts with "CAL" (calibration command)
  if (cmd.startsWith("CAL")) {
    // Find comma separator between command and parameter
    int index = cmd.indexOf(',');

    if (index != -1) {
      // Extract parameter after comma
      String param = cmd.substring(index + 1);

      if (param.equals("CLEAR")) {
        // Clear existing ORP calibration data
        clearORPCalibration();
        Serial.println("CALIBRATION CLEARED");
      } else {
        // Calibrate ORP sensor to specified value
        int cal_param = param.toInt();    // Convert parameter to integer
        calibrateORP(cal_param);          // Perform calibration
        Serial.println("ORP CALIBRATED");
      }
    }
  }
  // Note: Invalid commands are silently ignored
}