#ifndef RFIDSETUP_H
#define RFIDSETUP_H

#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include "webappSetup.h"
#include "sqliteSetup.h"
#include "heltecSetup.h"

// -----------------------------------------------
// Global variables
#define RFID_SDA 5
#define RFID_SCK 18
#define RFID_MOSI 23
#define RFID_MISO 19
#define RFID_RST 22
// #define RFID_IRQ 2

extern unsigned long UID[3];
extern String id;
extern unsigned long lastInterruptTime;
extern MFRC522 mfrc522;  // Create MFRC522 instance

extern int newUserTag;
extern int readUser;
extern String newUser;
extern String user;

// -------------------------------------------------
// Function prototypes
void setupRFID();
void printHex(byte *buffer, byte bufferSize);
void checkCard();
String newCard();
bool checkForCard();
void updateStatus( String message );
String access();

#endif