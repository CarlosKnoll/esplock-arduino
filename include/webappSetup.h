#ifndef WEBAPPSETUP_H
#define WEBAPPSETUP_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPAsyncWebServer.h>


// -----------------------------------------------
// Global variables
#define led 25

extern size_t content_len;

extern AsyncWebServer server;
extern AsyncWebSocket ws;

extern int ledState;
extern int flagTime;

extern String msg;


// -----------------------------------------------
// Function prototypes
void setupWebPages();
void notifyUserData (String info, String response, String type, uint32_t client);
void notifyRFID(String info, String uid);
void notifyError();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, uint32_t client);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
              void *arg, uint8_t *data, size_t len);
void initWebSocket();



#endif