/**
  @Author: Yichen Xu
  @Date:   19-Feb-2017 23:02:34
  @Email:  ichi@u.northwestern.edu
  @Last modified by:   Yichen Xu
  @Last modified time: 19-Feb-2017 23:02:41
*/



#include "PID.h"

int prevEI;
int prevE = 720;
float turnLeft = 720;

int calculatePID() {
  float u, pE, iE, dE;
  int dt = currentData.time - prevData.time;
  // change this so that going across 0 and 360 doesnt involve a huge difference (degrees or radians?)

  //change in orientation = change in turn left

  double xdot = (currentData.gyro.x-launchGyro) / (SENSORS_DPS_TO_RADS * 1000) * dt;

  Serial.println(xdot);

  //set reference angular velocity
  //take angular velocity * dt, should be equal to dPos
  //subtract from turnLeft
  //*need to figure out which way to turn (oppose direction of motion?)
  //*or stop velcoity first, but thats harder
  //then we can use turnleft to do everything else.

  turnLeft = turnLeft - xdot;

  pE = turnLeft * kp;
  dE = dt * (turnLeft - prevE);
  prevE = turnLeft;
  prevEI += turnLeft;
   if (prevEI > 1550) {
    prevEI = 1550;
  }
  if (prevEI < 0) {
    prevEI = 0;
  }
  iE = prevEI * ki;


  u = iE + dE + pE;

  return u;
}

void outputMotor(int power) {
  Serial.println(power);
  if (power < 0) {
  
  analogWrite(directionPin, 255);
    analogWrite(speedPin, abs(power));
  }
  else {
    analogWrite(directionPin, 0);
    analogWrite(speedPin, power);
  }
}

void doTheThing(uint32_t timestamp) {
  Serial.print(currentData.time-timestamp);
  Serial.print("    turnleft:  ");
  Serial.print(turnLeft);
  Serial.print("    power:  ");
  if (currentData.time-timestamp > 3000) {
    int u = calculatePID();

    if (u < 0) {
      if ( u < -255) {
        u = -255;
      }
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

