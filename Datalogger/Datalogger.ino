/*
   SD card datalogger

   This example shows how to log data from three analog sensors
   to an SD card using the SD library.

   The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11, pin 7 on Teensy with audio board
 ** MISO - pin 12
 ** CLK - pin 13, pin 14 on Teensy with audio board
 ** CS - pin 4,  pin 10 on Teensy with audio board

   created  24 Nov 2010
   modified 9 Apr 2012
   by Tom Igoe

   This example code is in the public domain.

 */

#include <SD.h>
#include <SPI.h>
#define GRAVITY 9.81

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Teensy audio board: pin 10
// Teensy 3.5 & 3.6 on-board: BUILTIN_SDCARD
// Wiz820+SD board: pin 4
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
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

void setup() {

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
    while (!Serial) {
        ; // wait for serial port to connect. Needed for Leonardo only
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
}

void loop() {
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
    dataString += xScaled;
    dataString += ",";
    dataString += yScaled;
    dataString += ",";
    dataString += zScaled;


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
