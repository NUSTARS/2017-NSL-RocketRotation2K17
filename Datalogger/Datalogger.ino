/**
   @Author: Yichen Xu
   @Date:   09-Feb-2017 19:02:46
   @Email:  ichi@u.northwestern.edu
   @Last modified by:   Yichen Xu
   @Last modified time: 09-Feb-2017 19:02:52
 */

#include "init.h"
#include "dataCollection.h"
#include "dataWriter.h"
#include "PID.h"






// ================= FUNCTION DECLARATIONS =================

// PIN DELCARATIONS
int chipSelect = BUILTIN_SDCARD;

int xInput = 21;
int yInput = 20;
int zInput = 19;


int buttonPin = 39; // input from button
int directionPin = 36; // controls direction
int speedPin = 37; // controls speed of motor
int torquePin = 38; // torque limit
int motorPin = 35; // turns on and off motor
int collectPin = 14; // led pin to see if its collecting data or not
int calibrationPin = 15; // turns on LED when calibrated
// int encoderPin = 16;
int pulsePin = 23; // taking data from encoder on motor
int builtInLED = 13;

// TODO fill these out
int directionReadPin = 20;
int encoderMarks = 24;
// ============VARIABLE DECLARATION============================

// PID Variables;
DataSet prevData;
DataSet currentData;
float kp = 1;
float ki = 0.00015;
float kd = .065; // 0.065


// I forgot what I made mode do lol
int MODE = 0;

volatile int encoderCounter;
volatile int encoderTime;
volatile float motorSpeed[5];
volatile uint8_t motorSpeedIndex;

int waitTime = 3000;

// =======Accel VARIABLES=======================

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


// ============BNO Declaration===============
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// ===================FILENAME SHENANIGANS=============
char outputFile[100];
String outputString;

// is this stuff running?
bool running = false;

// void pause();

void setup() {
    delay(1000); // idk just leave this in

    // initialize the digital pins
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(pulsePin, INPUT);
    pinMode(builtInLED, OUTPUT);
    pinMode(calibrationPin, OUTPUT);
    pinMode(motorPin, OUTPUT);
    pinMode(collectPin, OUTPUT);
    pinMode(torquePin, OUTPUT);
    pinMode(directionReadPin, INPUT);


    // Open serial communications and wait for port to open:
  #if DEBUG
    Serial.begin(9600);
    waitTime = 5000;
  #endif

    // Enable onboard LED to show that the program is running
    digitalWrite(builtInLED, HIGH);

    // Initalize all the sensors and SD Card
    initializeBNO();
    initializeAccel();   // This technically does nothign since we arent using the accelerometer
    initializeSD();

    // Turns motor on, and sets the torque to be 100 out of 255
    analogWrite(motorPin, 255);
    analogWrite(torquePin, 100);
    analogWrite(speedPin, 0);

    // Attatches interrupts for the buttons and pulses
    attachInterrupt(buttonPin, pause, FALLING);
    attachInterrupt(pulsePin, encoderReader, RISING);
}


// More varaibles needed for just the motor portion
uint32_t launchTimestamp = 0;
bool isLaunched = false;
double accel_vector = 0;
double launchGyro;

// function to pause and start the prgram when the button is pressed.
// Program starts off in the paused state
void pause() {
    // Debouncing Delay
    delay(50);

    if (!digitalRead(buttonPin)) {
        // Is not launched, tuns green LED off
        isLaunched = false;
        digitalWrite(calibrationPin, LOW);

        // toggle Running
        running = !running;

        #if DEBUG
        Serial.println("pls");
        #endif

        // If now running, make a new file and turn Red LED on
        if (running) {
            digitalWrite(collectPin, HIGH);
            newFile();
        }
        else {
            digitalWrite(collectPin, LOW);
        }
    }
}


// Reads the Encode for ticks. There are 12 ticks per revolution
void encoderReader() {
    // Temporary variable to store encoder time
    int tempTime = encoderTime;

    // get Encoder time in microseconds
    encoderTime = micros();

    // calculate time since last tik
    tempTime = encoderTime - tempTime;

    // counter for motor speed matrix. Used to calculate a moving average
    motorSpeedIndex = motorSpeedIndex % 5;

    // check direction of motor
    if (digitalRead(directionReadPin)) {

        // calculate speed for forward direction
        motorSpeed[motorSpeedIndex] = 1000000 / ((float) tempTime * encoderMarks);
    }
    else {
        // calculate for other direction
        motorSpeed[motorSpeedIndex] = -1000000 / ((float) tempTime * encoderMarks);
    }

    // increment Motor speed index
    motorSpeedIndex++;
}

// Main loop of the program
void loop() {

    #if DEBUG
    Serial.println("looping");
    #endif

    // Only run this if the button has been paused to unpause the code
    if (running) {

        // set prevData to be current old data and then grab new data
        prevData = currentData;
        currentData = getData();

        // makes sure the timestamps change. It always does
        if (currentData.time != prevData.time) {

            #if DEBUG
            // If in debug mode, print stuff, and also just launch at start of program without accelerometer trigger
            Serial.println(powerG);
            if (!isLaunched) {
                launchTimestamp = currentData.time;
            }
            isLaunched = true;
            #endif

            // write data gotten to SD card
            writeData(&currentData, powerG);

            // Accelerometer based launch trigger, set to when the entire system gets above 20 m/s^2 in all directions total
            if ((sqrt(pow(currentData.bAccel.x, 2) + pow(currentData.bAccel.y, 2) + pow(currentData.bAccel.z, 2)) > 20) && !isLaunched) {
                // sets Launched togged to true, and grabs the launch timestamp as the current timestamp
                isLaunched = true;
                launchTimestamp = currentData.time;
            }


            if (isLaunched) {
                digitalWrite(calibrationPin, HIGH);
                doTheThing(launchTimestamp);
            }


            // If time is greater than a limit set, kills the motor
            if (((currentData.time - launchTimestamp ) > (18000 + waitTime)) && isLaunched) {
                // Kills motor, stopping as fast as possible
                outputMotor(0);
                // stays in this mode until button is pressed. At which point program resets in theory
                while (isLaunched) {
                    //basically toggles green LED
                    digitalWrite(calibrationPin, !digitalRead(calibrationPin));
                    delay(500);
                }
            }

        }
    }


}
