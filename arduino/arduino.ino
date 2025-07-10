#include "sensorSystem.h"
#include "lora_comm.h"
#include "constants.h"
#include "pins.h"

const uint8_t bufferlen = 32;
char user_data[bufferlen];
uint8_t user_bytes_received = 0;

void parse_cmd(char* string);

void setup() {
  Serial.begin(9600);
  delay(200);

  Serial.println(F("Use command \"CAL,xxx\" to calibrate ORP to value xxx"));
  Serial.println(F("\"CAL,CLEAR\" clears ORP calibration"));

  initSensorSystem();

  Serial.println("Sensor system ready.");

  initLoRa();
}

void loop() {
  // Handle Serial Calibration Commands
  if (Serial.available() > 0) {
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));
  }
  if (user_bytes_received) {
    parse_cmd(user_data);
    user_bytes_received = 0;
    memset(user_data, 0, sizeof(user_data));
  }

  // Get and print data
  String data = getSensorDataString();
  sendMessage(data);

  delay(3000);
}

void parse_cmd(char* string) {
  strupr(string);
  String cmd = String(string);
  if (cmd.startsWith("CAL")) {
    int index = cmd.indexOf(',');
    if (index != -1) {
      String param = cmd.substring(index + 1);
      if (param.equals("CLEAR")) {
        clearORPCalibration();
        Serial.println("CALIBRATION CLEARED");
      } else {
        int cal_param = param.toInt();
        calibrateORP(cal_param);
        Serial.println("ORP CALIBRATED");
      }
    }
  }
}
