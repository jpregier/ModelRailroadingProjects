////////////////////////Four LED Stratolite Beacon Sketch using Millis//////////////////////////////
////////////////////////////////////By: James Regier////////////////////////////////////////////////
/////////////////////////////For Free Use and Modification//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//This sketch will set up a Stratolite four LED rotary beacon on an Arduino, as demonstrated in the 
//February 2021 What's Neat section of Model Railroad Hobbyist. Note that anything behind two or more
//forward slashes is not a vital part of the sketch -- dual forward slashes instruct the Arduino to
//ignore anything that follows on that line of programming -- but may contain useful explanations to
//help users understand the various portions of the sketch.  Feel free to modify and use as you like.
////////////////////////////////////////////////////////////////////////////////////////////////////

      //Assign four pins on the Arduino to four LEDs in the stratolight beacon.  
      //Name the pins for future reference in the sketch.  For the arduino, we will use pins 13, 12,
      //11, and 10 for led1, led2, led3, and led4, respectively. For the Attiny13, we will modify the
      //sketch so that led1=1, led2=2, led3=3, and led4=4, since the Attiny13 has only six programmable
      //pins.

int led1=13;
int led2=12;
int led3=11;
int led4=10;

     //Set the time constants for the onTime, transitionOn, and offTime in the beacon flash sequence.
     //The idea here is to attempt to capture not only the flash time in the beacon rotation, but to
     //simulate the warm up and cool down of incandescent bulbs.   
     //Assigning the constants here allows for easy tweaks and modifications later on, without having
     //to go through the entire sketch.

long onTime=200;
long transitionOn=50;
long transitionOff=125;

     //Set a variable (previousMillis) which will enable the Arduino to calculate amount of time between 
     //functions, based on internal timer.  We will set up the equation for this later on in the void 
     //loop.

unsigned long previousMillis = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//The void setup instructs the Arduino what to do with its various pins, whether to read (INPUT) or write
//(OUTPUT) data. The void setup runs only one time.

void setup() {
pinMode (led1, OUTPUT);
pinMode (led2, OUTPUT);
pinMode (led3, OUTPUT);
pinMode (led4, OUTPUT);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//The void loop instructs the arduino when to turn LEDs on or off.  The loop will cycle infinitely, so
//long that the Arduino or ATTINY13 has power.  After setting up the initial equations, we can treat each
//LED as a separate component of the program and progress it through the cycle using a series of
//if / else if / else statements.

void loop() {
unsigned long currentMillis = millis();  //millis is the internal timer of the Arduino. 
unsigned long timeDifference = (currentMillis - previousMillis);  //This equation coordinates the
                                                                  //Arduino's internal tymer with the
                                                                  //sequence.

if (timeDifference <= onTime + transitionOff) {
  digitalWrite (led1, HIGH);
}
else if (timeDifference <= 4*onTime - transitionOn) {
  digitalWrite (led1, LOW);
}
else if (timeDifference <= 4*onTime) {
  digitalWrite (led1, HIGH);
} 


if (timeDifference <= onTime - transitionOn){
  digitalWrite (led2, LOW);
}
else if (timeDifference <= 2 * onTime + transitionOff){
  digitalWrite (led2, HIGH);
}
else if (timeDifference <= 4 * onTime){
  digitalWrite (led2, LOW);
}




if (timeDifference <= 2 * onTime - transitionOn){
  digitalWrite (led3, LOW);
}
else if (timeDifference <= 3 * onTime + transitionOff){
  digitalWrite (led3, HIGH);
}
else if (timeDifference <= 4 * onTime){
  digitalWrite (led3, LOW);
}



if (timeDifference <= transitionOff){
  digitalWrite (led4, HIGH);
}
else if (timeDifference <= 3 * onTime - transitionOn) {
  digitalWrite (led4, LOW);
}
else if (timeDifference <= 4 * onTime) {
  digitalWrite (led4, HIGH);
}




else (previousMillis = currentMillis);
}
