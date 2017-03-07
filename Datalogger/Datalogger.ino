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



// if debugging, set to 1, otherwise set to 0
#define DEBUG 1


// ================= FUNCTION DECLARATIONS =================

// PIN DELCARATIONS
int chipSelect = BUILTIN_SDCARD;

int xInput = 21;
int yInput = 20;
int zInput = 19;

int buttonPin = 39; // input from button
int directionPin = 35; //controls direction
int speedPin = 36; // controls speed of motor
int torquePin = 37; //torque limit
int motorPin = 38; // turns on and off motor
int collectPin = 14; // led pin to see if its collecting data or not
int calibrationPin = 15; // turns on LED when calibrated
int encoderPin = 16;
int pulsePin = 39; // taking data from encoder on motor
int builtInLED = 13;

// ============VARIABLE DECLARATION============================

// PID Variables;
DataSet prevData;
DataSet currentData;
float kp = 0.2;
float kd = 0.1;
float ki = 0.1;


// I forgot what I made mode do lol
int MODE = 0;




//=======Accel VARIABLES=======================

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


//============BNO Declaration===============
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//===================FILENAME SHENANIGANS=============
char outputFile[100];
String outputString;

//is this stuff running?
bool running = false;

//void pause();

void setup() {
  delay(1000); // idk just leave this in

  //initialize the digital pins
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pulsePin, INPUT);
  pinMode(builtInLED, OUTPUT);
  pinMode(calibrationPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(collectPin, OUTPUT);
  pinMode(torquePin, OUTPUT);


  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  digitalWrite(builtInLED, HIGH);

  initializeBNO();


  initializeAccel();

  initializeSD();

  attachInterrupt(buttonPin, pause, FALLING);
}


uint32_t launchTimestamp = 0;
bool isLaunched = false;
double accel_vector = 0;
double launchGyro;

void pause() {
  delay(50);
  if (!digitalRead(buttonPin)) {

  isLaunched = false;
  digitalWrite(calibrationPin, LOW);
  running = !running;
  Serial.println("pls");
    if (running) {
      digitalWrite(collectPin, HIGH);
      newFile();
    }
    else digitalWrite(collectPin, LOW);
  }
}

void loop() {
  /*
    if (digitalRead(buttonPin)) {
    while (digitalRead(buttonPin)) {
      ;
    }
    running = !running;
    if (!running) {
      digitalWrite(collectPin,LOW);
    }
    else {
      digitalWrite(collectPin,HIGH);
    }
    }
  */
  Serial.println("pls");
  if (running) {
    prevData = currentData;
    currentData = getData();




    if (currentData.time != prevData.time) {
      Serial.println(powerG);
      writeData(&currentData, powerG);
      if (sqrt(pow(currentData.bAccel.x, 2) + pow(currentData.bAccel.y, 2) + pow(currentData.bAccel.z, 2)) > 10 && !isLaunched) {
        isLaunched = true;
        launchTimestamp = currentData.time;
        
      }
      
      if (isLaunched) {
        digitalWrite(calibrationPin, HIGH);
        doTheThing(launchTimestamp);
      }
    }
  }


}
