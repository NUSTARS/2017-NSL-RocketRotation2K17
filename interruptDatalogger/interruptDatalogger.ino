/**
* @Author: Yichen Xu
* @Date:   09-Feb-2017 19:02:46
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 09-Feb-2017 19:02:52
*/



#include <SD.h>
#include <SPI.h>
#define GRAVITY 9.81
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"
#include "utility/imumaths.h"

const int chipSelect = BUILTIN_SDCARD;
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

IntervalTimer myTimer;

int counter = 0;
int delayTime = 1000000;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup() {
    delay(5000);
    xVal = analogRead(xInput);
    yVal = analogRead(yInput);
    zVal = analogRead(zInput);

// Scale pins
    xScaled = (xVal - xBase) / xConv;
    yScaled = (yVal - yBase) / yConv;
    zScaled = (zVal - zBase) / zConv;

// scale if mode change
    if (MODE) {
        xScaled *= GRAVITY;
        yScaled *= GRAVITY;
        zScaled *= GRAVITY;
    }

    // UNCOMMENT THESE TWO LINES FOR TEENSY AUDIO BOARD:
    // SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
    // SPI.setSCK(14);  // Audio shield has SCK on pin 14

    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    if(!bno.begin())
     {
       /* There was a problem detecting the BNO055 ... check your connections */
       Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
       while(1);
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


    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");
    myTimer.begin(WriteData, delayTime);
}


void WriteData() {
    sensors_event_t event;
    bno.getEvent(&event);

    xVal = analogRead(xInput);
    yVal = analogRead(yInput);
    zVal = analogRead(zInput);

// Scale pins
    xScaled = (xVal - xBase) / xConv;
    yScaled = (yVal - yBase) / yConv;
    zScaled = (zVal - zBase) / zConv;

// scale if mode change
    if (MODE) {
        xScaled *= GRAVITY;
        yScaled *= GRAVITY;
        zScaled *= GRAVITY;
    }
    // make a string for assembling the data to log:
    String dataString = "";
    dataString += (counter++)*delayTime/1000000;
    dataString += ",";
    dataString += xScaled;
    dataString += ",";
    dataString += yScaled;
    dataString += ",";
    dataString += zScaled;
    dataString += event.orientation.x;
    dataString += ",";
    dataString += event.orientation.y;
    dataString += ",";
    dataString += event.orientation.z;

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.println("error opening datalog.txt");
    }
}

void loop() {
    ;
}
