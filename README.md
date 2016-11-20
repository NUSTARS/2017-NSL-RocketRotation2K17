# RocketRotation2K17
Code Layout


Information Recovery
SD?
FRAM?
Into what, an automated matlab graphing thing?
Possibly filter raw data again 


Feedback


Functions: 
DeterminePeakAcceleration: When accelerometer reaches peak acceleration, start everything else (use as interrupt to set off everything to begin: use boolean startkalman?)
DetermineInitialRotationSpeed: gets called at beginning and end of flight; use to find “zero” speed
KalmanFilter: do the kalman filter
DetermineOutputVoltage: input Kalman filter, and output voltage that will get us desired rotation?
StoreData: store the data into the SD/FRAM
DetermineApogee: when acceleration equals 0


KalmanFilter


Sensor Functions - possibly filter some of these depending on data received during testing 


Sensors: Fire them up, get data coming out, see if filtering needs to be done or how to get data back
Accelerometer
Gyro
Barometer
Compass
GPS
Absolute Orientation
