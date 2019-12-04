/*
  SonarMeasurements.cpp - Library for ultrasonic measurements
*/

#include "SonarMeasurements.h"

SonarMeasurements::SonarMeasurements() {
  this->servoAngle = 0;
}

SonarMeasurements::SonarMeasurements(int angle) {
  this->servoAngle = angle;
}

void SonarMeasurements::sensorSetup(int sonartrigPin, int sonarEchoPin){
   this->trigPin = sonartrigPin;
   this->echoPin = sonarEchoPin;

   pinMode(sonartrigPin, OUTPUT);
   pinMode(sonarEchoPin, INPUT);
}

void SonarMeasurements::setCurrentServoAngle(int angle) {
   this->servoAngle = angle;
}

int SonarMeasurements::getCurrentServoAngle(void) {
   return this->servoAngle;
}

int SonarMeasurements::getServoMinDegrees(void){
   return this->servoMinDegrees;
}

int SonarMeasurements::getServoMaxDegrees(void){
   return this->servoMaxDegrees;
}

long SonarMeasurements::measureDistance(void){
    long duration, distance;
    digitalWrite(this->trigPin, LOW);  
    delayMicroseconds(2); 
    digitalWrite(this->trigPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(this->trigPin, LOW);
    // duration = pulseIn(this->echoPin, HIGH, 20000); // timeout allows 20000/2/29 = 345cm max measurement distance
    duration = pulseIn(this->echoPin, HIGH);
    distance = (duration/2) / 29.1; // in cm
    return distance;
}

long * SonarMeasurements::getMeasuredDistances(void) {
  return this->measuredDistances;
}

long SonarMeasurements::getMinMeasuredDistance(void){
  long minimum = 4200;
  long sum = 0;
  int average, size, c, location = 1;

  size = sizeof(measuredDistances)/sizeof(measuredDistances[0]);
 
  //for (c = 1; c < size; c++)
  for (c = 30; c < size-30; c++)
  {
    sum += measuredDistances[c];
    if (measuredDistances[c] < minimum)
    {
       minimum  = measuredDistances[c];
       location = c+1;
    }
  }
  average = sum / size;
  return minimum;
  //return measuredDistances[45];
  //return average;
}

long SonarMeasurements::getMeasuredDistanceInDirection(int angle) {
  return this->measuredDistances[angle-servoMinDegrees];
}

void SonarMeasurements::setMeasuredDistance(int angle, long distance) {
  this->measuredDistances[angle-servoMinDegrees] = distance;
}
