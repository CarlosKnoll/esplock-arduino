#include "sleepSetup.h"

void sleepSetup() {
    esp_sleep_enable_timer_wakeup(1000); // Wake up every 500 ms
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 0);
    // Make sure button isn't already pressed before enabling EXT1
    while (digitalRead(GPIO_NUM_2) == LOW) {
        Serial.println("Button pressed — wait to arm EXT1 wakeup...");
        delay(10);
    }
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_2, ESP_EXT1_WAKEUP_ALL_LOW);
    delay(100); // Let pin state settle
    esp_deep_sleep_start();
}
