This is an automatic translation, may be incorrect in some places. See sources and examples!

# Gyverping
HC-SR04 ultrasound library
- Two operating modes: synchronous based on Pulsein and asynchronous at interruptions + micros
- Air temperature correction
- result in millimeters, accuracy ~ 1 mm (with filter)
- Two filtration steps: a ceremonial filter + exponential average
- integer calculations
- A separate tool for an asynchronous survey of an array of sensors

## compatibility
Compatible with all arduino platforms (used arduino functions)

## Content
- [use] (#usage)
- [versions] (#varsions)
- [installation] (# Install)
- [bugs and feedback] (#fedback)

<a id = "USAGE"> </A>

## Usage
### GPINGSYNC
Synchronous measurement based on `pulsein`

`` `CPP
// Indicate pins
GPINGSYNC (Uint8_t Trig, Uint8_t Echo);

// Perform measurement.True will return with success.The result can be taken through Getxxxx
Bool ping ();

// Perform measurement and return the raw result [mm]
uint16_t ping rank ();

// Perform measurement and return the median result [mm]
uint16_t pingFiltered ();

// Perform measurement and return the filtered result [mm]
// k = 2, 3, 4 ... the more - the smoother the filter
uint16_t pingsmooth (uint8_t k = 2);

// Get the average from n measurements with Dlavayms between them.Blocking call
Uint16_T Pingaverage (Uint8_t N, Uint16_T DELAYMS = 50);

// Set the air temperature [Celsius degrees]
VOID settemp (int8_t TEMP);

// get a raw distance from the last dimension [mm]
uint16_t getraft ();

// get a median distance from the last measurement [mm]
uint16_t getfilted ();

// get a filtered distance from the last dimension [mm]
// Each challenge filters the value more strongly
// k = 2, 3, 4 ... the more - the smoother the filter
uint16_t getsmooth (uint8_t k = 2);

// Configure the peak filter
// Thrsh - a threshold above which the deviation is considered a noise (silence 500 mm)
// n - a number exceeding the threshold of measurements in a row for the transition to a new range (silence 3)
Void Configfilter (Uint16_t Thrsh, Uint8_t N);
`` `

#### Example
`` `CPP
#include <gyverping.h>

#define hc_trig 3
#define hc_echo 2

GPINGSYNC SONAR (HC_TRIG, HC_ECHO);

VOID setup () {
Serial.Begin (115200);
}

VOID loop () {
// Open the Plotter for the output of the graph
sonar.ping ();
Serial.print (sonar.getraw ());
Serial.print (',');
Serial.print (sonar.getsmooth ());
Serial.print (',');
Serial.println (sonar.getfilted ());
DELAY (50);
}
`` `

## gpingsyncarray
GPINGSYNC array interviewer.Interviews asynchronously with a given period, the survey itself is synchronous

`` `CPP
GPINGSYNCARAY (GPINGSYNC* Arr, Uint8_t N, Uint16_T PRDMS = 50);

// ticker, call in LOOP.Will return True after a survey of all the sensors
Bool Tick ();

// Start measuring (launched by default)
VOID Start ();

// Stop measurements
VOID Stop ();

// Measurements are made
Bool Running ();

// The number of sensors
uint8_t leength ();

// access through []
GPINGSYNC & Operator [] (Uint8_T I);
`` `

#### Example
`` `CPP
#include <gyverping.h>

GPINGSYNC Arr [] = {gpingsync (3, 2), gpingsync (5, 4)};
GpinCranberries Gsyncarry Pings (Arr, 2);

VOID setup () {
Serial.Begin (115200);
}

VOID loop () {
// Open the Plotter for the output of the graph
if (pings.tick ()) {
for (uint8_t i = 0; i <pings.length (); i ++) {
Serial.print (pings [i] .getfilted ());
Serial.print (',');
}
Serial.println ();
}
}
`` `

## H Gpingisr
Asynchronous measurement based on micros

`` `CPP
// PIN TRIG
Gpingisr (uint8_t trig);

// Launch measurement
VOID Ping ();

// Call when changing the signal on the TRIG pin, for example, in the interruption by Change
VOID PINCHANGE ();

// Expected response from the sensor
Bool Waiting ();

// interrupt the current measurement
VOID ABORT ();

// ticker, call in LOOP.Will return True at the end of the measurement.The result can be taken through Getxxxx
Bool Tick ();

// Set the air temperature [Celsius degrees]
VOID settemp (int8_t TEMP);

// get a raw distance from the last dimension [mm]
uint16_t getraft ();

// get a median distance from the last measurement [mm]
uint16_t getfilted ();

// get a filtered distance from the last dimension [mm]
// Each challenge filters the value more strongly
// k = 2, 3, 4 ... the more - the smoother the filter
uint16_t getsmooth (uint8_t k = 2);

// Configure the peak filter
// Thrsh - a threshold above which the deviation is considered a noise (silence 500 mm)
// n - a number exceeding the threshold of measurements in a row for the transition to a new range (silence 3)
Void Configfilter (Uint16_t Thrsh, Uint8_t N);
`` `

#### Example
`` `CPP
#include <gyverping.h>

#define hc_trig 3

Gpingisr Sonar (HC_TRIG);

VOID setup () {
Serial.Begin (115200);

// Connect Echo PIN to Change interruptions
// In this case, PIN 2 (interruption 0)
Attachinterrupt (0, [] () {sonar.pinchange ();}, cheange);

// Start measuring
sonar.ping ();
}

VOID loop () {
// measurement is completed (in any case)
if (sonar.tick ()) {
Serial.print (sonar.getraw ());
Serial.print (',');
Serial.print (sonar.getsmooth ());
Serial.print (',');
Serial.println (sonar.getfilted ());

// We are waiting.In the real program, this should be an asynchronous timer
DELAY (50);

// Launch of the next measurement
sonar.ping ();
}
}
`` `

## H Gpingisrarray
GPingisr array interviewer

`` `CPP
Gpingisrarray (Gpingisr* Arr, Uint8_t N, Uint16_T PRDMS = 50);

// ticker, call in LOOP.Will return True after a survey of all the sensors
Bool Tick ();

// Start measuring (launched by default)
VOID Start ();

// Stop measurements
VOID Stop ();

// Measurements are made
Bool Running ();

// The number of sensors
uint8_t leength ();

// access through []
Gpingisr & Operator [] (uint8_t i);
`` `

#### Example
`` `CPP
#include <gyverping.h>

// TRIG Pins on 4 and 5
Gpingisr Arr [] = {gpingisr (4), gpingisr (5)};
Gpingisrarray Pings (Arr, 2);

VOID setup () {
Serial.Begin (115200);

// Echo Pins on 2 and 3 (Arduino Nano)
Attachinterrupt (0, [] () {pings [0] .pinchange ();}, chean);
Attachinterprept (1, [] () {pings [1] .pinchange ();}, chean);
}

VOID loop () {
// Open the Plotter for the output of the graph
if (pings.tick ()) {
for (uint8_t i = 0; i <pings.length (); i ++) {
Serial.print (pings [i] .getfilted ());
Serial.print (',');
}
Serial.println ();
}
}
`` `

<a ID = "Versions"> </a>

## versions
- V1.0

<a id = "Install"> </a>
## Installation
- The library can be found by the name ** gyverping ** and installed through the library manager in:
- Arduino ide
- Arduino ide v2
- Platformio
- [download the library] (https://github.com/gyverlibs/gyverps/archive/refs/heads/main.zip) .Zip archive for manual installation:
- unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
- unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
- unpack and put in *documents/arduino/libraries/ *
- (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read moreE detailed instructions for installing libraries[here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!

<a id = "Feedback"> </a>

## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code