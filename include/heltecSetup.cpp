// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
ESP32Time rtc;
struct tm timeinfo;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 3600;

void setupTime(){
  printMessage("Fetching current time...");
  WiFi.begin("Paulo Knoll", "2444666668888888");

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  /*---------set with NTP---------------*/
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if (getLocalTime(&timeinfo)){
    rtc.setTimeStruct(timeinfo); 
  }
  Serial.println(rtc.getTimeDate(true));

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

String returnTime(){
  String date = rtc.getTime("%d/%m/%y %H:%M:%S");
  Serial.println(date);
  return date;
}

void setupHeltec(){
  SPIFFS.begin(true);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  setupTime();
}

void printMessage( String string2print ){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 20, "======================");
  Heltec.display->drawString(0, 30, string2print);
  Heltec.display->drawString(0, 40, "======================");
  Heltec.display->display();
}