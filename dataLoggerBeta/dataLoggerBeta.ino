/**
 * @Author: Yichen Xu
 * @Date:   09-Feb-2017 19:02:46
 * @Email:  ichi@u.northwestern.edu
 * @Last modified by:   Yichen Xu
 * @Last modified time: 09-Feb-2017 19:02:52
 */



#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"
#include "utility/imumaths.h"

// if debugging, set to 1, otherwise set to 0
#define DEBUG 1

// ================= FUNCTION DECLARATIONS =================
typedef struct {
    uint8_t sysCal, gyroCal, bAccelCal, magCal;
} CalibrationData;

typedef struct {
    float time;
    sensors_vec_t accel, orientation;
    imu::Vector<3> gyro, bAccel, euler;
    CalibrationData c;
} DataSet;

void initializeAccel();
void initializeBNO();
void initializeSD();
sensors_vec_t getAccelData();
sensors_vec_t getOrientationData();
CalibrationData getCalibrationStatus();
DataSet getData();



// ============VARIABLE DECLARATION============================

// Pin select for SD Card. DO NOT MODIFY
const int chipSelect = BUILTIN_SDCARD;

// I forgot what I made mode do lol
const int MODE = 0;

// Connect x, y, z to 7, 8, 9, respectively
const int xInput = 0;
const int yInput = 1;
const int zInput = 2;

// min/max values for each axis
// get from calibration sketch
const int xMin = 498;
const int xMax = 512;

const int yMin = 497;
const int yMax = 512;

const int zMin = 498;
const int zMax = 512;

// WHOOO VARIABLES!
float xBase, yBase, zBase, xConv, yConv, zConv;
float xScaled, yScaled, zScaled;

// Initialize analog pin values
int xVal = 0;
int yVal = 0;
int zVal = 0;

// Interrupt timer declaration
IntervalTimer myTimer;

// Counters for keeping track of time
int counter = 0;
int delayTime = 20000;

// bno declaration
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// output file varable name declaration
char outputFile[100];
String outputString;

void setup() {
    delay(1000); // idk just leave this in

    // slow output console if debugging
    if (DEBUG) {
        delayTime = 1000000;
    }

    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    initializeBNO();

    initializeAccel();

    initializeSD();

    // initialize interrupt timer MAKE SURE THIS IS LAST OR IDK WHAT HAPPENS
    myTimer.begin(WriteData, delayTime);
}

// ISR that we call every time period to get data
void WriteData() {

    DataSet data = getData();

    // make a string for assembling the data to log:
    // output time,xAccel,yAccel,zAccel,OrienX,OrienY,OrienZ,angVx,AngVy,bnoAccelx,bnoAccely,bnoAccelz
    String dataString = "";
    // get time
    dataString += data.time;
    dataString += ",";
    dataString += data.accel.x;
    dataString += ",";
    dataString += data.accel.y;
    dataString += ",";
    dataString += data.accel.z;
    dataString += ",";
    dataString += data.orientation.x;
    dataString += ",";
    dataString += data.orientation.y;
    dataString += ",";
    dataString += data.orientation.z;
    dataString += ",";
    dataString += data.gyro.x(); //lol dont ask me why there nees to be () here
    dataString += ",";
    dataString += data.gyro.y();
    dataString += ",";
    dataString += data.gyro.z();
    dataString += ",";
    dataString += data.bAccel.x();
    dataString += ",";
    dataString += data.bAccel.y();
    dataString += ",";
    dataString += data.bAccel.z();
    dataString += ",";
    dataString += data.c.sysCal;
    dataString += ",";
    dataString += data.c.gyroCal;
    dataString += ",";
    dataString += data.c.bAccelCal;
    dataString += ",";
    dataString += data.c.magCal;

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open(outputFile, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.print("error opening ");
        Serial.println(outputFile);
    }
}

void loop() {
    ;
}

DataSet getData() {
    DataSet tempData;
    tempData.time = (counter++) * ((float) delayTime) / 1000000;
    tempData.accel = getAccelData();
    tempData.orientation = getOrientationData();
    tempData.gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    tempData.bAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    tempData.euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    tempData.c = getCalibrationStatus();
    return tempData;
}

CalibrationData getCalibrationStatus () {
    CalibrationData sensCal;
    bno.getCalibration(&(sensCal.sysCal), &(sensCal.gyroCal), &(sensCal.bAccelCal), &(sensCal.magCal));
    return sensCal;
}
sensors_vec_t getOrientationData() {
        sensors_event_t event;
        bno.getEvent(&event);
        return event.orientation;
}

sensors_vec_t getAccelData () {
    // get analog Sensor Data
    xVal = analogRead(xInput);
    yVal = analogRead(yInput);
    zVal = analogRead(zInput);

// Scale pins
    xScaled = (xVal - xBase) / xConv;
    yScaled = (yVal - yBase) / yConv;
    zScaled = (zVal - zBase) / zConv;

// scale if mode change
    if (MODE) {
        xScaled *= SENSORS_GRAVITY_EARTH;
        yScaled *= SENSORS_GRAVITY_EARTH;
        zScaled *= SENSORS_GRAVITY_EARTH;
    }

    sensors_vec_t accelPoint;
    accelPoint.x = xScaled;
    accelPoint.y = yScaled;
    accelPoint.z = zScaled;
    return accelPoint;
}

void initializeBNO() {
    // initializes BNO and checks if there is a connection
    if (!bno.begin()) {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1) {
            ;
        }
    }

}

void initializeSD() {
    // initalize SD CARD
    Serial.print("Initializing SD card...");
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");

    // Checks and determines appropriate filename
    outputString = "datalog0.csv";
    outputString.toCharArray(outputFile, 100);
    int i = 0;
    while (SD.exists(outputFile)) {
        i++;
        outputString = "datalog";
        outputString += String(i);
        outputString += ".csv";
        Serial.println(outputString);
        outputString.toCharArray(outputFile, 100);
    }
    // dont mind me I'm just making the excel easier to read

    String dataString = "Time,Accel_X,Accel_Y,Accel_Z,Orientation_X,Orientation_Y,Orientation_Z,Angular_Accel_X,Angular_Accel_Y,Angular_Accel_Z,BNO_Accel_X,BNO_Accel_Y,BNO_Accel_Z,Sys Calibration,Gyro Calibration,Accel Calibration,Mag Calibration";

    File dataFile = SD.open(outputFile, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.print("error opening ");
        Serial.println(outputFile);
    }
}

void initializeAccel() {
    // reads input?
    xVal = analogRead(xInput);
    yVal = analogRead(yInput);
    zVal = analogRead(zInput);

    // Scale pins
    xScaled = (xVal - xBase) / xConv;
    yScaled = (yVal - yBase) / yConv;
    zScaled = (zVal - zBase) / zConv;

    // scale if mode change
    if (MODE) {
        xScaled *= SENSORS_GRAVITY_EARTH;
        yScaled *= SENSORS_GRAVITY_EARTH;
        zScaled *= SENSORS_GRAVITY_EARTH;
    }

    // Calculation variable we really don't care about
    int xDiff = xMax - xMin;
    int yDiff = yMax - yMin;
    int zDiff = yMax - yMin;

    // Calculating the value of zero
    xBase = ((float) xMin + xMax) / 2;
    yBase = ((float) yMin + yMax) / 2;
    zBase = ((float) zMin + zMax) / 2;

    // Calculating the conversion factor for 1g
    xConv = ((float) xDiff) / 2;
    yConv = ((float) yDiff) / 2;
    zConv = ((float) zDiff) / 2;
}
