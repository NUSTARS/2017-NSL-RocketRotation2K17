/**
* @Author: Yichen Xu
* @Date:   19-Feb-2017 23:02:32
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 19-Feb-2017 23:02:38
*/

#ifndef PID_H
#define PID_H
#include "Datalogger.h"


// Calculates the PID output from current state,
int calculatePID();

//outputs the PID output to the motor, sanitizing as neccessary
void outputMotor(int power);

//Calls the other functions in sequence to get an output to the motor
void doTheThing(uint32_t timestamp);

//generates a linear trajectory error path for us to follow
float calculateError();

//used to reset the current velocity of motor to zero
int resetVelocity();

//resets controller
void resetController();

#endif
