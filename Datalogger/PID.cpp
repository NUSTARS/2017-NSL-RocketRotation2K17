/**
* @Author: Yichen Xu
* @Date:   19-Feb-2017 23:02:34
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 19-Feb-2017 23:02:41
*/



#include "PID.h"

int calculatePID(DataSet * d) {
    int u, pE, iE, dE;
    int dt = 1;
    //int dt = change in time IMPLEMENT
    int edot = d->orientation.x - prevPos
    turnLeft = turnLeft - edot;
    prevPos = d->orientation.x;

    pE = turnLeft * kp;

    prevEI += turnLeft * ki * dt;
    iE = prevEI + turnLeft * ki;
    dE = kd* edot;
    if (iE > 155) {
        iE = 155;
    }
    u = iE+dE+pE;


    if (u>255) {
        u = 255;
    }

    if (u<0) {
        u = 0;
    }

    return u;
}

void outputMotor(int power) {
    analogWrite(speedPin, power);
}

void doTheThing(DataSet  * d) {
    outputMotor(calculatePID(&d));
}
