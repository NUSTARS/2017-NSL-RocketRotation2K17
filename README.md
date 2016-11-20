# RocketRotation2K17

---

##Code Layout


###Information Recovery

SD?

FRAM?

Into what, an automated matlab graphing thing?

Possibly filter raw data again 


###Feedback
---


Functions: 

1. DeterminePeakAcceleration: When accelerometer reaches peak acceleration, start everything else (use as interrupt to set off everything to begin: use boolean startkalman?)
2. DetermineInitialRotationSpeed: gets called at beginning and end of flight; use to find “zero” speed
3. KalmanFilter: do the kalman filter
4. DetermineOutputVoltage: input Kalman filter, and output voltage that will get us desired rotation?
5. StoreData: store the data into the SD/FRAM
6. DetermineApogee: when acceleration equals 0

---
KalmanFilter



Sensor Functions - possibly filter some of these depending on data received during testing 



Sensors, Fire them up, get data coming out, see if filtering needs to be done or how to get data back:

1. Accelerometer
2. Gyro
3. Barometer
4. Compass
5. GPS
6. Absolute Orientation
