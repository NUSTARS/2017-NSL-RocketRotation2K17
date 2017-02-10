/**
* @Author: Yichen Xu
* @Date:   10-Feb-2017 15:02:71
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 10-Feb-2017 15:02:77
*/




#ifndef __BNORAWDATA__

#include "dependencies.h"

imu::Vector<3> getBNOrawAccel(Adafruit_BNO055 * bno);
imu::Vector<3> getBNOrawGyro(Adafruit_BNO055 * bno);

#endif
