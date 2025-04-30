#ifndef HELTECSETUP_H
#define HELTECSETUP_H

#include <Arduino.h>
#include <ESP32Time.h>
#include <heltec.h>
#include <SPIFFS.h>
#include "serverSetup.h"

// -----------------------------------------------
// Global variables

extern ESP32Time rtc;
extern struct tm timeinfo;

// -----------------------------------------------
// Function prototypes

String returnTime();
void timeUpdate(uint8_t *data);
void setupHeltec();
void printMessage( String string2print );
void printIP();
void msgEspLock1();

#endif
