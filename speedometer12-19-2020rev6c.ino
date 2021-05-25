/*
  speedometer12-19-2020rev6c.ino
  BMK & MKIE
  based on 12-14-2020

  Designed for two way engine travel (S1 to S2 or S2 to S1)
  Designed for cross track and reflected track IR sensors
  Designed for Audio playback
  Designed for Sensor LED
  Added audio exceed speed to System Monitor Panel

  MPH display
  Adafruit 0.56" 4-Digit 7-Segment Display w/I2C Backpack - White (Model: ADA1002)
  If MPH display does not function use the following sketch to test:
  File>Examples>Adafruit_LEDBackpack>sevenseg

  Timer display
  DIGITEN DC 12V Digital Timer Totalizer Hour Chronometer Industrial Meter Red (Model: HMT1201 )
  Use the folling sketch to test the timer display
  display_test.ino

  Addition information for general design can be found at:
  http://bobotstrains.blogspot.com/2017/03/supplemental-info-for-model-railroad.html
  Model Railroad Speedometer

  Additional information for the MPH display can be found at:
  https://learn.adafruit.com/adafruit-led-backpack/0-dot-56-seven-segment-backpack-arduino-setup
  Connect GND to common ground
  MPH dispaly connects to 3.3VDC (5V will also work)
  Connect VCC+ to power,  5V is best but 3V also seems to work for 3V microcontrollers.
  MPH display (CLK) connects to pin "A5"
  MPH display Data (DAT) connects to pin "A4"

    SCALE         SCALE to FOOT   PROPORTION
    O(17)         .266" (6.75mm)  1:45.2
    O,On3,On2     .250" (6.35mm)  1:48
    S,Sn3         .188" (4.76mm)  1:64
    OO            4.0mm (.1575")  1:76.2
    HO,HOn3,HOn2  3.5mm (.1378")  1:87.1
    TT, TTn3      .100" (2.54mm)  1:120
    N, Nn3        .075" (1.90mm)  1:160
    Z             .055" (1.39mm)  1:220

  See notes below for crosstrack and reflected IR operations.
  For crosstrack IR operations (output of IR is always LOW) change all (s1open to !s1open) and (s2open to !s2open).
  For crosstrack IR operations (output of IR is always LOW) change all (!s1open to s1open) and (!s2open to s2open).
  Mode function programming included for crosstrack IR sensors operations or reflected IR sensors operations
  Mode switch is connected to Pin 8 (Low for reflected track operations - LED pin 10 LOW)
*/

/*
 * Here we add in the various libraries required to operate the digital display from Adafruit and other
 * components. 
 */
#include "Adafruit_GFX.h"
#include "Adafruit_GrayOLED.h"
#include "Adafruit_SPITFT.h"
#include "Adafruit_SPITFT_Macros.h"
#include "gfxfont.h"
#include "Wire.h"
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();

/*Here we establish some variables to set distance between sensors and to set scale we need for the speedometer
 * to work. 
 */

float distance = 47.5;   /* Actual distance between sensors in inches. Place sensors, measure, and fill in here.*/
int scale = 87.1; /*Give in the scale of your model to calculate the scale speed: 
HO-Scale = 1:87.1
N-Scale = 1:160; 
O scale, use 1:48;
This list is not exhaustive. You simply need to give your scale's proportion.
*/

unsigned long restartDelay = 10000; /*This is the amount of time, given in milliseconds that your Arduino will delay without
responding to sensor input after completion of an operating cycle.  In this case, we have a 10 second, or 10,000 millisecond delay.
*/

int sensor1 = 2;   /*sensor1 digital pin*/
int sensor2 = 4;   /*sensor2 digital pin*/

int s1open = 1;   /*this initial value assumes that s1 is open and uncovered. When triggered or covered s1 will = 0*/
int s2open = 1;   /*this initial value assumes that s2 is open and uncovered. When triggered or covered s2 will = 0*/

int systemStop = 12;    /*Sets up the LED denoting system reset on pin 12*/
int displayReset = 11;  /*Sets timer reset control to pin 11*/
int display = 10;       /*Sets timer display start control to pin 10*/
int modeLED = 9;        /*Sets the mode function LED to pin nine*/

// Audio
int audioOne = 6;
int audioTwo = 7;
//Audio

//sensor LED's
int sensor1Led = 3;
int sensor2Led = 5;
int sensor1State = 0;
int sensor2State = 0;
//Sensor LED's

unsigned long startTime, stopTime, elapsedTime;
float elapsed, miles, hours, mph, scaleMPH;

//mode functions
int modeoperation = 8;
int Modesensor = 1;
//mode functions

void setup() {
  Serial.begin(9600);

  pinMode (display, OUTPUT);        /*Timer display start*/
  pinMode (displayReset, OUTPUT);   /*Timer display reset*/
  resetTimer();

  ///////////
  digitalWrite (systemStop, HIGH);
  ///////////

  pinMode (systemStop, OUTPUT);   /* System Restting LED*/
  pinMode(sensor1, INPUT);        /*Arduino Obstacle Avoidance Sensor (IR module)*/
  pinMode(sensor2, INPUT);        /*Arduino Obstacle Avoidance Sensor (IR module)*/

  //mode functions
  pinMode (modeoperation, INPUT);
  pinMode (modeLED, OUTPUT);
  //mode functions

  // Audio
  pinMode (audioOne, OUTPUT);
  pinMode (audioTwo, OUTPUT);
  // Audio

  // Sensor LED's
  pinMode (sensor1Led, OUTPUT);
  pinMode (sensor2Led, OUTPUT);
  // Sensor LED's

  matrix.begin(0x70);
  
  matrix.setBrightness(3);/*setBrightness(brightness)- will let you change the overall brightness of the entire 
                          display. 0 is least bright, 15 is brightest and is what is initialized by the display 
                          when you start*/
  matrix.print(8888);     /*clear display to show 8888 for checking display */
  matrix.writeDisplay();

  // Audio
  digitalWrite (audioOne, HIGH);  /*Audio trigger one*/
  digitalWrite (audioTwo, HIGH);  /* Audio trigger one*/
  // Audio

  //sensor LED
  digitalWrite (sensor1Led, HIGH);
  digitalWrite (sensor2Led, HIGH);
  //Sensor LED
}

enum  /*list of values that are distinct*/
{
  INIT,
  FWD_DETECT,
  REV_DETECT,
  TIMED,
  QUIET_WAIT,
  RESET
} state;

void resetTimer( void)
{
  digitalWrite (display, LOW);        /*Timer start trigger*/
  digitalWrite (displayReset, LOW);   /* Timer display reset on*/
  delay (30);                         /* delay to reset timer display*/
  digitalWrite (displayReset, HIGH);  /* Timer display reset off*/
}

void loop() {
  /*mode functions*/
  Modesensor = digitalRead(modeoperation);
  if (Modesensor == LOW) {
    digitalWrite (modeLED, LOW);
    delay (30);  /*for testing only

    mode functions
    ***********************************************************
    The following sketch code is for reflected IR operations*/

    unsigned long curms = millis();
    /*read respective sensor if not already triggered*/
    s1open = digitalRead(sensor1);
    s2open = digitalRead(sensor2);

    if (state == INIT)  {
      if (!s1open) state = FWD_DETECT;  /* The symbol "!" equial "NOT"*/
      else if (!s2open) state = REV_DETECT;
      else return;
      /*blinkRate(rate) - You can blink the entire display. 0 is no blinking.
      1, 2 or 3 is for display blinking.*/
      matrix.blinkRate(0);
      matrix.print(0.0);   /*clear display to show 0*/
      matrix.writeDisplay();
      resetTimer();
      startTime = curms;
      digitalWrite (display, HIGH);
      Serial.println ("Started");

      /*sensor LED's*/
      sensor1State = digitalRead(sensor1);
      if (sensor1State == HIGH) {
        digitalWrite (sensor1Led, HIGH);
      } else {
        digitalWrite (sensor1Led, LOW);
      }
      sensor2State = digitalRead(sensor2);
      if (sensor2State == HIGH) {
        digitalWrite (sensor2Led, HIGH);
      } else {
        digitalWrite (sensor2Led, LOW);
      }
      /*sensor LED's*/

    }
    else if (state == FWD_DETECT)
    {

      /*sensor LED's*/
      sensor2State = digitalRead(sensor2);
      if (sensor2State == HIGH) {
        digitalWrite (sensor2Led, HIGH);
      } else {
        digitalWrite (sensor2Led, LOW);
      }
      /*sensor LED's*/

      if (s2open) return;
      stopTime = curms;
      state = TIMED;
    }
    else if (state == REV_DETECT)
    {

      /*sensor LED's*/
      sensor1State = digitalRead(sensor1);
      if (sensor1State == HIGH) {
        digitalWrite (sensor1Led, HIGH);
      } else {
        digitalWrite (sensor1Led, LOW);
      }
      /*sensor LED's*/

      if (s1open) return;
      stopTime = curms;
      state = TIMED;
    }
    if (state == TIMED)
    {

      digitalWrite (display, LOW);
      Serial.println("Finished");
      elapsed = stopTime - startTime;
      Serial.print("Seconds: ");
      Serial.println(elapsed / 1000); /* This converts the elapsed time from milliseconds to seconds*/
      /*miles = distance in inches / 63360 inches to the mile; miles
      hours = elapsed time in milliseconds / 3,600,000 milliseconds in an hour; hours
      mph = miles / hours*/

      mph = distance * 56.818181 / elapsed;

      scaleMPH = mph * scale;
      Serial.print("Scale MPH: ");
      Serial.println(scaleMPH);
      Serial.println ();

      //slow blink if between 50 and 69 scale MPH
      //fast blink if over 70 scale MPH
      if ((scaleMPH >= 50) && (scaleMPH < 70)) {
        // blinkRate(rate) - You can blink the entire display.
        // 0 is no blinking. 1, 2 or 3 is for display blinking.
        matrix.blinkRate(3);

        ///// Audio
        digitalWrite (audioOne, LOW); // Audio trigger one
        delay (500);// Audio trigger one
        digitalWrite (audioOne, HIGH);// Audio trigger one
        Serial.print("Level One Speed exceeded");
        Serial.println ();
        Serial.println ();
        /////Audio

      } else if (scaleMPH >= 70) {
        matrix.blinkRate(1);

        //// Audio
        digitalWrite (audioTwo, LOW); // Audio trigger two
        delay (500);// Audio trigger two
        digitalWrite (audioTwo, HIGH);// Audio trigger two
        Serial.print("Level Two Speed exceeded");
        Serial.println ();
        Serial.println ();
        ////Audio

      }

      // print scale MPH on display
      if (scaleMPH > 9999)
        scaleMPH = 9999;
      matrix.print(scaleMPH);
      matrix.writeDisplay();

      digitalWrite (systemStop, LOW);
      Serial.println ("System Paused");


      delay (restartDelay);
      // How long to wait before resuming checking
      state = QUIET_WAIT;
      startTime = curms;

    }
    if ( state == QUIET_WAIT)
    {

      if (!s1open || !s2open)
        startTime = curms;
      else if (curms - startTime > restartDelay)
        state = RESET;
    }

    if  (state == RESET)
    {
      // if wating to turn MPH off after reset
      // matrix.blinkRate(0);
      // matrix.print(0.0);   //clear display to show 0
      // matrix.writeDisplay();
      // resetTimer();
      Serial.println ("System Ready");
      Serial.println ();
      digitalWrite (systemStop, HIGH);

      // sensor LED's
      digitalWrite (sensor1Led, HIGH);
      digitalWrite (sensor2Led, HIGH);
      // sensor LED's
      state = INIT;
    }
  }
  // End reflected IR operation sketch code
  /////////////////
  else
    ///////////////////////

    //***********************************************************
    // The following sketch code is for crosstrack IR operations
  {

    //mode functions
    digitalWrite (modeLED, HIGH);
    delay (30);  // for testing only
    //mode functions

    unsigned long curms = millis();
    //read respective sensor if not already triggered
    s1open = digitalRead(sensor1);
    s2open = digitalRead(sensor2);

    if (state == INIT)  {
      if (s1open) state = FWD_DETECT;  // ! equial "NOT"
      else if (s2open) state = REV_DETECT;
      else return;
      //blinkRate(rate) - You can blink the entire display. 0 is no blinking.
      //1, 2 or 3 is for display blinking.
      matrix.blinkRate(0);
      matrix.print(0.0);   //clear display to show 0
      matrix.writeDisplay();
      resetTimer();
      startTime = curms;
      digitalWrite (display, HIGH);
      Serial.println ("Started");


      /////////////// sensor LED's
      sensor1State = digitalRead(sensor1);
      if (sensor1State == HIGH) {
        digitalWrite (sensor1Led, LOW);
      } else {
        digitalWrite (sensor1Led, HIGH);
      }
      sensor2State = digitalRead(sensor2);
      if (sensor2State == HIGH) {
        digitalWrite (sensor2Led, LOW);
      } else {
        digitalWrite (sensor2Led, HIGH);
      }
      ////////////sensor LED's


    }
    else if (state == FWD_DETECT)
    {


      ////////////sensor LED's
      sensor2State = digitalRead(sensor2);
      if (sensor2State == HIGH) {
        digitalWrite (sensor2Led, LOW);
      } else {
        digitalWrite (sensor2Led, HIGH);
      }
      ////////////sensor LED's


      if (!s2open) return;
      stopTime = curms;
      state = TIMED;
    }
    else if (state == REV_DETECT)
    {


      /////////////// sensor LED's
      sensor1State = digitalRead(sensor1);
      if (sensor1State == HIGH) {
        digitalWrite (sensor1Led, LOW);
      } else {
        digitalWrite (sensor1Led, HIGH);
      }
      /////////////// sensor LED's

      if (!s1open) return;
      stopTime = curms;
      state = TIMED;
    }
    if (state == TIMED)
    {
      digitalWrite (display, LOW);
      Serial.println("Finished"); //
      elapsed = stopTime - startTime;
      Serial.print("Seconds: "); //
      Serial.println(elapsed / 1000); //
      //miles = distance / 63360;   //miles
      //hours = elapsed / 3600;   //hours
      //mph = miles / hours;

      mph = distance * 56.818181 / elapsed;

      scaleMPH = mph * scale;
      Serial.print("Scale MPH: "); //
      Serial.println(scaleMPH); //
      Serial.println ();

      //slow blink if between 50 and 69 scale MPH
      //fast blink if over 70 scale MPH
      if ((scaleMPH >= 50) && (scaleMPH < 70)) {
        // blinkRate(rate) - You can blink the entire display.
        // 0 is no blinking. 1, 2 or 3 is for display blinking.
        matrix.blinkRate(3);

/////Audio
        digitalWrite (audioOne, LOW); // Audio trigger one
        delay (500);// Audio trigger one
        digitalWrite (audioOne, HIGH);// Audio trigger one
        Serial.print("Level One Speed exceeded");
        Serial.println ();
        Serial.println ();
//// Audio        

      } else if (scaleMPH >= 70) {
        matrix.blinkRate(1);

///// Audio        
        digitalWrite (audioTwo, LOW); // Audio trigger two
        delay (500);// Audio trigger two
        digitalWrite (audioTwo, HIGH);// Audio trigger two
        Serial.print("Level Two Speed exceeded");
        Serial.println ();
        Serial.println ();
//// Audio        
//// Audio
        
      }

      // print scale MPH on display
      if (scaleMPH > 9999)
        scaleMPH = 9999;
      matrix.print(scaleMPH);
      matrix.writeDisplay();

      digitalWrite (systemStop, LOW);
      Serial.println ("System Paused");

      delay(restartDelay);
      // How long to wait before resuming checking
      state = QUIET_WAIT;
      startTime = curms;

    }
    if ( state == QUIET_WAIT)
    {

      if (s1open || s2open)
        startTime = curms;
      else if (curms - startTime > restartDelay)
        state = RESET;
    }

    if  (state == RESET)
    {
      // if wating to turn MPH off after reset
      // matrix.blinkRate(0);
      // matrix.print(0.0);   //clear display to show 0
      // matrix.writeDisplay();
      // resetTimer();
      Serial.println ("System Ready");
      Serial.println ();
      digitalWrite (systemStop, HIGH);


      // sensor LED's
      digitalWrite (sensor1Led, HIGH);
      digitalWrite (sensor2Led, HIGH);
      // sensor LED's



      state = INIT;
    }

  }
  // End crossstrack IR operation sketch code
}
