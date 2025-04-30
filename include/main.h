#ifndef MAIN_H
#define MAIN_H

// -----------------------------------------------
// Libraries needed for the project

#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include <Update.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SPI.h>
#include <MFRC522.h>
#include <sqlite3.h>
#include <ESP32Time.h>
#include <WiFiUdp.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"

// -----------------------------------------------
// Global variables

extern String ipString;

// -----------------------------------------------
// Inclusion of custom header files

#include "heltecSetup.h"
#include "serverSetup.h"
#include "webappSetup.h"
#include "rfidSetup.h"
#include "sqliteSetup.h"
#include "sleepSetup.h"

// -----------------------------------------------
// Function prototypes
void initializeModules();

#endif