#ifndef ACTUATIONSETUP_H
#define ACTUATIONSETUP_H

#include <Arduino.h>

#define direction1 2
#define direction2 17

#define delayOpen 5000
#define actuationTime 150
#define coastTime 50
#define rampSteps 10
#define rampDelay 5
#define maxDuty 210
#define PWM_FREQ 20000
#define MOTOR_CH1 0
#define MOTOR_CH2 1

void setupMotor();
void motorBrake();
void motorCoast();
void motorDir1();
void motorDir2();
void actuate_lock();

#endif