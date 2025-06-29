#include "main.h"

String ipString;
esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
bool stayAwake = false;
// -----------------------------------------------
void setup(void)
{
    pinMode(wakeMonitor, OUTPUT);
    digitalWrite(wakeMonitor, HIGH);
    Serial.begin(115200);
    setupRFID();
    
    bool cardDetected = checkForCard();

    Serial.println("[BOOT] Wakeup reason: " + String(wakeup_reason));
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            stayAwake = false;
            Serial.println("[WAKE] Woke up by timer. Checking for card...");
            if (digitalRead(GPIO_NUM_13) == LOW) {
                stayAwake = true;
                initializeModules(0);
                delay(10);
                break;
            } else{
                if (!cardDetected) {
                    Serial.println("[WAKE] No card detected. Going to sleep...");
                    sleepSetup();
                    break;
                } else {
                    Serial.println("[WAKE] Card found. Staying awake.");
                    initializeModules(1);
                    access();
                break;
            }

        default:
            stayAwake = true;
            Serial.println("[BOOT] Fresh boot or unknown wakeup. Init RFID and check.");
            initializeModules(0);
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

void initializeModules(int moduleControl){
    if (moduleControl == 0){
        setupHeltec();
        setupAP();
        setupOTAasync();
        setupWebPages();
        initWebSocket();
        beginServer();
        setupDNS();
        beginDB();
    }
    else{
        setupHeltec();
        beginDB();
    }


    pinMode(led, OUTPUT);
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);

    msgEspLock1();

    ipString = "Success! " + ipMsg;
}