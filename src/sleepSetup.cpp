#include "sleepSetup.h"

void sleepSetup() {
    esp_sleep_enable_timer_wakeup(500); // Wake up every 500 ms
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 1);
    delay(100); // Let pin state settle
    esp_deep_sleep_start();
}
