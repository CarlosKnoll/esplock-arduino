#include "main.h"

String ipString;
esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
bool stayAwake = false;


// -----------------------------------------------
void setup(void){
    //On top of future SLEEP pin, safeguard on driver pins
    pinMode(direction1, OUTPUT);
    pinMode(direction2, OUTPUT);

    //Monitor pin for data gathering of esps wake cycle
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
            if (digitalRead(wakeupPin) == LOW) {
                stayAwake = true;
                flagTime = true;
                initializeModules(0);
                delay(10);
                break;
            } else {
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

    // If detected button press and timestamp is updated, go to sleep
    if (digitalRead(wakeupPin) == LOW && flagTime) {
        Serial.println("[SLEEP] Button pressed. Going to sleep...");
        sleepSetup();
    }
}

// -----------------------------------------------

void initializeModules(int moduleControl){
    pinMode(led, OUTPUT);
    wakeButtonSetup();

    setupHeltec();
    beginDB();
    setupMotor();

    if (moduleControl == 0){
        setupAP();
        setupOTAasync();
        setupWebPages();
        initWebSocket();
        beginServer();
        setupDNS();
    }

    msgEspLock1();

    ipString = "Success! " + ipMsg;
}