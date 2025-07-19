#include <SPI.h>
#include <LoRa.h>
#include <Crypto.h>
#include <AES.h>
#include <ArduinoJson.h>

AES128 aes;
byte key[16] = { 's','e','c','r','e','t','k','e','y','1','2','3','4','5','6','7' }; //AES Secret Key

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial);
  delay(200);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check Wiring.");
    while (true);
  }

  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.setSyncWord(0x34);
  LoRa.enableCrc();

  aes.setKey(key, sizeof(key));
  Serial.println("LoRa + AES Receiver Ready");
}

void loop() {
  // put your main code here, to run repeatedly:

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Packet Size: ");
    Serial.println(packetSize);
    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    byte cipher[packetSize]; 
    int j = 0;
    while(LoRa.available() && j < packetSize) {
      cipher[j++] = LoRa.read();  //Reads message character by character
    }

    int total_len = packetSize;
    byte padded_msg[total_len];

    for (int i = 0; i < total_len; i += 16) {
      aes.decryptBlock(padded_msg + i, cipher + i);
    }

    int pad_len = (int)padded_msg[total_len - 1];
    int msg_len = total_len - pad_len;

    String msg = "";

    for (int i = 0; i < msg_len; i++) {
      msg += (char)padded_msg[i];
    }

    Serial.print("Received: ");

    for (int i = 0; i < total_len; i++) {
      Serial.print(cipher[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    Serial.print("Decrypted: ");
    Serial.println(msg);

    Serial.print("RSSI: ");
    Serial.print(rssi);
    Serial.println("dB");

    Serial.print("SNR: ");
    Serial.print(snr);
    Serial.println("dB\n");

    StaticJsonDocument<128> doc;
    doc["packet_size"] = packetSize;
    doc["message"] = msg;
    doc["rssi"] = rssi;
    doc["snr"] = snr;

    serializeJson(doc, Serial);
    Serial.println();
    LoRa.flush();
  }

}
