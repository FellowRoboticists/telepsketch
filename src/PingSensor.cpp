// PingSensor Arduino Library
//
// Copyright (c) 2013 Dave Sieh
// See LICENSE.txt for details.

#include <Arduino.h>
#include "PingSensor.h"

PingSensor::PingSensor(int pingPin) {
  pin = pingPin;
}

void PingSensor::begin() {
}

int PingSensor::getDistance() {
  /*
    The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
    Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  */
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);

  pinMode(pin, INPUT);

  // if a pulse does not arrive in 20 ms then the ping sensor is not connected
  long duration = pulseIn(pin, HIGH, 20000); 
  if (duration >= 20000) {
    duration = 10000;
  }

  // convert the time into a distance
  long cm = microsecondsToCentimeters(duration);

  return (cm * 10) / 25 ; // convert cm to inches
}

int PingSensor::getAverageDistance(int numSamples) {
  int distance;
  int samplesTaken;
  int cumulative;

  distance = samplesTaken = cumulative = 0;

  // Take samples of the distance to average later...
  for (int i=0; i < numSamples; i++) {  
    distance = getDistance();
    if (distance > 0) {
      samplesTaken++;
      cumulative += distance;
    }
  }

  return (samplesTaken > 0) ? distance = cumulative / samplesTaken : 0;
}


long PingSensor::microsecondsToCentimeters(long microseconds) {
  /*
    The speed of sound is 340 m/s or 29 microseconds per centimeter.
    The ping travels out and back, so to find the distance of the
    object we take half of the distance travelled.
  */
  return microseconds / 29 / 2;
}
