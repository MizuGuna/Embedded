#ifndef LORA_COMM_H
#define LORA_COMM_H

#include <Arduino.h>
#include <LoRa.h>
#include <Crypto.h>
#include <AES.h>

// Header file for encrypted LoRa communication interface
// Provides functions for secure wireless message transmission using LoRa + AES-128 encryption
// Handles initialization, message padding, encryption, and transmission

// External declaration of AES encryption object
// Used for encrypting messages before LoRa transmission
extern AES128 aes;

// External declaration of AES encryption key
// 16-byte key used for AES-128 encryption
// Must be identical on both sender and receiver for successful decryption
extern byte key[16];

// Function to initialize LoRa module with encryption
// Configures LoRa radio parameters for maximum range and reliability
// Sets up AES encryption with predefined key
// Must be called in setup() before sending messages
void initLoRa();

// Function to send encrypted message via LoRa
// Automatically handles message padding to AES block size (16 bytes)
// Encrypts message using AES-128 before transmission
// Provides debug output showing both encrypted data and original message
// Parameter: msg - String message to encrypt and send
// Returns: true when message is successfully queued for transmission
bool sendMessage(const String& msg);

#endif