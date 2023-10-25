
// ------------------------------------------------------------------
// Set these to your desired credentials.
// const char* host = "esp32";
const char *ssid;
const char *password;
String ip;

size_t content_len;

// Set static IP:
// IPAddress local_IP(192, 168, 200, 95);  // Set your Static IP address
// IPAddress gateway(192, 168, 200, 1);    // Set your Gateway IP address
// IPAddress subnet(255, 255, 255, 0);     // IP mask
// IPAddress primaryDNS(8, 8, 8, 8);       //optional
// IPAddress secondaryDNS(8, 8, 4, 4);     //optional

// Objects
// WiFiServer server(80);
// WebServer server(80);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ------------------------------------------------------------------
// Test page
int ledState = LOW;
int displayState = LOW;
#define led     25
String msg="";

#include "bmp.h"
// ------------------------------------------------------------------

String processor(const String& var){
  Serial.println(var);
  if(var == "STATELED"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  if(var == "STATEDISPLAY"){
    if (displayState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

// ------------------------------------------------------------------

void printIP( String string2print ){
  printMessage(string2print);
}

void setupWebPages(){
  server.on("/esplockstyle.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/esplockstyle.css", "text/css");
  });
  server.on("/wsHandler.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/wsHandler.js", "text/javascript");
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0; URL='./main'\"/>");
  });
  server.on("/main", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockMain.html", "text/html");
  }); 
  server.on("/acessos", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "acessos");
  }); 
  server.on("/entrada", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "entrada");
  }); 
  server.on("/usuarios", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockUsers.html" ,"text/html");
  }); 
  server.on("/saida", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "saida");
  }); 
  server.on("/remove", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "remover usuario");
  }); 



  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/test.html", "text/html", false, processor);
  });
}

// ------------------------------------------------------------------
// Setup as AP
// ------------------------------------------------------------------
void setupAP(){
  ssid = "ESP32-Access-Point";
  password = "87654321"; 

  Serial.print("Setting AP (Access Point)…");
  String string2print = "Setting AP (Access Point)…";
  printMessage(string2print);
  
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  ip = IP.toString();
  string2print = "Success! " + ip;
  printIP( string2print );
  
}

// ------------------------------------------------------------------
void setupOTAasync(){
  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
}

// ------------------------------------------------------------------
void beginServer(){
  server.begin();
}

// -----------------------------------------------
// Websocket handler
// -----------------------------------------------

void notifyClients() {
  ws.textAll(String (ledState) + String (displayState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggleLED") == 0) {
      ledState = !ledState;
      digitalWrite(led, ledState);
      notifyClients();
    }
    if (strcmp((char*)data, "toggleDisplay") == 0) {
      displayState = !displayState;
      Heltec.display->clear();
      if (displayState == LOW){
        printIP("Success! " + ip);
      }
      if (displayState == HIGH){
        Heltec.display->drawXbm(34, 14, bird_width, bird_height, bird_bits);
      }
      Heltec.display->display();
      notifyClients();
    }
    if (strcmp((char*)data, "getMessage") == 0) {
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// -----------------------------------------------