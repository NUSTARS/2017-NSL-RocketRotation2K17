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
int delayTime = 1000000;

// bno declaration
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// output file varable name declaration
string outputFile = "";

void setup() {
    delay(5000); // idk just leave this in

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
        xScaled *= SENSORS_GRAVITY_EARTY;
        yScaled *= SENSORS_GRAVITY_EARTY;
        zScaled *= SENSORS_GRAVITY_EARTY;
    }


    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    // initializes BNO and checks if there is a connection
    if (!bno.begin()) {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1) {
            ;
        }
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
    sprintf(outputFile, "datalog0.csv");
    int i = 0;
    while (sd.Exists(outputFile)) {
        sprintf(outputFile, "datalog\n.csv", ++i);
    }
    //dont mind me I'm just making the excel easier to read

    String dataString = "";
    //get time
    dataString += "Time";
    dataString += ",";
    dataString += "Accel_x";
    dataString += ",";
    dataString += "Accel_Y";
    dataString += ",";
    dataString += "Accel_Z";
    datastring += ","
    dataString += "Orientation_X";
    dataString += ",";
    dataString += "Orientation_Y";
    dataString += ",";
    dataString += "Orientation_Z";
    datastring += ","
    dataString += "Angular_Accel_X";
    dataString += ",";
    dataString += "Angular_Accel_Y";
    dataString += ",";
    dataString += "Angular_Accel_Z";
    datastring += ","
    dataString += "BNO_Accel_X";
    dataString += ",";
    dataString += "BNO_Accel_Y";
    dataString += ",";
    dataString += "BNO_Accel_Z";
    datastring += ",";
    datastring += "Sys Calibration";
    datastring += ",";
    datastring += "Gyro Calibration";
    datastring += ",";
    datastring += "Accel Calibration";
    datastring += ",";
    datastring += "Mag Calibration";

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

    // initialize interrupt timer MAKE SURE THIS IS LAST OR IDK WHAT HAPPENS
    myTimer.begin(WriteData, delayTime);
}

// ISR that we call every time period to get data
void WriteData() {
    // get BNO sensor data
    sensors_event_t event;

    bno.getEvent(&event);

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
        xScaled *= SENSORS_GRAVITY_EARTY;
        yScaled *= SENSORS_GRAVITY_EARTY;
        zScaled *= SENSORS_GRAVITY_EARTY;
    }


// get raw BNO data
    imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

    uint8_t system, cgyro, caccel, cmag;
    system = cgyro = caccel = cmag = 0;
    bno.getCalibration(&system, &cgyro, &caccel, &cmag);

    // make a string for assembling the data to log:
    //output time,xAccel,yAccel,zAccel,OrienX,OrienY,OrienZ,angVx,AngVy,bnoAccelx,bnoAccely,bnoAccelz
    String dataString = "";
    //get time
    dataString += (counter++) * delayTime / 1000000;
    dataString += ",";
    dataString += xScaled;
    dataString += ",";
    dataString += yScaled;
    dataString += ",";
    dataString += zScaled;
    datastring += ","
    dataString += event.orientation.x;
    dataString += ",";
    dataString += event.orientation.y;
    dataString += ",";
    dataString += event.orientation.z;
    datastring += ","
    dataString += gyro.x();
    dataString += ",";
    dataString += gyro.y();
    dataString += ",";
    dataString += gyro.z();
    datastring += ","
    dataString += accel.x();
    dataString += ",";
    dataString += accel.y();
    dataString += ",";
    dataString += accel.z();
    datastring += ",";
    datastring += system;
    datastring += ",";
    datastring += cgyro;
    datastring += ",";
    datastring += caccel;
    datastring += ",";
    datastring += cmag;

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
