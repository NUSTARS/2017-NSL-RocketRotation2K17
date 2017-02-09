/////////// BNO 055 declarations
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_GPS.h>

    
Adafruit_BNO055 bno = Adafruit_BNO055(55);

///////////// End BNO 055 declarations ////////




/////////////// GPS Declarations

// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada



// This sketch is ONLY for the Arduino Due!
// You should make the following connections with the Due and GPS module:
// GPS power pin to Arduino Due 3.3V output.
// GPS ground pin to Arduino Due ground.
// For hardware serial 1 (recommended):
//   GPS TX to Arduino Due Serial1 RX pin 19
//   GPS RX to Arduino Due Serial1 TX pin 18
#define mySerial Serial1

Adafruit_GPS GPS(&mySerial);


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;

void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy
////////////////////end gps declarations/////////////////////////////


//////////////////// accelerometer declarations ///////////////////////////////////
#define GRAVITY 9.81

// set to 0 for Gs, 1 for m/s^2
const int MODE = 0;


// Connect x, y, z to 7, 8, 9, respectively
const int xInput = 7;
const int yInput = 8;
const int zInput = 9;

// min/max values for each axis
// get from calibration sketch
const int xMin = 512;
const int xMax = 512;

const int yMin = 512;
const int yMax = 512;

const int zMin = 512;
const int zMax = 512;

// WHOOO VARIABLES!
float xBase, yBase, zBase, xConv, yConv, zConv;
float xScaled, yScaled, zScaled;

// Initialize analog pin values
int xVal = 0;
int yVal = 0;
int zVal = 0; 
//////////////////////// end accelerometer declarations /////////////////////////////
    









void setup(void) 
{


/////// Accelerometer setup ////////
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
////////end Accelerometer setup ////////////



/////////////// BNO 055 setup /////////
Serial.begin(9600);
Serial.println("Orientation Sensor Test"); Serial.println("");

/* Initialise the sensor */
if(!bno.begin())
{
  /* There was a problem detecting the BNO055 ... check your connections */
  Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  while(1);
}

delay(1000);
  
bno.setExtCrystalUse(true);
/////////////////End  BNO 055  setup /////////////////



/////////////// GPS Setup///////////////////////////////
 // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
 // also spit it out
 Serial.begin(115200);
 Serial.println("Adafruit GPS library basic test!");
 // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
 GPS.begin(9600);
 mySerial.begin(9600);
 
 // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
 GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
 // uncomment this line to turn on only the "minimum recommended" data
 //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
 // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
 // the parser doesn't care about other sentences at this time
 
 // Set the update rate
 GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
 // For the parsing code to work nicely and have time to sort thru the data, and
 // print it out we don't suggest using anything higher than 1 Hz
 // Request updates on antenna status, comment out to keep quiet
 / GPS.sendCommand(PGCMD_ANTENNA);
 // the nice thing about this code is you can have a timer0 interrupt go off
 // every 1 millisecond, and read data from the GPS for you. that makes the
 // loop code a heck of a lot easier!

#ifdef __arm__
  usingInterrupt = false;  //NOTE - we don't want to use interrupts on the Due
#else
  useInterrupt(true);
#endif

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);

//////////end GPS setup /////////////////////////


}

#ifdef __AVR__
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
    }
   void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}
#endif //#ifdef__AVR__

uint32_t timer = millis();


void loop(void) 
{
Serial.print("\n Accelerometer Data: \n")
read_accelerometer();
delay(1000);



////////////// BNO 055 loop //////////////////////////
/* Get a new sensor event */ 
sensors_event_t event; 
bno.getEvent(&event);

/* Display the floating point data */
Serial.print("X: ");
Serial.print(event.orientation.x, 4);
Serial.print("\tY: ");
Serial.print(event.orientation.y, 4);
Serial.print("\tZ: ");
Serial.print(event.orientation.z, 4);
Serial.println("");

delay(100);
//////////////// End BNO 055 loop ///////////////////////////


////////////// GPS loop ////////////////////////
   
// in case you are not using the interrupt above, you'll
// need to 'hand query' the GPS, not suggested :(
if (! usingInterrupt) {
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
}

// if a sentence is received, we can check the checksum, parse it...
if (GPS.newNMEAreceived()) 
{
  // a tricky thing here is if we print the NMEA sentence, or data
  // we end up not listening and catching other sentences! 
  // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
  //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

  if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
    return;  // we can fail to parse a sentence in which case we should just wait for another
}

// if millis() or timer wraps around, we'll just reset it
if (timer > millis())  timer = millis();
// approximately every 2 seconds or so, print out the current stats
if (millis() - timer > 2000) { 
timer = millis(); // reset the timer
 
  Serial.print("\nTime: ");
  Serial.print(GPS.hour, DEC); Serial.print(':');
  Serial.print(GPS.minute, DEC); Serial.print(':');
  Serial.print(GPS.seconds, DEC); Serial.print('.');
  Serial.println(GPS.milliseconds);
  Serial.print("Date: ");
  Serial.print(GPS.day, DEC); Serial.print('/');
  Serial.print(GPS.month, DEC); Serial.print("/20");
  Serial.println(GPS.year, DEC);
  Serial.print("Fix: "); Serial.print((int)GPS.fix);
  Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
if (GPS.fix) {
  Serial.print("Location: ");
  Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
  Serial.print(", "); 
  Serial.print(GPS.longitude, 4); Serial.println(GPS.lon); 
  Serial.print("Speed (knots): "); Serial.println(GPS.speed);
  Serial.print("Angle: "); Serial.println(GPS.angle);
  Serial.print("Altitude: "); Serial.println(GPS.altitude);
  Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
}
}
delay(100)
/////////////end GPS loop////////////////
}


void read_accelerometer(void){
/////////Accelerometer loop//////////////////
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
////// end accelerometer loop /////////////////
}

void bno_read(void){
////////////// BNO 055 loop //////////////////////////
/* Get a new sensor event */ 
sensors_event_t event; 
bno.getEvent(&event);

/* Display the floating point data */
Serial.print("X: ");
Serial.print(event.orientation.x, 4);
Serial.print("\tY: ");
Serial.print(event.orientation.y, 4);
Serial.print("\tZ: ");
Serial.print(event.orientation.z, 4);
Serial.println("");

delay(100);
//////////////// End BNO 055 loop ///////////////////////////
}