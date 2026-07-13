#ifndef SLEEPSETUP_H
#define SLEEPSETUP_H

#include <Arduino.h>
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"

// -----------------------------------------------
// Global variables
#define BUTTON_PIN GPIO_NUM_13
#define DVRSLEEP_PIN GPIO_NUM_14

// -----------------------------------------------
// Function prototypes
void sleepSetup();
void sleepButtonsSetup();

#endif