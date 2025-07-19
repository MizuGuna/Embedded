#include "sensorSystem.h"
#include "pins.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifdef USE_PULSE_OUT
  #include "orp_iso_surveyor.h"
  Surveyor_ORP_Isolated ORP = Surveyor_ORP_Isolated(A2);
#else
  #include "orp_surveyor.h"
  Surveyor_ORP ORP = Surveyor_ORP(A2);
#endif

// === Sensor Setup ===
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void initSensorSystem() {
  sensors.begin();
  ORP.begin();  // Load ORP calibration
}

float readTemperature() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  return (temp == DEVICE_DISCONNECTED_C) ? 25.0 : temp;
}

float readPH() {
  int phRaw = analogRead(PH_PIN);
  float voltagePH = phRaw * (5.0 / 1023.0);
  return 7 + ((2.5 - voltagePH) / 0.18);
}

float readTDS() {
  int tdsRaw = analogRead(TDS_PIN);
  float voltageTDS = tdsRaw * (5.0 / 1023.0);
  return (133.42 * pow(voltageTDS, 3)
         - 255.86 * pow(voltageTDS, 2)
         + 857.39 * voltageTDS) * 0.5;
}

int readORP() {
  return ORP.read_orp();
}

void calibrateORP(int value) {
  ORP.cal(value);
}

void clearORPCalibration() {
  ORP.cal_clear();
}

String getSensorDataString() {
  float temperature = readTemperature();
  float pH = readPH();
  float tds = readTDS();
  int orp = readORP();

  String data = "Temp:" + String(temperature, 2);
  data += " | pH:" + String(pH, 2);
  data += " | TDS:" + String(tds, 1);
  data += " | ORP:" + String(orp);

  return data;
}
