#ifndef SERVERSETUP_H
#define SERVERSETUP_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <ESPmDNS.h>
#include "heltecSetup.h"
#include "webappSetup.h"

// -----------------------------------------------
// Global variables
#define SSID "ESP32-Access-Point"
#define PASSWORD "12345678"
extern IPAddress Ip;
extern IPAddress NMask;

extern String ipMsg;

// -----------------------------------------------
// Function prototypes
void setupAP();
void setupOTAasync();
void beginServer();
void printIP();
void setupDNS();

#endif