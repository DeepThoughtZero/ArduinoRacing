// -----LIBRARIES

#include <Servo.h>
#include <SonarMeasurements.h>

// use minicom in Ubuntu terminal

// ----CONSTANTS

const boolean VERBOSE_DEBUG = false;

const int onBoardLedPin =  13;      // the pin numbers for the LEDs

const int servoPin = 7; // the pin number for the servo signal

const int onBoardLedInterval = 2000; // number of millisecs between blinks

const int servoInterval = 25; //50;  // millisecs between servo actions
const int sonarInterval = servoInterval; // millisecs between sonar measurements
const int driveInterval = 100; // millisecs between drive actions

const int sonarTrigPin = 8;
const int sonarEchoPin = 9;

// Setup Motor A pins
int enableL = 11; //1;
int pinL1 = 10;//0; //1; //3;
int pinL2 = 2;
// Setup Motor B pins
int enableR = 3; //10; //6;
int pinR1 = 5;
int pinR2 = 4;

const int maxDrivePWM = 110; //100; //80; //70;
const int minDrivePWM = 50; // With this PWM ratio, the vehicle is hardly moving. 

const int criticalObstacleDistance = 100; //120; //100;
const int dangerousObstacleDistance = 55; //45; //40; //30;
const int disastrousObstacleDistance = 35; //20; //30;


//------- VARIABLES

byte onBoardLedState = LOW;             // used to record whether the LEDs are on or off

Servo myservo;  // create servo object to control a servo 

int servoPosition = 90;     // the current angle of the servo - starting at 90.
int servoDegrees = 1;       // amount servo moves at each step (This must be 1 to make the rest of the code work.)
                           //    will be changed to negative value for movement in the other direction
                           


unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
unsigned long previousOnBoardLedMillis = 0;   // will store last time the LED was updated
unsigned long previousServoMillis = 0; // the time when the servo was last moved
unsigned long previousSonarMillis = 0; // the time when the sonar was last used
unsigned long previousDriveMillis = 0; // the time when the sonar was last used

SonarMeasurements sonarMeasurements;
 
 

//========

void setup() {

 Serial.begin(9600);
 Serial.println("Starting LookAround.ino");  // so we know what sketch is running
 
 pinMode(onBoardLedPin, OUTPUT);
 
 sonarMeasurements.sensorSetup(sonarTrigPin, sonarEchoPin);

 // waste some time and blink LED meanwhile
 digitalWrite(LED_BUILTIN,HIGH);
 delay(onBoardLedInterval);
 digitalWrite(LED_BUILTIN,LOW);
 delay(onBoardLedInterval);
 digitalWrite(LED_BUILTIN,HIGH);
 delay(onBoardLedInterval);
 digitalWrite(LED_BUILTIN,LOW);
 delay(onBoardLedInterval);
 
 myservo.write(servoPosition); // sets the initial position
 myservo.attach(servoPin);

   pinMode (enableL, OUTPUT);
   pinMode (pinL1, OUTPUT);
   pinMode (pinL2, OUTPUT);
   pinMode (enableR, OUTPUT);
   pinMode (pinR1, OUTPUT);
   pinMode (pinR2, OUTPUT);
 /*
 motorLforward();
 delay(onBoardLedInterval);
 motorLbackward();
 delay(onBoardLedInterval);
 motorLoff();
 delay(onBoardLedInterval);
 */
}



//=======

void loop() {

     // Notice that none of the action happens in loop() apart from reading millis()
     //   it just calls the functions that have the action code

 currentMillis = millis();   // capture the latest value of millis()
                             //   this is equivalent to noting the time from a clock 
 
 servoSweep();
 measDistance();
 driveAction();

}

//========


void servoSweep() {

     // this is similar to the servo sweep example except that it uses millis() rather than delay()

     // nothing happens unless the interval has expired
     // the value of currentMillis was set in loop()
 
 if (currentMillis - previousServoMillis >= servoInterval) {
       // its time for another move
   previousServoMillis += servoInterval;
   
   servoPosition = servoPosition + servoDegrees; // servoDegrees might be negative

   if ((servoPosition >= sonarMeasurements.getServoMaxDegrees()) || (servoPosition <= sonarMeasurements.getServoMinDegrees()))  {
         // if the servo is at either extreme, change the sign of the degrees to make it move the other way
     servoDegrees = - servoDegrees; // reverse direction
         // and update the position to ensure it is within range
     servoPosition = servoPosition + servoDegrees; 
   }
       // make the servo move to the next position
   myservo.write(servoPosition);
 }
}


void measDistance() {
  if (currentMillis - previousSonarMillis >= sonarInterval) {
    previousSonarMillis += sonarInterval;
    
    long distance = sonarMeasurements.measureDistance(); 
    
     int currentServoAngle = servoPosition; // - (sonarMeasurements.getServoMaxDegrees() - sonarMeasurements.getServoMinDegrees())/2; 
     if (currentServoAngle < 0) {
       currentServoAngle = 0;
       Serial.println("currentServoAngle < 0");
     }

    sonarMeasurements.setCurrentServoAngle(currentServoAngle);
    sonarMeasurements.setMeasuredDistance(currentServoAngle, distance); 
    
    /*
    Serial.print(sonarMeasurements.getCurrentServoAngle()); 
    Serial.println("Grad: ");
    
    Serial.print(distance);
    Serial.println(" cm");
    */
    
    if (distance < criticalObstacleDistance) {  // This is where the LED On/Off happens
      digitalWrite(LED_BUILTIN,HIGH); 
    }
    else {
      digitalWrite(LED_BUILTIN,LOW);
    }

  }
}


void driveAction() {
  int curvePWM;
  
  if (currentMillis - previousDriveMillis >= driveInterval) {
    previousDriveMillis += driveInterval;
   
   /*
   Serial.println("sonarMeasurements.getMinMeasuredDistance()=");
   Serial.println(sonarMeasurements.getMinMeasuredDistance());
   */
   
   int currentServoAngle = sonarMeasurements.getCurrentServoAngle();
   long currentDistance = sonarMeasurements.getMeasuredDistanceInDirection(currentServoAngle);
   
   
   if(currentDistance<disastrousObstacleDistance){
     if (VERBOSE_DEBUG) {
       Serial.println("b"); // back
     } else {
       motorLbackward(maxDrivePWM * 0.7);    
       motorRbackward(maxDrivePWM * 0.7);
     }
   }
   else if (currentDistance<dangerousObstacleDistance){
     if (VERBOSE_DEBUG) {
       Serial.println("r"); // rotate
     } else {
       if (currentServoAngle<(sonarMeasurements.getServoMinDegrees()+sonarMeasurements.getServoMaxDegrees())/2){ 
         motorRforward(maxDrivePWM * 0.8);
         motorLbackward(maxDrivePWM * 0.8);
       } else{  
         motorRbackward(maxDrivePWM * 0.8);
         motorLforward(maxDrivePWM * 0.8);
       }
     }
   
   }
   else if (currentDistance<criticalObstacleDistance){ 
     if (VERBOSE_DEBUG) {
       Serial.println("c"); // curve
     } else {
       curvePWM = minDrivePWM + (maxDrivePWM - maxDrivePWM) * currentDistance/criticalObstacleDistance * currentDistance/criticalObstacleDistance;
       //curvePWM = minDrivePWM + (maxDrivePWM - maxDrivePWM) * currentDistance/criticalObstacleDistance;
       if (currentServoAngle<(sonarMeasurements.getServoMinDegrees()+sonarMeasurements.getServoMaxDegrees())/2){ 
         motorLforward(curvePWM);
         motorRforward(maxDrivePWM);
       } else{
         motorLforward(maxDrivePWM);
         motorRforward(curvePWM);
       }
     }
   }
   else {
     if (VERBOSE_DEBUG) {
       Serial.println("f"); // forward
     } else {
       motorLforward();
       motorRforward();
     }
   }
        
  }    
}

// Create motor functions
void motorLforward() {
 analogWrite(enableL, maxDrivePWM); 
 digitalWrite (pinL1, HIGH);
 digitalWrite (pinL2, LOW);
}
void motorRforward() {
 analogWrite(enableR, maxDrivePWM); 
 digitalWrite (pinR1, HIGH);
 digitalWrite (pinR2, LOW);
}
void motorLforward(int tgtPWM) {
 analogWrite(enableL, tgtPWM); 
 digitalWrite (pinL1, HIGH);
 digitalWrite (pinL2, LOW);
}
void motorRforward(int tgtPWM) {
 analogWrite(enableR, tgtPWM); 
 digitalWrite (pinR1, HIGH);
 digitalWrite (pinR2, LOW);
}
void motorLbackward() { 
 analogWrite(enableL, maxDrivePWM); 
 digitalWrite (pinL1, LOW);
 digitalWrite (pinL2, HIGH);
}
void motorRbackward() {
 analogWrite(enableR, maxDrivePWM); 
 digitalWrite (pinR1, LOW);
 digitalWrite (pinR2, HIGH);
}
void motorLbackward(int tgtPWM) { 
 analogWrite(enableL, tgtPWM); 
 digitalWrite (pinL1, LOW);
 digitalWrite (pinL2, HIGH);
}
void motorRbackward(int tgtPWM) {
 analogWrite(enableR, tgtPWM); 
 digitalWrite (pinR1, LOW);
 digitalWrite (pinR2, HIGH);
}
void motorLstop() {
 digitalWrite (pinL1, HIGH);
 digitalWrite (pinL2, HIGH);
}
void motorRstop() {
 digitalWrite (pinR1, HIGH);
 digitalWrite (pinR2, HIGH);
}
void motorLcoast() {
 digitalWrite(enableL, LOW);
 digitalWrite (pinL1, LOW);
 digitalWrite (pinL2, LOW);
}
void motorRcoast() {
 digitalWrite(enableR, LOW);
 digitalWrite (pinR1, LOW);
 digitalWrite (pinR2, LOW);
}
void motorLon() {
 digitalWrite (enableL, HIGH);
}
void motorRon() {
 digitalWrite (enableR, HIGH);
}
void motorLoff() {
 digitalWrite (enableL, LOW);
}
void motorRoff() {
 digitalWrite (enableR, LOW);
}
