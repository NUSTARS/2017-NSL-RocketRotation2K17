
#include "dataWriter.h"

void writeData(DataSet * d, int power) {
// make a string for assembling the d to log:
// output time,xAccel,yAccel,zAccel,OrienX,OrienY,OrienZ,angVx,AngVy,bnoAccelx,bnoAccely,bnoAccelz

    //calculate moving average speed of motor
    double avgSpeed = 0.000;
    for (int i = 0; i< 5; i++) {
        avgSpeed += motorSpeed[i];
    }
    avgSpeed = avgSpeed/5;

    //starting construction of datastring to write to SD
    String dataString = "";


    dataString += d->time;
    dataString += ",";
    dataString += d->orientation.x;
    dataString += ",";
    dataString += d->orientation.y;
    dataString += ",";
    dataString += d->orientation.z;
    dataString += ",";
    dataString += d->gyro.x; // lol dont ask me why there nees to be () here
    dataString += ",";
    dataString += d->gyro.y;
    dataString += ",";
    dataString += d->gyro.z;
    dataString += ",";
    dataString += d->bAccel.x;
    dataString += ",";
    dataString += d->bAccel.y;
    dataString += ",";
    dataString += d->bAccel.z;
    dataString += ",";
    dataString += d->c.sysCal;
    dataString += ",";
    dataString += d->c.gyroCal;
    dataString += ",";
    dataString += d->c.bAccelCal;
    dataString += ",";
    dataString += d->c.magCal;
    dataString += ",";
    dataString += power;
    dataString += ",";
    dataString += avgSpeed;
    dataString += ",";
    dataString += turnLeft;
    dataString += ",";
    dataString += pE;
    dataString += ",";
    dataString += iE;
    dataString += ",";
    dataString += dE;


// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another

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
