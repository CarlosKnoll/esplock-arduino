#include "sleepSetup.h"
#include "main.h"

void sleepSetup() {
    rtc_gpio_deinit(BUTTON_PIN);         // Ensure RTC control is allowed
    rtc_gpio_pullup_en(BUTTON_PIN);      // Enable pull-up on GPIO 2
    rtc_gpio_pulldown_dis(BUTTON_PIN);   // Disable pull-down on GPIO 2
    esp_sleep_enable_timer_wakeup(1000); // Wake up every 500 ms
    delay(100); // Let pin state settle
    digitalWrite(wakeMonitor, LOW);
    esp_deep_sleep_start();
}
