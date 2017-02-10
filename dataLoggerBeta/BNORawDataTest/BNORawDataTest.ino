/**
 * @Author: Yichen Xu
 * @Date:   10-Feb-2017 14:02:50
 * @Email:  ichi@u.northwestern.edu
 * @Last modified by:   Yichen Xu
 * @Last modified time: 10-Feb-2017 14:02:56
 */



#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"
#include "utility/imumaths.h"

Adafruit_BNO055 bno = Adafruit_BNO055(55);
IntervalTimer sysClock;


void setup() {
    Serial.begin(9600);

    if (!bno.begin()) {
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1) {
            ;
        }
    }

    sysClock.begin(getData, 500000);
}

void getData() {
    imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    Serial.print("X: ");
    Serial.print(accel.x());
    Serial.print(" Y: ");
    Serial.print(accel.y());
    Serial.print(" Z: ");
    Serial.print(accel.z());
    Serial.println("");
    Serial.print("X: ");
    Serial.print(gyro.x());
    Serial.print(" Y: ");
    Serial.print(gyro.y());
    Serial.print(" Z: ");
    Serial.print(gyro.z());
    Serial.println("");
}

void loop() {
  ;
}


