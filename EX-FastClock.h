#include <Arduino.h>

#include <EEPROM.h>

// include the specific clock configuration
#include "config.h"

// The following line governs the clock speeds available for selection
// Clockspeed of 1 = real time
// Clockspeed of 2 = half real time i.e. 1 min equiv. to 30 seconds
// Clockspeed of 4 = 1/4 real time i.e. 1 min equiv. to 15 seconds
// etc.
// These do not need to be changed but could be
byte clockSpeeds[] = {1, 2, 4, 6, 8, 12, 30};  //  Fast Clock Speeds

// The following line sets the default (startup) speed.

byte clockSpeed = 4;               //  Initial Fast Clock Speed Setting

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  15

unsigned long currentMillis = 0;  //  Current Millis value for comparison
unsigned long lastMillis = 0;     //  Last Millis for comparison
unsigned long startTime = 0;      //  Milliseconds since 00:00.000 to Session Start Time
unsigned long runTime = 0;        //  Milliseconds since Session Start Time

// Constants
const int milPerSec = 1000;       //  Milliseconds per Second
unsigned long milPerMin = 60000;  //  Milliseconds per minute
unsigned long milPerHr = 3600000; //  Milliseconds per Hour


byte debounceDelay = 75;           //  the debounce time; increase if the output flickers
byte counter = 2;                  //  for program logic
bool pausePlay = false;            //  clock state - running or paused

byte HH;                           //  Integer Hours
byte MM;                           //  Integer Minutes
byte HD;                           //  number of full Days since conting
byte MH;                           //  number of full hours since conting
byte SM;                           //  number of full minutes since conting

byte LastMinutes = 99;

byte ButtonPressed = 0;

struct MyTime {
  byte hour;
  byte minute;
  byte speed;
};

MyTime PauseTime;
