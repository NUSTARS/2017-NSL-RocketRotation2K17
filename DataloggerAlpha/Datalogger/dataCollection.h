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

sensors_vec_t getAccelData();
CalibrationData getCalibrationStatus();
DataSet getData();

#endif
