
#include "init.h"


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

void initializeSD() {
    // initalize SD CARD
    #if DEBUG
    Serial.print("Initializing SD card...");
    #endif
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        #if DEBUG
        Serial.println("Card failed, or not present");
        #endif
        // don't do anything more:
        return;
    }
    #if DEBUG
    Serial.println("card initialized.");
    #endif
}

//Makes a new file
void newFile() {
    outputString = "data0.csv";
    outputString.toCharArray(outputFile, 100);
    int i = 0;
    while (SD.exists(outputFile)) {
        i++;
        outputString = "data";
        outputString += String(i);
        outputString += ".csv";
        outputString.toCharArray(outputFile, 100);
    }
    #if DEBUG
    Serial.print("Filename: ");
    Serial.println(outputString);

    #endif

    // dont mind me I'm just making the excel easier to read

    String dataString = "Time,Orientation_X,Orientation_Y,Orientation_Z,Angular_Accel_X,Angular_Accel_Y,Angular_Accel_Z,BNO_Accel_X,BNO_Accel_Y,BNO_Accel_Z,Sys Calibration,Gyro Calibration,Accel Calibration,Mag Calibration, power, Motor Speed, turnLeft, pE, iE, dE";

    dataFile = SD.open(outputFile, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);

        // print to the serial port too:
        #if DEBUG
        Serial.println(dataString);
        #endif
    }
    // if the file isn't open, pop up an error:
    else {
        #if DEBUG
        Serial.print("error opening ");
        Serial.println(outputFile);
        #endif
    }
}

//Initializes BNO
void initializeBNO() {
    // initializes BNO and checks if there is a connection
    if (!bno.begin()) {
        /* There was a problem detecting the BNO055 ... check your connections */
        #if DEBUG
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        #endif
        while (1) {
            ;
        }
    }

}
