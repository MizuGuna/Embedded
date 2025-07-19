#include <Arduino.h>
#include <Crypto.h>
#include <LoRa.h>
#include <SPI.h>
#include <AES.h>
#include "lora_comm.h"
#include "constants.h"
#include "pins.h"

// AES encryption object for secure LoRa communication
AES128 aes;

// AES-128 encryption key (16 bytes)
// WARNING: In production, use a more secure key generation method
// This key must match exactly on both sender and receiver
byte key[16] = {'s','e','c','r','e','t','k','e','y','1','2','3','4','5','6','7'};

// Initialize LoRa module with encryption capabilities
// Configures LoRa radio parameters for optimal range and reliability
// Sets up AES encryption for secure message transmission
// Must be called before sending any messages
void initLoRa() {
    // Set up LoRa module pins as defined in pins.h
    LoRa.setPins(LoRa_SS, LoRa_RST, LoRa_DIO0);

    // Initialize LoRa radio on 433MHz frequency
    if (!LoRa.begin(433E6)) {
        Serial.println("LoRa init failed. Check wiring.");
        while (true);  // Halt execution if LoRa fails to initialize
    }

    // Configure LoRa parameters for maximum range and reliability
    LoRa.setSpreadingFactor(12);    // Maximum spreading factor for longest range
    LoRa.setSignalBandwidth(125E3); // 125kHz bandwidth for good sensitivity
    LoRa.setCodingRate4(8);         // Maximum error correction (4/8 coding rate)
    LoRa.setSyncWord(0x34);         // Sync word to distinguish our network
    LoRa.enableCrc();               // Enable CRC for error detection

    // Initialize AES encryption with the predefined key
    aes.setKey(key, sizeof(key));
    Serial.println("LoRa + AES Sender Ready");
}

// Send an encrypted message via LoRa
// Applies PKCS#7 padding to message, encrypts it using AES-128, then transmits
// Padding ensures message length is multiple of 16 bytes (AES block size)
// Prints both encrypted hex data and original message for debugging
// Returns true when message is successfully queued for transmission
bool sendMessage(const String& msg) {
    int msg_len = msg.length();

    // Calculate padding needed to make message length multiple of 16 (AES block size)
    int pad_len = 16 - (msg_len % 16);
    int total_len = msg_len + pad_len;

    // Create padded message buffer
    byte padded_msg[total_len];

    // Copy original message to padded buffer
    for (int i = 0; i < msg_len; i++) {
        padded_msg[i] = msg[i];
    }

    // Apply PKCS#7 padding - fill remaining bytes with pad length value
    // This allows the receiver to determine how much padding to remove
    for (int i = 0; i < pad_len; i++) {
        padded_msg[msg_len + i] = pad_len;
    }

    // Buffer to store encrypted data
    byte cipher[total_len];

    // Encrypt message in 16-byte blocks using AES-128
    for (int i = 0; i < total_len; i += 16) {
        aes.encryptBlock(cipher + i, padded_msg + i);
    }
    
    // Transmit encrypted data via LoRa
    LoRa.beginPacket();
    LoRa.write(cipher, total_len);
    LoRa.endPacket();

    // Debug output: print encrypted data in hexadecimal format
    Serial.print("Encrypted: ");
    for (int i = 0; i < total_len; i++) {
        Serial.print(cipher[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // Debug output: print original message
    Serial.println("Sent: " + msg);
    return true;
}