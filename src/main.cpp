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
    setupRFID();                // Initialize RFID reader, which decides next actions
    
    bool cardDetected = checkForCard();

    Serial.println("[BOOT] Wakeup reason: " + String(wakeup_reason));
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER: // Wakeup from timer
            stayAwake = false;
            Serial.println("[WAKE] Woke up by timer. Checking for card...");
            if (digitalRead(wakeupPin) == LOW) { // If button is pressed, stay awake and initialize modules
                stayAwake = true;
                flagTime = true;
                initializeModules(0);
                delay(10);
                break;
            } else { // If button is not pressed, simply check for card
                if (!cardDetected) { // If no card is detected, go to sleep
                    Serial.println("[WAKE] No card detected. Going to sleep...");
                    sleepSetup();
                    break;
                } else { // If card is detected, initialize modules for logging access, then go to sleep
                    Serial.println("[WAKE] Card found. Staying awake.");
                    initializeModules(1);
                    access();
                break;
            }

        default: // Fresh boot or unknown wakeup reason
            stayAwake = true; // Forcing stayAwake to true to allow for time update
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
    if (digitalRead(wakeupPin) == LOW && flagTime) {    // flagtime is gating a sleep cycle with time desync
        Serial.println("[SLEEP] Button pressed. Going to sleep...");
        sleepSetup();
    }
}

// -----------------------------------------------

void initializeModules(int moduleControl){
    pinMode(led, OUTPUT);
    wakeButtonSetup();          // Setup wake button with pull-up resistor and deinit RTC control

    setupHeltec();              // Initialize Heltec display and SPIFFS
    beginDB();                  // Initialize SQLite database
    setupMotor();               // Initialize motor control pins

    if (moduleControl == 0){    // Fresh boot or unknown wakeup, setup all else so time can be updated
        setupAP();              // Setup Access Point
        setupOTAasync();        // Setup OTA updates
        setupWebPages();        // Setup web pages and routes
        initWebSocket();        // Initialize WebSocket for real-time communication
        beginServer();          // Start the web server
        setupDNS();             // Setup mDNS for easier aliasing of the device on the network
    }

    msgEspLock1();
    ipString = "Success! " + ipMsg;
}