void deepSleepSetup(){
    #if CONFIG_IDF_TARGET_ESP32
    #define THRESHOLD 40 // Greater the value, more the sensitivity
    #else // ESP32-S2 and ESP32-S3 + default for other chips (to be adjusted)
    #define THRESHOLD 3000 // Lower the value, more the sensitivity
    #endif

    touchSleepWakeUpEnable(2, THRESHOLD);
}



