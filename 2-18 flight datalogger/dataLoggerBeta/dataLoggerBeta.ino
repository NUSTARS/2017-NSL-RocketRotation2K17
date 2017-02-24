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
#define DEBUG      0
#define COLLECTPIN 11
#define CALPIN     17
#define OTHERPIN   14
#define SPEEDPIN   15

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
void newFile();
void writeData(DataSet * d);
sensors_vec_t getAccelData();
sensors_vec_t getOrientationData();
CalibrationData getCalibrationStatus();
DataSet getData();



// ============VARIABLE DECLARATION============================
// Pin select for on toggle switch
// Pin select for SD Card. DO NOT MODIFY
const int chipSelect = BUILTIN_SDCARD;

// I forgot what I made mode do lol
const int MODE = 0;

// Connect x, y, z to 7, 8, 9, respectively
const int xInput = 1;
const int yInput = 2;
const int zInput = 3;

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

bool running = false;

void setup() {
    pinMode(13, OUTPUT);
    pinMode(CALPIN, OUTPUT);
    pinMode(COLLECTPIN, OUTPUT);
    pinMode(11, INPUT);
    digitalWrite(13, HIGH);
    delay(1000); // idk just leave this in
    pinMode(6, INPUT);
    // slow output console if debugging
    if (DEBUG) {
        delayTime = 1000000;
    }

    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    initializeBNO();

    // initializeAccel();

    // initializeSD();

    // initialize interrupt timer MAKE SURE THIS IS LAST OR IDK WHAT HAPPENS
    while (!digitalRead(11)) {
        ;
    }

    Serial.println("Starting Data Acquisition");
    running = true;
    digitalWrite(COLLECTPIN, HIGH);
    while (digitalRead(23)) {
        ;
    }
    myTimer.begin(dataTick, delayTime);
}

// ISR that we call every time period to get d
long postLaunchCount = 0;
bool isLaunched = false;
double accel_vector = 0;

void dataTick() {

    DataSet data = getData();

    // writeData(&data);
    if (data.c.sysCal > 1) {
        digitalWrite(CALPIN, HIGH);
    }
    else {
        digitalWrite(CALPIN, LOW);
    }

    if (isLaunched == false) {
        accel_vector = sqrt(pow((data.bAccel.x()), 2) + pow((data.bAccel.y()), 2) + pow((data.bAccel.z()), 2));
        Serial.println(accel_vector);
    }
    if (accel_vector > 3 * 9.8) {
        isLaunched = true;
    }
    if (isLaunched) {
        postLaunchCount++;

        if (postLaunchCount <= 175) { // when
            analogWrite(OTHERPIN, 255);  // turn on motor (and for this test, change direction to 1 and torque to 255)
            // analogWrite(39, 255);  //set direction
            // analogWrite(40, 255);  //set max torque limit
            analogWrite(SPEEDPIN, 0);   // set speed 0
            Serial.println("stage1");

        }
        else if (175 < postLaunchCount && postLaunchCount < 325) {
            // digitalWrite(37, HIGH);  //turn on motor
            // digitalWrite(39, LOW);  //set direction
            // analogWrite(40, 255);  //set max torque limit
            analogWrite(SPEEDPIN, 128); // set speed half
            Serial.println("stage2");
        }
        else if (325 <= postLaunchCount && postLaunchCount < 475) {
            // digitalWrite(37, HIGH);  //turn on motor
            // digitalWrite(39, LOW);  //set direction
            // analogWrite(40, 255);  //set max torque limit
            analogWrite(SPEEDPIN, 255); // set speed full
            Serial.println("stage3");
        }
        else {
            analogWrite(OTHERPIN, 0);   // turn off motor
            // digitalWrite(39, LOW);  //set direction
            // analogWrite(40, 0);    //set max torque limit
            analogWrite(SPEEDPIN, 0);   // set speed full
            myTimer.end();  // stop checking time
            digitalWrite(COLLECTPIN, LOW);
            Serial.println("stagedone");
        }
    }
    if (digitalRead(11)) {
        delay(50);
        if (digitalRead(11)) {
            running = false;
            Serial.println("Stopping Data Aquisition");
            myTimer.end();
            digitalWrite(COLLECTPIN, LOW);
            while (digitalRead(11)) {
                ;
            }

            while (!running) {
                if (digitalRead(11)) {
                    delay(50);
                    if (digitalRead(11)) {
                        running = true;
                        digitalWrite(COLLECTPIN, HIGH);
                        while (digitalRead(11)) {
                            ;
                        }
                    }
                }
            }
            // newFile();
            Serial.println("Starting Data Aquisition");
            myTimer.begin(dataTick, delayTime);
        }
    }


}

void loop() {
    ;
}

void writeData(DataSet * d) {
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
    dataString += d->gyro.x(); // lol dont ask me why there nees to be () here
    dataString += ",";
    dataString += d->gyro.y();
    dataString += ",";
    dataString += d->gyro.z();
    dataString += ",";
    dataString += d->bAccel.x();
    dataString += ",";
    dataString += d->bAccel.y();
    dataString += ",";
    dataString += d->bAccel.z();
    dataString += ",";
    dataString += d->c.sysCal;
    dataString += ",";
    dataString += d->c.gyroCal;
    dataString += ",";
    dataString += d->c.bAccelCal;
    dataString += ",";
    dataString += d->c.magCal;

// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another
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

CalibrationData getCalibrationStatus() {
    CalibrationData sensCal;

    bno.getCalibration(&(sensCal.sysCal), &(sensCal.gyroCal), &(sensCal.bAccelCal), &(sensCal.magCal));
    return sensCal;
}
sensors_vec_t getOrientationData() {
    sensors_event_t event;

    bno.getEvent(&event);
    return event.orientation;
}

sensors_vec_t getAccelData() {
    // get analog Sensor d
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
    Serial.print("Filename: ");
    Serial.println(outputString);
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

    newFile();
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
