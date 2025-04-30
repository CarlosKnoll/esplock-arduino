#include "main.h"

String ipString;
esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
bool stayAwake = false;
// -----------------------------------------------
void setup(void)
{
    Serial.begin(115200);
    setupRFID();
    
    bool cardDetected = checkForCard();

    Serial.println("[WAKE] Wakeup reason: " + String(wakeup_reason));
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            stayAwake = false;
            Serial.println("[WAKE] Woke up by timer. Checking for card...");
            if (digitalRead(GPIO_NUM_12) == LOW) {
                stayAwake = true;
                initializeModules();
                delay(10);
                break;
            } else{
                if (!cardDetected) {
                    Serial.println("[SLEEP] No card detected. Going to sleep...");
                    sleepSetup();
                    break;
                } else {
                    Serial.println("[ACTIVE] Card found. Staying awake for now.");
                    initializeModules();
                    access();
                break;
            }

        default:
            stayAwake = true;
            Serial.println("[BOOT] Fresh boot or unknown wakeup. Init RFID and check.");
            initializeModules();
            printMessage("ESPLOCK reiniciado.\n Atualize o horario.");
            break;
        }
    }
}

// -----------------------------------------------
void loop(void){
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
    if (newUserTag != 1){
        user = access();
    }
    if (user != ""){
        notifyRFID("NewAccess",user);
        if (user != "FALSE"){
            notifyUserData("updateAccess", "add", "all", 0);
        }
        user = "";
    }
}

// -----------------------------------------------

void initializeModules(){
    setupHeltec();
    setupAP();
    setupOTAasync();
    setupWebPages();
    initWebSocket();
    beginServer();
    setupDNS();
    beginDB();

    pinMode(led, OUTPUT);
    msgEspLock1();

    Serial.println("HTTP server started");
    ipString = "Success! " + ipMsg;
}