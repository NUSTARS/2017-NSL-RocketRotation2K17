/**
* @Author: Yichen Xu
* @Date:   10-Feb-2017 15:02:56
* @Email:  ichi@u.northwestern.edu
* @Last modified by:   Yichen Xu
* @Last modified time: 10-Feb-2017 15:02:62
*/



#include "getBNORawData.h"


imu::Vector<3> getBNOrawAccel(Adafruit_BNO055 * bno) {
    return bno->getVector(Adafruit_BNO055::VECTOR_LINEACCEL);
}
imu::Vector<3> getBNOrawGyro(Adafruit_BNO055 * bno) {
    return bno->getVector(Adafruit_BNO055::VECTOR_EULER);

}
