#include "sensorSystem.h"
#include "pins.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Conditional compilation for ORP sensor type
// USE_PULSE_OUT macro determines which ORP sensor library to use
#ifdef USE_PULSE_OUT
  #include "orp_iso_surveyor.h"
  Surveyor_ORP_Isolated ORP = Surveyor_ORP_Isolated(A2);  // Isolated ORP sensor on analog pin A2
#else
  #include "orp_surveyor.h"
  Surveyor_ORP ORP = Surveyor_ORP(A2);  // Standard ORP sensor on analog pin A2
#endif

// OneWire bus setup for Dallas temperature sensor
// Allows multiple DS18B20 sensors on single data line
OneWire oneWire(ONE_WIRE_BUS);

// Dallas temperature sensor library object
// Handles temperature sensor discovery and reading
DallasTemperature sensors(&oneWire);

// Initialize all water quality sensors
// Sets up temperature sensor bus and loads ORP sensor calibration from EEPROM
// Must be called in setup() before reading sensor values
void initSensorSystem() {
  sensors.begin();  // Initialize OneWire temperature sensors
  ORP.begin();      // Initialize ORP sensor and load calibration data
}

// Read water temperature from DS18B20 sensor
// Requests temperature reading and waits for conversion
// Returns 25.0°C as default if sensor is disconnected or fails
// Temperature is returned in Celsius
float readTemperature() {
  sensors.requestTemperatures();  // Initiate temperature conversion
  float temp = sensors.getTempCByIndex(0);  // Read first sensor on bus

  // Return default temperature if sensor is disconnected
  return (temp == DEVICE_DISCONNECTED_C) ? 25.0 : temp;
}

// Read pH value from analog pH sensor
// Converts analog voltage to pH scale (0-14)
// Uses linear conversion: pH = 7 + ((2.5V - measured_voltage) / 0.18)
// Assumes pH 7.0 = 2.5V, with 0.18V per pH unit sensitivity
float readPH() {
  int phRaw = analogRead(PH_PIN);  // Read raw ADC value (0-1023)
  float voltagePH = phRaw * (5.0 / 1023.0);  // Convert to voltage (0-5V)

  // Convert voltage to pH using calibration formula
  // pH 7.0 corresponds to 2.5V, with ~0.18V per pH unit
  return 7 + ((2.5 - voltagePH) / 0.18);
}

// Read Total Dissolved Solids (TDS) from analog sensor
// Converts analog voltage to TDS in ppm (parts per million)
// Uses cubic polynomial calibration curve for accuracy
// Includes temperature compensation factor (0.5)
float readTDS() {
  int tdsRaw = analogRead(TDS_PIN);  // Read raw ADC value (0-1023)
  float voltageTDS = tdsRaw * (5.0 / 1023.0);  // Convert to voltage (0-5V)

  // Apply cubic polynomial calibration curve
  // Coefficients determined through sensor calibration process
  return (133.42 * pow(voltageTDS, 3)
         - 255.86 * pow(voltageTDS, 2)
         + 857.39 * voltageTDS) * 0.5;  // 0.5 factor for temperature compensation
}

// Read Oxidation Reduction Potential (ORP) from sensor
// Returns ORP value in millivolts (mV)
// Uses calibrated sensor library for accurate readings
// Positive values indicate oxidizing conditions, negative = reducing
int readORP() {
  return ORP.read_orp();  // Library handles voltage conversion and calibration
}

// Calibrate ORP sensor to known standard solution
// Performs single-point calibration using reference solution
// Calibration value is stored in EEPROM for persistence
// Common calibration solutions: +200mV, +400mV, +475mV
void calibrateORP(int value) {
  ORP.cal(value);  // Store calibration offset in EEPROM
}

// Clear ORP sensor calibration data
// Removes calibration offset from EEPROM
// Sensor will return raw, uncalibrated readings until re-calibrated
void clearORPCalibration() {
  ORP.cal_clear();  // Erase calibration data from EEPROM
}

// Create formatted string with all sensor readings
// Combines temperature, pH, TDS, and ORP into single pipe-delimited string
// Used for logging and LoRa transmission of sensor data
// Format: "Temp:XX.XX | pH:X.XX | TDS:XXX.X | ORP:XXX"
String getSensorDataString() {
  // Read all sensor values
  float temperature = readTemperature();
  float pH = readPH();
  float tds = readTDS();
  int orp = readORP();

  // Format data string with appropriate decimal places
  String data = "Temp:" + String(temperature, 2);  // 2 decimal places for temperature
  data += " | pH:" + String(pH, 2);                // 2 decimal places for pH
  data += " | TDS:" + String(tds, 1);              // 1 decimal place for TDS
  data += " | ORP:" + String(orp);                 // Integer value for ORP

  return data;
}