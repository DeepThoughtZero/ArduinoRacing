/*
  SonarMeasurements.h - Library for ultrasonic measurements
*/
#ifndef SonarMeasurements_h
#define SonarMeasurements_h

#include "Arduino.h"

class SonarMeasurements {
  private:
    int servoAngle;	// in Deg
    const int static servoMinDegrees = 20; //10;
    const int static servoMaxDegrees = 90; //100;
    int trigPin;
    int echoPin;
    long measuredDistances[servoMaxDegrees - servoMinDegrees + 1];
  public:
    SonarMeasurements();
    SonarMeasurements(int angle);
    void sensorSetup(int sonartrigPin, int sonarEchoPin);
    int getCurrentServoAngle(void);
    void setCurrentServoAngle(int angle);
    int getServoMinDegrees(void);
    int getServoMaxDegrees(void);
    long measureDistance(void);
    long * getMeasuredDistances(void);
    long getMinMeasuredDistance(void);
    void setMeasuredDistance(int angle, long distance);
    long getMeasuredDistanceInDirection(int angle);
};

#endif
