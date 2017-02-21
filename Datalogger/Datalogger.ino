/**
 * @Author: Yichen Xu
 * @Date:   09-Feb-2017 19:02:46
 * @Email:  ichi@u.northwestern.edu
 * @Last modified by:   Yichen Xu
 * @Last modified time: 09-Feb-2017 19:02:52
 */

#include "init.h"
#include "dataCollection.h"
#include "dataWriter.h"


// if debugging, set to 1, otherwise set to 0
#define DEBUG 1


// ================= FUNCTION DECLARATIONS =================

// PIN DELCARATIONS
int chipSelect = BUILTIN_SDCARD;

int xInput = 1;
int yInput = 2;
int zInput = 3;

int buttonPin = 34;
int speedPin = 35;
int motorPin = 36;
int collectPin = 37;
int calibrationPin = 38;
int pulsePin = 39;
int builtInLED = 40;

// ============VARIABLE DECLARATION============================

int prevPos;
int prevEI;
int turnLeft = 720;

// I forgot what I made mode do lol
int MODE = 0;

// Connect x, y, z to 7, 8, 9, respectively


// min/max values for each axis
// get from calibration sketch
int xMin = 498;
int xMax = 512;

int yMin = 497;
int yMax = 512;

int zMin = 498;
int zMax = 512;

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
    delay(1000); // idk just leave this in
    pinMode(buttonPin, INPUT);
    pinMode(pulsePin, INPUT);
    pinMode(builtInLED, OUTPUT);
    pinMode(calibrationPin, OUTPUT);
    pinMode(motorPin, OUTPUT);
    pinMode(collectPin, OUTPUT);

    // slow output console if debugging
    if (DEBUG) {
        delayTime = 1000000;
    }

    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    initializeBNO();

    initializeAccel();

    initializeSD();

    attatchInterrupt(buttonPin, pause, rising);
}

// ISR that we call every time period to get d
long postLaunchCount = 0;
bool isLaunched = false;
double accel_vector = 0;

void pause() {
    running != running;
    if (running) {
        newFile();
    }
}

uint32_t timePrev;
void loop() {
    while (!running) {
        digitalWrite(collectPin, LOW);
    }
    DataSet data = getData();

    if (data.timestamp != timePrev) {
        writeData(&data);
        timePrev = data.timestamp;


        if (isLaunched == false) {
            accel_vector = sqrt(pow(data.bAccel.x, 2) + pow(data.bAccel.y, 2) + pow(data.bAccel.z, 2));
        }
        if (accel_vector > 3) {
            isLaunched = true;
        }
        if (isLaunched) {
            postLaunchCount++;

            if (postLaunchCount <= 175) { // when
                analogWrite(37, 255);  // turn on motor (and for this test, change direction to 1 and torque to 255)
                // analogWrite(39, 255);  //set direction
                // analogWrite(40, 255);  //set max torque limit
                analogWrite(38, 0);   // set speed 0

            }
            else if (175 < postLaunchCount < 325) {
                // digitalWrite(37, HIGH);  //turn on motor
                // digitalWrite(39, LOW);  //set direction
                // analogWrite(40, 255);  //set max torque limit
                analogWrite(38, 128); // set speed half
            }
            else if (325 <= postLaunchCount < 475) {
                // digitalWrite(37, HIGH);  //turn on motor
                // digitalWrite(39, LOW);  //set direction
                // analogWrite(40, 255);  //set max torque limit
                analogWrite(38, 255); // set speed full
            }
            else {
                analogWrite(37, 0);   // turn off motor
                // digitalWrite(39, LOW);  //set direction
                // analogWrite(40, 0);    //set max torque limit
                analogWrite(48, 0);   // set speed full
                isLaunched = false;  // stop checking time
            }
        }
    }

}
