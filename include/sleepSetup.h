#ifndef SLEEPSETUP_H
#define SLEEPSETUP_H

#include <Arduino.h>
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"

// -----------------------------------------------
// Global variables
#define BUTTON_PIN GPIO_NUM_13

// -----------------------------------------------
// Function prototypes
void sleepSetup();

#endif