#include "main.h"

size_t content_len;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int ledState = LOW;
bool flagTime = false;
String msg="";

// ------------------------------------------------------------------
// Setup every webpage/file requests
// ------------------------------------------------------------------
void setupWebPages(){
  Serial.println("Setting up web pages...");
  // General files
  server.on("/esplockstyle.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/esplockstyle.css", "text/css"); //css for html elements
  });
  server.on("/users.db", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/users.db", "text/plain"); //users database
  });
  server.on("/access.csv", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/access.csv", "text/plain", true); //csv for downloading the database
  }); 

  // Main webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"0; URL='./main'\"/>"); // Redirect of root to /main
  });
  server.on("/main", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockMain.html", "text/html"); //webpage HTML
  }); 
  server.on("/esplockMain.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockMain.js", "text/javascript"); //webpage HTML
  }); 

  // Users webpage
  server.on("/usuarios", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockUsers.html" ,"text/html"); //webpage HTML
  }); 
  server.on("/esplockUsers.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/esplockUsers.js", "text/javascript"); //webpage javascript
  });

  // Add new users webpage
  server.on("/novouser", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockAddUser.html", "text/html"); //webpage HTML
  }); 
  server.on("/esplockAddUser.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/esplockAddUser.js", "text/javascript"); //webpage javascript
  });

  // Access history webpage
  server.on("/acessos", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockAccess.html", "text/html");
  }); 
  server.on("/esplockAccess.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockAccess.js", "text/javascript");
  }); 

  // Access webpage
  server.on("/entrada", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/esplockNewAccess.html", "text/html");
  }); 
  server.on("/esplockNewAccess.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/esplockNewAccess.js", "text/javascript");
  }); 

  // Placeholder for future webpages/functionalities
  server.on("/saida", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "saida");
  });
}

// -----------------------------------------------
// Websockets
// -----------------------------------------------

// Send responses to client-side javascript

// Users webpage
void notifyUserData (String info, String response, String type, uint32_t client){ //Sends back raw db data
  String message = String (info) + "#" + String (response);
  if (type == "individual"){
    Serial.println("[DEBUG] Sending message to the client: " + String(client));
    ws.text(client,message);
  }
  else{
  ws.textAll(message);
  Serial.println(message);
  }
}

// New user webpage
void notifyRFID(String info, String uid){
  ws.textAll(String (info) + "=" + String (uid));
}

void notifyError(){
  ws.textAll("unavailable");
}

// Websocket Handler
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, uint32_t client) {
  Serial.println("Client: " + String(client));
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    //Test for messages from users webpage
    if (strstr((char*)data, "populateUsers") != NULL) { //If message contains populateUsers
      String message = (char*)data;
      String numPage = message.substring(message.indexOf(";numPage=") + 9);
      String response = getData(numPage, "users");
      notifyUserData("users", response, "individual", client);
    }

    if (strstr((char*)data, "removeUser") != NULL) { //If message contains removeUser
      String message = (char*)data;
      int removalId = message.substring(11, message.indexOf(";numPage=")).toInt();
      removeUser(removalId);
      notifyUserData("updateUsers", "add", "all", client);
    }

    //Test for messages from add new users webpage
    if (strstr((char*)data, "addUser") != NULL) { //If message contains addUser
      String message = (char*)data;
      String newUser = message.substring(message.indexOf("User=") + 5, message.indexOf("ID=") - 1);
      String newID = message.substring(message.indexOf("ID=") + 3);
      int availableTag = checkTag(newID);
      if (availableTag == 0){
        notifyError();
      }
      else{
        addUser(newUser, newID);
        notifyUserData("success", "add", "all", client);
      }
    }

    //Test for messages from access webpage
    if (strstr((char*)data, "populateAccess") != NULL) { //If message contains populateUsers
      String message = (char*)data;
      String numPage = message.substring(message.indexOf(";numPage=") + 9);
      String response = getData(numPage, "access");
      notifyUserData("access", response, "individual", client);
    }

    if (strcmp((char*)data, "clear") == 0) { //If message equals clear
      Serial.println(String((char*)data));
      clearDB();
      notifyUserData("access", "empty", "all", client);
    }

    if (strcmp((char*)data, "get") == 0) { //If message equals get
      Serial.println("[WEBAPP] Received WS message: " + String((char*)data));
      Serial.println("[DEBUG] Received WS message from client: " + String(client));
      getDBAsync(client);
      // String csvData = getDB();
      // Serial.println("[DEBUG] CSV Data: " + csvData + "; length: " + String(csvData.length()) + "; Returning data.");
      // notifyUserData("csv", csvData, "individual", client);
    }

    //Test for messages regarding RFID readings
    if (strcmp((char*)data, "readRFID") == 0) { //If message = readRFID  
      String message = (char*)data;
      updateStatus("");
    }
    if (strcmp((char*)data, "cancelRFID") == 0) { //If message = cancelRFID
      String message = (char*)data;
      String uid = "";
      updateStatus(uid);
      notifyRFID("cancel", uid);
    }

    //Test for time update
    if (strstr((char*)data, "epoch") != NULL) { //If message contains epoch
      if (flagTime == false){
        timeUpdate(data);
        flagTime = true;
        printMessage("ESPLOCK");
      } 
    }

    //Test for deep sleep request
    if (strcmp((char*)data, "deepSleep") == 0) { //If message = deepSleep
      sleepSetup();
      // esp_light_sleep_start();
      // esp_restart();
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
      handleWebSocketMessage(arg, data, len, client->id());
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  Serial.println("Setting up WebSocket...");
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}