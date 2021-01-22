
int led1=13;
int led2=12;
int led3=11;
int led4=10;

long onTime=200;
long transitionOn=50;
long transitionOff=125; 

unsigned long previousMillis = 0;

void setup() {
pinMode (led1, OUTPUT);
pinMode (led2, OUTPUT);
pinMode (led3, OUTPUT);
pinMode (led4, OUTPUT);

}

void loop() {
  unsigned long currentMillis = millis ();
  unsigned long timeDifference = (currentMillis - previousMillis);

 if (timeDifference <= onTime + transitionOff) {
      digitalWrite (led1, HIGH);
    }
  
    else if (timeDifference <= 4*onTime - transitionOn){
      digitalWrite (led1, LOW);
    }
    else if (timeDifference <= 4*onTime){
      digitalWrite (led1, HIGH);
    }

   if (timeDifference <= onTime - transitionOn){
      digitalWrite (led2, LOW);
    }
    else if (timeDifference <= 2*onTime+transitionOff){
      digitalWrite (led2, HIGH);
    }
    else if (timeDifference <= 4*onTime){
      digitalWrite (led2, LOW);
    }

    if (timeDifference <= 2*onTime - transitionOn){
      digitalWrite (led3, LOW);
    }
    else if (timeDifference <= 3*onTime + transitionOff){
      digitalWrite (led3, HIGH);
    }
    else if (timeDifference <= 4*onTime){
      digitalWrite (led3, LOW);
    }
   
    if (timeDifference <= transitionOff){
      digitalWrite (led4, HIGH);
    }
    else if (timeDifference <= 3*onTime - transitionOn){
      digitalWrite (led4, LOW);
    }
    else if (timeDifference <= 4*onTime){
      digitalWrite (led4, HIGH);
    }
    else {
      previousMillis = currentMillis;
    }
    
}
