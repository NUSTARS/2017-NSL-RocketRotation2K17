/**
   @Author: Yichen Xu
   @Date:   19-Feb-2017 23:02:34
   @Email:  ichi@u.northwestern.edu
* @Last modified by:
* @Last modified time: 2017-04-01T03:25:45-05:00
 */



#include "PID.h"

int prevEI;

float turnLeft = 750;
int prevE = turnLeft;
int powerG;
int isLaunchGyroSet = 0;
float pE, iE, dE;
float dAvg[5];
int dAvgPoint = 0;
float initalVelocityThreshold = 180;
enum flightPlan_t { RESET, PID } plan = RESET;


// target time for trajectory
int targetTime = 5000;

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

    // what is powerG? I don't know I blame Ethan.
    powerG = power;

    #if DEBUG
    Serial.print("Power:   ");
    Serial.println(power);
    #endif

    // Positive power is direction Pin HIGH. Just because
    if (power > 0) {
        digitalWrite(directionPin, HIGH);
        analogWrite(speedPin, abs(power));  // abs(power) cant hurt
    }
    else { // else its 0 or negative. Either way, direction is LOW
        digitalWrite(directionPin, LOW);
        analogWrite(speedPin, abs(power));
    }
}

// Yeah I need to be better about naming things
void doTheThing(uint32_t startTime) {
    // Really startTime isnt neccessary considering we already have launchTimestamp

#if DEBUG
    Serial.print(currentData.time - startTime);
    Serial.print("    turnleft:  ");
    Serial.print(turnLeft);

#endif

    // If time elapsed (which shoudl be its own thing) is greater than waitTime
    if ((currentData.time - startTime) > waitTime) {
        // get initial velocity
        if (!isLaunchGyroSet) {
            launchGyro = currentData.gyro.x;
            isLaunchGyroSet = !isLaunchGyroSet;

            // Checks if the launchGyro velocity is greater than the threshold in degrees per second
            if (launchGyro / SENSORS_DPS_TO_RADS > initalVelocityThreshold) {
                // if it is, we do the PID only method of contolling it
                plan = PID;
            }
            else {
                // else we change the plan to reset the velocity first
                plan = RESET;
            }

        }

        // u is the number to output to the Motor
        int u = 0;

        // if the plan is to PID the controller
        if (plan == PID) {
            // gets PID calculations
            u = calculatePID();
        }
        // Else if the plan is to Reset, we call the resetVelocity function
        else if (plan == RESET) {
            // sets u to the velocity given by the resetVelocity function
            u = resetVelocity();
        }

        // limits u to +- 255, though it should be fine without this
        if (u < 0) {
            if ( u < -255) {
                u = -255;
            }
            // Outputs that to the motor
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

// Generates a linear trajectory to reach the desired position.
float calculateError(void) {
    // get time elasped, which is currentTime - launchTime - waitTime
    int timeElapsed = currentData.time - launchTimestamp - waitTime;
    // this is kinda like how far from the target we should be
    //So this returns anywhere from 0.8 of the original turnLeft to 0;
    float errorT = 0.8 * turnLeft - (0.8 * turnLeft * timeElapsed) / targetTime;

    #if DEBUG
    Serial.print("timeElapsed: ");
    Serial.println(timeElapsed);
    #endif
    // we dont want this to keep going forever
    if (errorT < 0) {
        errorT = 0;
    }

    //If I sneak in a Fuck Evan here, He won't notice right?
    #if DEBUG
    Serial.print("error: ");
    Serial.println(turnLeft - errorT);
    #endif

    // we take turnLeft - errorT to get a line going from 20% of turnLeft to 100% of turnLeft in targetTime
    errorT = turnLeft - errorT;

    return errorT;
}

// resets the Velocity of the Motor
int resetVelocity(void) {

    // If the signs of the gyro and launch gyro match, then the thing hasn't crossed 0 velocity yet in theory. That would cause a sign change, the signs dont match, then the formula below would return something less than 0
    if (((float) currentData.gyro.x) / launchGyro > 0) {
        // Returns a velocity given by the initial velocity *kreset Constant.
        // Important to note this is not a PID contorller, just a quick and dirty method of getting it to reset.

        // NOTE: possible that we need to set a time limit to this function
        int returnVelocity = (launchGyro / SENSORS_DPS_TO_RADS) * kreset;
        //kreset might be negative
        return max(returnVelocity, 100); //return a number making sure that we give the motor more than a slight nudge.
    }
    else {
        // This means the resetVelocity function's job is done
        plan = PID; // sets plan to PID to start the PID functionality of the code

        if (launchGyro < 0) { // the signs totally might be flipped on this one

            turnLeft = -1 * turnLeft; // Turnleft needs to go in the opposite direction
            prevE = -1 * turnLeft;

        }
        else {
            ;//do nada. Everything should be fine
        }
        launchGyro = 0; // our initial velocity is now 0

        return 0;
    }
}
