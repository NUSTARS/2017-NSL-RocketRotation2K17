/**
 * @Author: Yichen Xu
 * @Date:   19-Feb-2017 23:02:38
 * @Email:  ichi@u.northwestern.edu
 * @Last modified by:   Yichen Xu
 * @Last modified time: 19-Feb-2017 23:02:48
 */

#include "dataCollection.h"

// Gets data from every single sensor and stores it in a DataSet
DataSet getData() {
    DataSet tempData;
    sensors_event_t event;

    bno.getEvent(&event);
    tempData.time = event.timestamp;
    tempData.orientation = event.orientation;
    tempData.gyro = event.gyro;
    tempData.bAccel = event.acceleration;
    tempData.c = getCalibrationStatus();
    return tempData;
}

// Gets calibration data and stores it in the CalibrationData
CalibrationData getCalibrationStatus() {
    CalibrationData sensCal;
    
    bno.getCalibration(&(sensCal.sysCal), &(sensCal.gyroCal), &(sensCal.bAccelCal), &(sensCal.magCal));
    return sensCal;
}

// Gets acceleration data from accelerometer
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
