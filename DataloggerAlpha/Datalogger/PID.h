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



int calculatePID();
void outputMotor(int power);
void doTheThing(uint32_t timestamp);
float calculateError();

#endif
