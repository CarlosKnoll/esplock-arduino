#include "heltecSetup.h"

ESP32Time rtc;
struct tm timeinfo;

String returnTime(){
  // String date = rtc.getTime("%d/%m/%y %T");
  String date = rtc.getTime("%m/%d/%y %T");
  Serial.println("[ESP32Time] getTime response: " + date);
  return date;
}

void timeUpdate(uint8_t *data){
        char *dados[3];
        char *values[7];
        char *ptr = NULL;
        int index = 0;

        ptr = strtok((char*)data, "=");
        while (ptr != NULL){
          dados[index] = ptr;
          index++;
          ptr = strtok(NULL, "=");
        }

        index = 0;
        ptr = strtok(dados[1], ",");
        while (ptr != NULL){
          values[index] = ptr;
          index++;
          ptr = strtok(NULL, ",");
        }

        int s = atoi(values[0]);
        int m = atoi(values[1]);
        int h = atoi(values[2]);
        int D = atoi(values[3]);
        int M = atoi(values[4]);
        int Y = atoi(values[5]);

        timeinfo = {0, 0, 0, 0, 0, 0, 0, 0, 0};      // Initalize to all 0's
        timeinfo.tm_year = Y - 1900;    // This is year-1900, so 121 = 2021
        timeinfo.tm_mon = M;
        timeinfo.tm_mday = D;
        timeinfo.tm_hour = h;
        timeinfo.tm_min = m;
        timeinfo.tm_sec = s;

	      rtc.setTimeStruct(timeinfo); 
        msgEspLock1();
}

void setupHeltec(){
  SPIFFS.begin(true);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
}

void printMessage( String string2print ){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 25, string2print);
  Heltec.display->display();
}

void printIP(){
  printMessage(ipMsg);
}

void msgEspLock1() {
  String string2print = "ESPLOCK";
  printMessage(string2print);
}