#include "sleepSetup.h"

void sleepSetup() {
    rtc_gpio_deinit(GPIO_NUM_12);         // Ensure RTC control is allowed
    rtc_gpio_pullup_en(GPIO_NUM_12);      // Enable pull-up on GPIO 2
    rtc_gpio_pulldown_dis(GPIO_NUM_12);   // Disable pull-down on GPIO 2
    esp_sleep_enable_timer_wakeup(1000); // Wake up every 500 ms
    delay(100); // Let pin state settle
    esp_deep_sleep_start();
}
