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

#include <Arduino.h>
#include <heltec.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

// -----------------------------------------------
// Function prototypes

void printMessage( String string2print );
void jsonHandler( String idTag );

// -----------------------------------------------

#include "webServerSetup.cpp"
#include "heltecDisplay.cpp"
#include "rfidSetup.cpp"
#include "jsonHandler.cpp"

// -----------------------------------------------

String ipString;

// -----------------------------------------------
// void handleNotFound()
// {
//     String message = "File Not Found\n\n";
//     message += "URI: ";
//     message += server.uri();
//     message += "\nMethod: ";
//     message += (server.method() == HTTP_GET) ? "GET" : "POST";
//     message += "\nArguments: ";
//     message += server.args();
//     message += "\n";

//     for (uint8_t i = 0; i < server.args(); i++)
//     {
//         message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//     }

    // server.send(404, "text/plain", message);
// }

// -----------------------------------------------
void msgEspLock1() {
    String string2print = "        ESPLOCK       ";
    printMessage(string2print);
}

// -----------------------------------------------
void setup(void)
{
    Serial.begin(115200);
    SPIFFS.begin(true);
    Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
    setupRFID();

    pinMode(led, OUTPUT);

    msgEspLock1();

    setupAP();
    setupOTAasync();
    setupWebPages();
    initWebSocket();
    beginServer();

    Serial.println("HTTP server started");
    ipString = "Success! " + ip;
}

// -----------------------------------------------
void loop(void)
{
    //server.handleClient();
    checkCard();
}
