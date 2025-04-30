#include "main.h"

String ipString;
esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

// -----------------------------------------------
void setup(void)
{
    Serial.begin(115200);
    // pinMode(BUTTON_PIN, INPUT);
    // rtc_gpio_deinit(GPIO_NUM_2);         // Ensure RTC control is allowed
    // rtc_gpio_pullup_dis(GPIO_NUM_2);      // Enable internal pull-up (optional if external used)
    // rtc_gpio_pulldown_en(GPIO_NUM_2);      // Enable internal pull-down (optional if external used)
    setupRFID();
    
    bool cardDetected = checkForCard();

    Serial.println("[WAKE] Wakeup reason: " + String(wakeup_reason));
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("[WAKE] Woke up by BUTTON press. Staying awake for user action.");
            initializeModules();
            break;

        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("[WAKE] Woke up by timer. Checking for card...");
            if (!cardDetected) {
                Serial.println("[SLEEP] No card detected. Going to sleep...");
                sleepSetup();
                // initializeModules();
                break;
            } else {
                Serial.println("[ACTIVE] Card found. Staying awake for now.");
                initializeModules();
            break;

        default:
            Serial.println("[BOOT] Fresh boot or unknown wakeup. Init RFID and check.");
            if (!checkForCard()) {
                Serial.println("[SLEEP] No card detected. Going to sleep...");
                sleepSetup();
                break;
            }
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
    beginDB();

    pinMode(led, OUTPUT);
    msgEspLock1();

    Serial.println("HTTP server started");
    ipString = "Success! " + ipMsg;
}