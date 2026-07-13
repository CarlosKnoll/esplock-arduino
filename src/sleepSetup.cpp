#include "sleepSetup.h"
#include "main.h"

void sleepSetup() {
    sleepButtonsSetup();                        // Configure sleep buttons
    esp_sleep_enable_timer_wakeup(500000);      // Function takes time argument in microseconds (500000 us = 0.5 s)
    digitalWrite(wakeMonitor, LOW);
    dvrSleep(true);                    // Put DVR to sleep
    esp_deep_sleep_start();
}

void sleepButtonsSetup() {
    // Configure GPIO 13 as wakeup source
    rtc_gpio_deinit(BUTTON_PIN);         // Ensure RTC control is allowed
    rtc_gpio_pullup_en(BUTTON_PIN);      // Enable pull-up on GPIO 2
    rtc_gpio_pulldown_dis(BUTTON_PIN);   // Disable pull-down on GPIO 2

    // Configure GPIO 14 as output for DVR sleep control
    pinMode(DVRSLEEP_PIN, OUTPUT);          

    delay(100); // Let pins state settle
    Serial.println("[SLEEP] Sleep buttons initialized.");
}