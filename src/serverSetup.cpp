#include "serverSetup.h"

IPAddress Ip(192, 168, 4, 1);
IPAddress NMask(255, 255, 255, 0);
String ipMsg;

void setupAP(){  
  Serial.print("Setting AP (Access Point)…");
  String string2print = "Setting AP (Access Point)…";
  printMessage(string2print);
  
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);
  delay(100);

  WiFi.softAPConfig(Ip, Ip, NMask);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  String ip = IP.toString();
  ipMsg = "Success! " + ip;
  printIP();
}
  
void setupOTAasync(){
  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  Serial.println("OTA started");
}
  
void beginServer(){
  Serial.println("Starting server...");
  flagTime = 0;
  server.begin();
}
  