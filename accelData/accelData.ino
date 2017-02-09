/**
 * @Author: Yichen Xu
 * @Date:   19-Jan-2017 11:01:21
 * @Email:  ichi@u.northwestern.edu
 * @Last modified by:   Yichen Xu
 * @Last modified time: 19-Jan-2017 11:01:30
 */

#define GRAVITY 9.81

// set to 0 for Gs, 1 for m/s^2
const int MODE = 0;


// Connect x, y, z to 7, 8, 9, respectively
const int xInput = 7;
const int yInput = 8;
const int zInput = 9;

// min/max values for each axis
// get from calibration sketch
const int xMin = 244;
const int xMax = 512;

const int yMin = 278;
const int yMax = 512;

const int zMin = 266;
const int zMax = 512;

// WHOOO VARIABLES!
float xBase, yBase, zBase, xConv, yConv, zConv;
float xScaled, yScaled, zScaled;

// Initialize analog pin values
int xVal = 0;
int yVal = 0;
int zVal = 0;


void setup() {
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

    Serial.begin(38400);
}


void loop() {
    // Read pins
    xVal = analogRead(7);
    yVal = analogRead(8);
    zVal = analogRead(9);

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

    // TODO Write code displaying units depending on mode

    // Pprint data
    Serial.print(xVal);
    Serial.print(" ");
    Serial.print(xScaled);
    Serial.print(" ");
    Serial.print(yScaled);
    Serial.print(" ");
    Serial.println(zScaled);
    delay(100);
}
