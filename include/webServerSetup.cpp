
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
int ledState = LOW;
int flagTime = 0;
#define led     25
String msg="";


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
  flagTime = 0;
  server.begin();
}

// -----------------------------------------------
// Websockets
// -----------------------------------------------

// Send responses to client-side javascript

// Users webpage
void notifyUserData (String info, String response, String type, uint32_t client){ //Sends back raw db data
  String message = String (info) + "#" + String (response);
  if (type == "individual"){
    ws.text(client,message);
  }
  else{
  ws.textAll(message);
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
      notifyUserData("access", "", "all", client);
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
      if (flagTime == 0){
        timeUpdate(data);
        flagTime = 1;
      } 
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
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// -----------------------------------------------