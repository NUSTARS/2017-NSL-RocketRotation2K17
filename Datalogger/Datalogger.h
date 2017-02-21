/**
* @Author: Yichen Xu
* @Date:   20-Feb-2017 01:02:75
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 20-Feb-2017 01:02:86
*/



#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"
#include "utility/imumaths.h"
#include <stdio.h>

typedef struct {
    uint8_t sysCal, gyroCal, bAccelCal, magCal;
} CalibrationData;

typedef struct {
    int32_t time;
    sensors_vec_t accel, orientation, gyro, bAccel;
    CalibrationData c;
} DataSet;

extern char outputFile[100];
extern String outputString;

extern int buttonPin, speedPin, motorPin, collectPin, calibrationPin;
extern int pulsePin;

extern int MODE, xMin, xMax, yMin, yMax, yMin, zMin, zMax;


extern int xVal, yVal, zVal;
extern int xInput, yInput, zInput;
extern int chipSelect;

externInt prevError

extern float xBase, yBase, zBase, xConv, yConv, zConv, xScaled, yScaled, zScaled;

extern Adafruit_BNO055 bno;

#endif
