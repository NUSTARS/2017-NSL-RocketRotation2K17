/**
   @Author: Yichen Xu
   @Date:   19-Feb-2017 23:02:34
   @Email:  ichi@u.northwestern.edu
   @Last modified by:   Yichen Xu
   @Last modified time: 19-Feb-2017 23:02:41
 */



#include "PID.h"

int prevEI;
int prevE = 750;
float turnLeft = 750;
int powerG;
int isLaunchGyroSet = 0;
float pE, iE, dE;
float dAvg[5];
int dAvgPoint = 0;

// Actually a PSD controller
int calculatePID() {
    float u;
    int dt = currentData.time - prevData.time;
    // change this so that going across 0 and 360 doesnt involve a huge difference (degrees or radians?)

    // change in orientation = change in turn left

    double xdot = ((currentData.gyro.x - launchGyro) / (SENSORS_DPS_TO_RADS * 1000)) * dt;

    // double xdot = (currentData.gyro.x) / (SENSORS_DPS_TO_RADS * 1000) * dt;

#if DEBUG
    Serial.print("    xDot:  ");
    Serial.println(xdot);
#endif

    // set reference angular velocity
    // take angular velocity * dt, should be equal to dPos
    // subtract from turnLeft
    // *need to figure out which way to turn (oppose direction of motion?)
    // *or stop velcoity first, but thats harder
    // then we can use turnleft to do everything else.

    turnLeft = turnLeft - xdot;
    float error = 0;

    // Gets triangle function for calculating Error
    error = calculateError();


    // Scales pE
    pE = error * kp;


    // calculates Moving average derivative
    dAvg[dAvgPoint] = (error - prevE) / dt;
    dAvgPoint++;
    dAvgPoint = dAvgPoint % 5;
    dE = 0;
    for (int i = 0; i < 5; i++) {
        dE += dAvg[i];
    }
    dE = dE / 5;

    // Sets prevE to be this eror
    prevE = error;

    // Sets prevEI to be running sum
    prevEI += error * dt;


    // Anti integrator windup part
    if (prevEI > 100 / ki) {
        prevEI = 100 / ki;
    }
    if (prevEI < -100 / ki) {
        prevEI = -100 / ki;
    }

    // scales iE
    iE = prevEI * ki;

    // scales dE
    dE = kd * dE;

    // Sum of everything is the motor output
    u = iE + dE + pE;

    return u;
}

// Takes in power and turns it into a directional output
void outputMotor(int power) {

    //what is powerG? I don't know I blame Ethan.
    powerG = power;

    #if DEBUG
    Serial.print("Power:   ");
    Serial.println(power);
    #endif

    //Positive power is direction Pin HIGH. Just because
    if (power > 0) {
        digitalWrite(directionPin, HIGH);
        analogWrite(speedPin, abs(power));  //abs(power) cant hurt
    }
    else { //else its 0 or negative. Either way, direction is LOW
        digitalWrite(directionPin, LOW);
        analogWrite(speedPin, abs(power));
    }
}

//Yeah I need to be better about naming things
void doTheThing(uint32_t startTime) {
    //Really startTime isnt neccessary considering we already have launchTimestamp

#if DEBUG
    Serial.print(currentData.time - startTime);
    Serial.print("    turnleft:  ");
    Serial.print(turnLeft);

#endif

    //If time elapsed (which shoudl be its own thing) is greater than waitTime
    if ((currentData.time - startTime) > waitTime) {
        //get initial velocity
        if (!isLaunchGyroSet) {
            launchGyro = currentData.gyro.x;
            isLaunchGyroSet = !isLaunchGyroSet;
        }
        //TODO if initial velocity is greater than NUM, do nothing. Else set flag to do another thing instead of calculatePID;

        //getsPID calculations
        int u = calculatePID();

        //limits u to +- 255, though it should be fine without this
        if (u < 0) {
            if ( u < -255) {
                u = -255;
            }
            //Outputs that to the motor
            outputMotor(u);
        }
        else {
            if (u > 255) {
                u = 255;
            }
            outputMotor(u);
        }
    }
}

//Generates a linear trajectory to reach the desired position.
float calculateError(void) {
    int timeElapsed = currentData.time - launchTimestamp - 5000;
    float errorT = 650 - (750 * timeElapsed) / 5000;

    Serial.print("timeElapsed: ");
    Serial.println(timeElapsed);
    if (errorT < 0) {
        errorT = 0;
    }
    Serial.print("error: ");
    Serial.println(turnLeft - errorT);
    errorT = turnLeft - errorT;

    return errorT;

}
