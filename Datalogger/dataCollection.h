/**
* @Author: Yichen Xu
* @Date:   19-Feb-2017 23:02:39
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 19-Feb-2017 23:02:47
*/

#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H
#include "Datalogger.h"

// get accell data and return a weird vector
sensors_vec_t getAccelData();
//Honestly we didnt really use calibration status. We kinda just let it fly on hope
//That and we decided to just integrate the gyroscope which is always calibrated
CalibrationData getCalibrationStatus();
//Gets all the data. 
DataSet getData();

#endif
