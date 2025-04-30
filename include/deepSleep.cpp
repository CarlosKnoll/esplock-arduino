void deepSleepSetup(){
    Serial.println("Setting up deep sleep...");
    // #if CONFIG_IDF_TARGET_ESP32
    // #define THRESHOLD 40 // Greater the value, more the sensitivity
    // #else // ESP32-S2 and ESP32-S3 + default for other chips (to be adjusted)
    //#define THRESHOLD 3000 // Lower the value, more the sensitivity
    // #endif

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, LOW);
    //esp_err_t err = touchSleepWakeUpEnable(TOUCH_PAD_NUM2, THRESHOLD);
    // if (err != ESP_OK) {
    // Serial.println("Error enabling touch wakeup");
    // }
}


