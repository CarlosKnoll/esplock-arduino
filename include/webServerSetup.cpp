
// ------------------------------------------------------------------
// Set these to your desired credentials.
// const char* host = "esp32";
const char *ssid;
const char *password;
String ipMsg;

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

void printIP(){
  printMessage(ipMsg);
}

// ------------------------------------------------------------------
// Setup every webpage/file requests
// ------------------------------------------------------------------
void setupWebPages(){
  // General files
  server.on("/esplockstyle.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/esplockstyle.css", "text/css"); //css for html elements
  });
  server.on("/users.db", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/users.db", "text/plain"); //users database
  });

  // Main webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0; URL='./main'\"/>"); // Redirect of root to /main
  });
  server.on("/main", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockMain.html", "text/html"); //webpage HTML
  }); 

  // Users webpage
  server.on("/usuarios", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockUsers.html" ,"text/html"); //webpage HTML
  }); 
  server.on("/userdataHandler.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/userdataHandler.js", "text/javascript"); //webpage javascript
  });

  // Add new users webpage
  server.on("/novouser", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockNewUser.html", "text/html"); //webpage HTML
  }); 
  server.on("/addUser.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/addUser.js", "text/javascript"); //webpage javascript
  });

  // Placeholder for future webpages/functionalities
  server.on("/acessos", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "acessos");
  }); 
  server.on("/entrada", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "entrada");
  }); 
  server.on("/saida", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "saida");
  }); 
  server.on("/remove", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "remover usuario");
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

  String ip = IP.toString();
  ipMsg = "Success! " + ip;
  printIP();
  
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
// Websockets
// -----------------------------------------------

// Send responses to client-side javascript

// Users webpage
void notifyUserData (String response){ //Sends back raw db data
  Serial.println("data: " + response);
  ws.textAll(String (response));
}

// New user webpage
void notifyRFID(String uid){
  Serial.println("Notifying rfid: " + uid);
  ws.textAll(uid);
}

void notifyError(){
  ws.textAll("unavailable");
}

void notifyNewUser(){
  ws.textAll("clearFields");
}

// Websocket Handler
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    //Test for messages from users webpage
    if (strstr((char*)data, "populateUsers") != NULL) { //If message contains populateUsers
      String message = (char*)data;
      Serial.println("Got the following message data: " + message);
      String response = getData();
      notifyUserData(response);
    }

    if (strstr((char*)data, "removeUser") != NULL) { //If message contains removeUser
      String message = (char*)data;
      Serial.println("Got the following message data: " + message);
      removeUser(message.substring(11).toInt());
      String response = getData();
      notifyUserData(response);
    }

    //Test for messages from add new users webpage
    if (strstr((char*)data, "addUser") != NULL) { //If message contains addUser
      String message = (char*)data;
      Serial.println("Got the following message data: " + message);
      String newUser = message.substring(message.indexOf("User=") + 5, message.indexOf("ID=") - 1);
      String newID = message.substring(message.indexOf("ID=") + 3);
      int availableTag = checkTag(newID);
      if (availableTag == 0){
        notifyError();
      }
      else{
        addUser(newUser, newID); // TODO: define it for db implementation
        String response = getData();
        notifyUserData(response);
        notifyNewUser();
      }
    }
    if (strcmp((char*)data, "readRFID") == 0) { //If message = readRFID  
      updateStatus();
    }
    
    if (strcmp((char*)data, "cancelRFID") == 0) { //If message = cancelRFID
      updateStatus();
      String uid = "cancel";
      notifyRFID(uid);
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