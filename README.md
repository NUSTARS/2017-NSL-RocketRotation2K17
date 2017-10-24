# RocketRotation2K17



## Code Layout

The datalogger directory is what you will mainely need to be viewing. All the rest is mostly fluff used in testing or various calibration cycles. 

The kalman estimator can be useful in filtering out data from the noise, but in our experience, this is hardly neccessary. 

The code is split up into many different portions to hopefully make it easy to edit and understand.

1. Datalogger.ino

    - main function used to control everything and is what gets uploaded to the arduino/teensy
2. Adafruit_BNO055.cpp 

    - It is an almost carbon copy of the libary provided by Adafruit for the BNO055 chip, except with a few lines edited to provide us with better formatted data. Use of the original is generally okay, though may have large reprecussions on the rest of the code

3. dataCollection.cpp
    - Used to collect data from the sensors

4. dataWriter.cpp
    - Responsible for writing data to the SD card

5. init.cpp
    - Calles the initialization routines for the teensy and the BNO055

6. PID.cpp
    - The brains of the PID controller for the system. Controlls the motor output. Calculates the amount of power to provide to the motors 

Furthermore, the following include libraries are needed for the function of the program

- Teensy (not arduino iirc) SD library
- SPI
- Wire
- unility/imumaths.h from Adafruit, should be in BNO libary package
- adafruit sensor library
- stdio.h should be included by default



### Information Recovery

The information is stored as a .csv file that can be opened with excel. This file can easily be edited in excel as well to produce graphs. File names start with a standard prefix and go up with a numerical suffix. 


### Feedback

The feedback controller is a PID controller. It first generates a target function that we have set to be linear. It calculates the actual error between the current position and target. It stores this error for future use. A proportional constant calcualtes the proportional part of the PID output. A integrating constant is applied to the cumulative sum of the error over time, this allows it to achieve 0 steady state error. Finally a differentiating constant is applied to the difference between the current and previous errors to approximate future error change and slightly tone back the response. 


