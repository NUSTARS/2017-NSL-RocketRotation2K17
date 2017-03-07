
#include "dataWriter.h"
void writeData(DataSet * d, int power) {
// make a string for assembling the d to log:
// output time,xAccel,yAccel,zAccel,OrienX,OrienY,OrienZ,angVx,AngVy,bnoAccelx,bnoAccely,bnoAccelz
    String dataString = "";

// get time
    dataString += d->time;
    dataString += ",";
    dataString += d->accel.x;
    dataString += ",";
    dataString += d->accel.y;
    dataString += ",";
    dataString += d->accel.z;
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

// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another
    File dataFile = SD.open(outputFile, FILE_WRITE);

// if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        
        // Serial.println(dataString);
    }
// if the file isn't open, pop up an error:
    else {
        Serial.print("error opening ");
        Serial.println(outputFile);
    }
}
