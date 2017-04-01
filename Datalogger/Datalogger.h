/**
* @Author: Yichen Xu
* @Date:   20-Feb-2017 01:02:75
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 20-Feb-2017 01:02:86
*/



#ifndef DATALOGGER_H
#define DATALOGGER_H

// if debugging, set to 1, otherwise set to 0
#define DEBUG 1


#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"
#include "utility/imumaths.h"
#include <stdio.h>

//basically everything is an Extern to simplify and speed up code.
typedef struct {
    uint8_t sysCal, gyroCal, bAccelCal, magCal;
} CalibrationData;

typedef struct {
    int32_t time;
    sensors_vec_t accel, orientation, gyro, bAccel;
    CalibrationData c;
} DataSet;

//============PIN DECLARATIONS
extern int xInput, yInput, zInput;
extern int chipSelect;
extern int buttonPin, speedPin, motorPin, collectPin, calibrationPin;
extern int pulsePin;
extern int directionPin;
extern int torquePin;

//==================File Shenanigans
extern char outputFile[100];
extern String outputString;
extern File dataFile;

//WEIRD ACCELEROMETER STUFF
extern int MODE, xMin, xMax, yMin, yMax, yMin, zMin, zMax;
extern int xVal, yVal, zVal;

extern float xBase, yBase, zBase, xConv, yConv, zConv, xScaled, yScaled, zScaled;

//PID STUFF
extern DataSet prevData, currentData;
extern Adafruit_BNO055 bno;
extern float kp, ki, kd;
extern double launchGyro;
extern int powerG;
extern int waitTime;
extern volatile float motorSpeed[5];
extern float turnLeft, pE, dE, iE;
extern uint32_t launchTimestamp;

#endif
