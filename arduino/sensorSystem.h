#ifndef SENSOR_SYSTEM_H
#define SENSOR_SYSTEM_H

#include <Arduino.h>

void initSensorSystem();
String getSensorDataString();
void calibrateORP(int value);
void clearORPCalibration();

#endif
