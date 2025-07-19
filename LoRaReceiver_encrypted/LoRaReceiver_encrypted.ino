/*
 * LoRa Receiver with AES-128 Decryption
 *
 * This program receives encrypted messages via LoRa radio communication
 * and decrypts them using AES-128 encryption. The decrypted messages
 * along with signal quality metrics are output as JSON format.
 *
 * Hardware Requirements:
 * - Arduino compatible board (ESP32, Arduino Uno, etc.)
 * - LoRa module (SX1276/SX1278 based)
 * - Proper wiring for SPI communication
 *
 * Dependencies:
 * - LoRa library for radio communication
 * - Crypto library for AES encryption/decryption
 * - ArduinoJson library for JSON output formatting
 */

#include <SPI.h>          // SPI communication for LoRa module
#include <LoRa.h>         // LoRa radio communication library
#include <Crypto.h>       // Cryptographic functions base library
#include <AES.h>          // AES encryption/decryption implementation
#include <ArduinoJson.h>  // JSON serialization for structured output

// Initialize AES-128 cipher object
AES128 aes;

// AES-128 encryption key (16 bytes)
// NOTE: In production, this key should be securely generated and stored
// The key must match exactly between transmitter and receiver
byte key[16] = { 's','e','c','r','e','t','k','e','y','1','2','3','4','5','6','7' };

void setup() {
  // Initialize serial communication for debugging and output
  Serial.begin(9600);

  // Wait for serial port to be ready (important for some boards like Leonardo)
  while (!Serial);
  delay(200);  // Additional delay to ensure stable communication

  // Initialize LoRa module at 433 MHz frequency
  // Common frequencies: 433E6 (433 MHz), 868E6 (868 MHz), 915E6 (915 MHz)
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check Wiring.");
    while (true);  // Halt execution if LoRa initialization fails
  }

  // Configure LoRa radio parameters for optimal performance
  // These settings should match the transmitter configuration

  LoRa.setSpreadingFactor(12);    // SF12 = longest range, slowest data rate (293 bps)
                                  // Range: SF7 (fastest) to SF12 (longest range)

  LoRa.setSignalBandwidth(125E3); // 125 kHz bandwidth
                                  // Options: 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, 500E3

  LoRa.setCodingRate4(8);         // Coding rate 4/8 (highest error correction)
                                  // Options: 5 (4/5), 6 (4/6), 7 (4/7), 8 (4/8)

  LoRa.setSyncWord(0x34);         // Sync word for network identification
                                  // Devices with different sync words won't communicate

  LoRa.enableCrc();               // Enable CRC for error detection

  // Initialize AES cipher with the encryption key
  aes.setKey(key, sizeof(key));

  Serial.println("LoRa + AES Receiver Ready");
}

void loop() {
  // Check if a LoRa packet has been received
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    // Packet received - start processing
    Serial.print("Packet Size: ");
    Serial.println(packetSize);

    // Get signal quality metrics
    int rssi = LoRa.packetRssi();   // Received Signal Strength Indicator (dBm)
    float snr = LoRa.packetSnr();   // Signal-to-Noise Ratio (dB)

    // Read the encrypted data from LoRa buffer
    byte cipher[packetSize]; 
    int j = 0;

    // Read each byte of the received packet
    while(LoRa.available() && j < packetSize) {
      cipher[j++] = LoRa.read();
    }

    // Prepare buffer for decrypted data
    int total_len = packetSize;
    byte padded_msg[total_len];

    // Decrypt the message in 16-byte blocks (AES block size)
    // AES operates on fixed 16-byte blocks, so we process the entire message
    // in chunks of 16 bytes
    for (int i = 0; i < total_len; i += 16) {
      aes.decryptBlock(padded_msg + i, cipher + i);
    }

    // Remove PKCS#7 padding from decrypted message
    // The last byte indicates how many padding bytes were added
    int pad_len = (int)padded_msg[total_len - 1];
    int msg_len = total_len - pad_len;

    // Convert decrypted bytes to string
    String msg = "";
    for (int i = 0; i < msg_len; i++) {
      msg += (char)padded_msg[i];
    }

    // Display received encrypted data in hexadecimal format
    Serial.print("Received: ");
    for (int i = 0; i < total_len; i++) {
      Serial.print(cipher[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Display decrypted message
    Serial.print("Decrypted: ");
    Serial.println(msg);

    // Display signal quality information
    Serial.print("RSSI: ");
    Serial.print(rssi);
    Serial.println("dB");

    Serial.print("SNR: ");
    Serial.print(snr);
    Serial.println("dB\n");

    // Create JSON object for structured output
    // This format makes it easy to parse the data in other applications
    StaticJsonDocument<128> doc;
    doc["packet_size"] = packetSize;  // Size of received packet in bytes
    doc["message"] = msg;             // Decrypted message content
    doc["rssi"] = rssi;               // Signal strength
    doc["snr"] = snr;                 // Signal quality

    // Output JSON to serial port
    serializeJson(doc, Serial);
    Serial.println();

    // Clear any remaining data in LoRa buffer
    LoRa.flush();
  }

  // Loop continues to check for new packets
  // No delay needed as parsePacket() is non-blocking
}