
/*
 * Fast Clock Version 3.0
 * Colin H Murdoch - December 2022
 *
 * Modified December 2022
 * Amended to operate in conjuction with DCC-EX and pass clock time values over I2C
 * to allow time based operation of EXRAIL commands.
 *
 * Based on the original Written by Jim Gifford June 2017
 * COPYRIGHT © 2017 Jim Gifford
 * http://www.hallettcovesouthern.com/ - The website for my Layout
 * http://halletcovesouthern.blogspot.com.au - The Construction & Activities Blog
 *
 * Provided under a Creative Commons Attribution, Non-Commercial Share Alike,3.0 Unported License
 *
 * This version modified by Colin H Murdoch - March 2022
 * Modified to operate on an Arduino Uno with an McuFriend type TFT LCD Shield.
 *
 * Design Notes:
 *
 * This code is designed to use the Arduino Uno R3 with a 2.8" plug in shield TFT display.
 * On initialisation the clock is paused ready to set the start time. The start time can be adjusted
 * up and/or from the 06:00 preset with a preset fast clock ratio of 4:1.  The fast clock ratio can
 * be adjusted to 1, 2, 4, 6, 8, 12 or 30.  A reset function has been included. When ready to commence
 * operation the start/pause button is pressed.  the oroginal buttons have been replaced with on-screen buttons
 * set up using the Adafruit_GFX graphics library.
 *
 * The system can now use the EEPROM to hold the time value.  If Button 2 (Save) is prwssed, the clock is halted
 * and the values are saved to EEPROM.  on restart the clock loads up this time.  This can be overwritten
 * * with the Reset button.
 *
 * On screen buttons.
 *
 * Button 1   - Start/Pause
 * Button 2   - Save
 * Button 3   - Reset
 * Button 4   - +Time
 * Button 5   - -Time
 * Button 6   - Reset
 *
 *
 */


#include "EX-FastClock.h"
//#include "stdio.h"

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <LiquidCrystal_I2C.h>

Adafruit_7segment matrix = Adafruit_7segment();
LiquidCrystal_I2C lcd(0x27,40,2);

// only load the wire library if we transmit to CS
#ifdef SEND_VIA_I2C
    #include <Wire.h>
#endif

int buttons[7] = {2,3,4,5,6,7,8};
int buttonState[7];
int lastButtonState[7] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
unsigned long lastDebounceTime[7] = {0UL,0UL,0UL,0UL,0UL,0UL,0UL};

bool drawDots = false;

void printClock()
{
/*
    setBrightness(brightness)- will let you change the overall brightness of the entire display. 0 is least bright, 15 is brightest and is what is initialized by the display when you start
    blinkRate(rate) - You can blink the entire display. 0 is no blinking. 1, 2 or 3 is for display blinking.
*/
    matrix.setBrightness(4);
    matrix.writeDigitNum(0, (HH/10), drawDots);
    matrix.writeDigitNum(1, (HH%10), drawDots);
    matrix.drawColon(drawDots);
    matrix.writeDigitNum(3, (MM/10), drawDots);
    matrix.writeDigitNum(4, (MM%10), drawDots);
    matrix.writeDisplay();

    char buffer[6];
    lcd.setCursor(0,0);
    sprintf(buffer, "%02d:%02d", HH, MM);
    lcd.print(buffer);

}

void printText(char *Msg)
{
    lcd.setCursor(10,0);
    lcd.print(Msg);


}

#ifdef SEND_VIA_SERIAL
void SendTime(byte hour, byte mins, byte speed) {

  int itime = (hour * 60) + mins;
  Serial.print(F("<JC "));
  Serial.print( itime);
  Serial.print(F(" "));
  Serial.print((int)speed);
  Serial.print(F(">\n"));

}

#endif

void TimeCheck() {

  HH = ((startTime + runTime) / milPerHr) ;

    if (HH >= 24)
        {
          HD = (HH / 24);
          HH = (HH - (24 * HD));
        }


      MM = ((startTime + runTime) % milPerHr) / milPerMin;

    if (MM > 59)
        {
          MH = (MM / 60);
          MM = (MM - ( 60 * MH));
        }

}

void CheckClockTime() {
  drawDots = true;
//Serial.println("Clock Tick");

  if (currentMillis - lastMillis >= milPerSec) {  // cycle every second

    runTime = runTime + (clockSpeed * milPerSec);

    TimeCheck();

  lastMillis = currentMillis;

  if (MM != LastMinutes){
    LastMinutes = MM;

    printClock();

    #ifdef SEND_VIA_SERIAL
      SendTime(HH, MM, clockSpeed);
    #endif
  }

  }

}

void PauseClock() {

pausePlay = !pausePlay;

//Serial.print("PausePlay = ");
//Serial.println(pausePlay);

if (pausePlay == true)                   //  Clock paused
      {

        drawDots = false;
        printText("PAUSED");

      }

else
    {
        drawDots = true;
        printText("      ");


        #ifdef SEND_VIA_SERIAL
          //SendTime(HH, MM, clockSpeed);
        #endif
    }

}

void AdjustTime(byte OPT){

   if (!pausePlay){
     PauseClock();
    }
          // if runTime is > 2 minutes (120000 millis) adjust runTime if under 2 minutes startTime
          //  Increment by 15 Min "a press"
        switch (OPT) {
          case 1:
            if (runTime > 120000) {
              runTime = runTime + 900000;
            } else  {
              startTime = startTime + 900000;
            }
            break;
          case 2:
            if (runTime > 120000)
              {
                runTime = runTime - 900000;
              } else  {
                startTime = startTime - 900000;
              }
            break;
          default:
            break;
        }


        TimeCheck();


        printClock();

          #ifdef SEND_VIA_SERIAL
            //SendTime(HH, MM, clockSpeed);
          #endif

}

void displaySpeed(byte x) {

   clockSpeed = clockSpeeds[x];

    lcd.setCursor(0,1);
    char bufferB[10];
    sprintf(bufferB, "Speed: %d", clockSpeed);
    lcd.print(bufferB);
}


void AlterRate(){

    if (!pausePlay){
     PauseClock();
    }

    if (counter < 6)
    {
      counter++;
    } else {
      counter = 0;
    }
    displaySpeed(counter);

    currentMillis = millis();

}


void ResetAll(){

    if (!pausePlay){
     PauseClock();
    }

    startTime = 21600000;              //  default start time 06:00
    counter = 2;                       //  initial clock speed 4:1
    runTime = 0;                       //  Reset run time
    LastMinutes = 99;

    displaySpeed(counter);

    CheckClockTime();           // display the time


}

void SaveTime(){

    if (!pausePlay){
     PauseClock();
    }

    PauseTime.hour = HH;
    PauseTime.minute = MM;
    PauseTime.speed = counter;

    int eeAddress = 0;

    EEPROM.put(eeAddress, PauseTime);

}

void GetSavedTime(){

    int eeAddress = 0;
    EEPROM.get(eeAddress, PauseTime);

    // Check we have something valid from EEPROM
    if (PauseTime.hour > 0 && PauseTime.hour < 25){
      // we have a valid time so calculate start point
        startTime = (PauseTime.hour * milPerHr) + (PauseTime.minute * milPerMin);
        counter = PauseTime.speed;
      }
    else {
      // not valid so set defaults.  Either first use or EEPROM corrupt
        startTime = 21600000;           //  default start time 06:00
        clockSpeed = 4;                 //  initial clock speed 4:1

    }

    lastMillis = millis();          //  first reference reading of arduino O/S

}

#ifdef SEND_VIA_I2C
void TransmitTime() {
        // send the data over I2C
        // send the time as <mmmm> as two bytes followed by clockspeed
        int timetosend = (HH * 60) + MM;
        byte TimeArray[2];

        TimeArray[0] = (timetosend >> 8);
        TimeArray[1] = timetosend & 0xFF;
        Wire.write(TimeArray, 2);

        Wire.write(clockSpeed);

}
#endif



void setup()
{
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  #ifdef SEND_VIA_SERIAL
    Serial.begin(115200);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
  #endif

  #ifdef SEND_VIA_I2C       // only start the wire library if we transmit to CS
    //Serial.print("Starting Wire Library at address : ");
    //Serial.println(I2CAddress);
    Wire.begin(I2CAddress);
    Wire.onRequest(TransmitTime);
  #endif

  matrix.begin(0x70);

  for (int i=0; i<7; i++) {
    pinMode(buttons[i],INPUT_PULLUP);
  }

  currentMillis = millis();

  GetSavedTime();               // Read the EEPROM

  displaySpeed(counter);

  CheckClockTime();

  pausePlay = false;
  printText("     ");
  //Serial.println("Setup Finished");

}




void loop()
{

  if (pausePlay == false){
    CheckClockTime();
  } else {
    printClock();
  }

  for (int i=0; i<7; i++) {
    int reading = digitalRead(buttons[i]);
    if (reading != lastButtonState[i]) {
    // reset the debouncing timer
    lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
    if (reading != buttonState[i]) {
      buttonState[i] = reading;
      if (buttonState[i] == LOW) {
        ButtonPressed = (i+1);
      }
    }
    }
    lastButtonState[i] = reading;
  }

  switch (ButtonPressed){



    case 1:
        PauseClock();
      break;

    case 2:
        SaveTime();
      break;

    case 3:

        ResetAll();
      break;

    case 4:
        AdjustTime(1); // add time
      break;

    case 5:
        AdjustTime(2); // deduct time
      break;

    case 6:
        AlterRate();
      break;

  }

    ButtonPressed = 0;
    currentMillis = millis();


}



