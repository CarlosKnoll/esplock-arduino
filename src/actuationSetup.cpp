#include "actuationSetup.h"

void setupMotor() {
    ledcSetup(0, PWM_FREQ, 8);  // channel 0, freq, 8-bit resolution
    ledcSetup(1, PWM_FREQ, 8);  // channel 1
    ledcAttachPin(direction1, 0);  // GPIO32 → channel 0
    ledcAttachPin(direction2, 1);  // GPIO33 → channel 1
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    Serial.println("[MOTOR] LEDC attached on GPIO" + String(direction1) + " and GPIO" + String(direction2));
}

void motorBrake() {
    ledcWrite(0, 255);
    ledcWrite(1, 255);
}

void motorCoast() {
    ledcWrite(0, 0);
    ledcWrite(1, 0);
}

static void rampUp(uint8_t channel) {
    for (int i = 1; i <= rampSteps; i++) {
        ledcWrite(channel, (maxDuty / rampSteps) * i);
        delay(rampDelay);
    }
}

void motorDir1() {
    ledcWrite(1, 0);
    rampUp(0);
    delay(actuationTime);
    motorBrake();
    delay(coastTime);
    motorCoast();
}

void motorDir2() {
    ledcWrite(0, 0);
    rampUp(1);
    delay(actuationTime);
    motorBrake();
    delay(coastTime);
    motorCoast();
}

void actuate_lock() {
    motorCoast();
    delay(coastTime);
    Serial.println("[ACTUATION] Actuating lock...");
    motorDir1();
    Serial.println("[ACTUATION] Delay for user entry...");
    delay(delayOpen);
    Serial.println("[ACTUATION] Reversing lock...");
    motorDir2();
}