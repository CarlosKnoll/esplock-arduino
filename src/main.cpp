/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   C:\Users\mauri\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\WebServer\examples\AdvancedWebServer
   Kit Mauricio TTGO MAC 24-6F-28-25-50-F8
*/

// STRUCT MATRIZ ESTÁTICA? 

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
#include <NTPClient.h>
#include <WiFiUdp.h>

// -----------------------------------------------
// Function prototypes

void printMessage( String string2print );
void dbCheck( String id );
void printIP();
void removeUser( int idDelete );
void addUser(String usuario, String id);
String newCard();
void updateStatus(String message);
void updateMode(String message);
int checkTag(String newTag);
String getData();
String getAccess();
String access();
String dbAccessCheck(String tag);
String returnTime();

// -----------------------------------------------

#include "webServerSetup.cpp"
#include "heltecSetup.cpp"
#include "rfidSetup.cpp"
#include "sqliteHandler.cpp"

// -----------------------------------------------

String ipString;
int newUserTag = LOW;
int readUser = LOW;
String newUser = "";
String user = "";

// -----------------------------------------------
void msgEspLock1() {
    String string2print = "        ESPLOCK       ";
    printMessage(string2print);
}

void updateStatus( String message ){
    if (message == "cancel"){
        readUser = LOW;
    }
    else{
        newUserTag = !newUserTag;
    }
}

void updateMode( String message ){
    if (message == "cancel"){
        readUser = LOW;
    }
    else{
        readUser = !readUser;
    }
}

// -----------------------------------------------
void setup(void)
{
    Serial.begin(115200);
    setupRFID();
    setupHeltec();

    pinMode(led, OUTPUT);

    msgEspLock1();

    setupAP();
    setupOTAasync();
    setupWebPages();
    initWebSocket();
    beginServer();
    beginDB();

    Serial.println("HTTP server started");
    ipString = "Success! " + ipMsg;
    
}

// -----------------------------------------------
void loop(void){
    if (newUserTag == 0 && readUser == 0){
        checkCard();
    }

    // If set to add new user
    if (newUserTag == 1){
        newUser = newCard();
    }
    if (newUser != ""){
        notifyRFID("newUserId",newUser);
        newUser = "";
        updateStatus("");
    }

    // If set to read user for access
    if (readUser == 1){
        user = access();
    }
    if (user != ""){
        notifyRFID("NewAccess",user);
        user = "";
        updateMode("");
    }
}
