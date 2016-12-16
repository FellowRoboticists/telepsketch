// Look Arduino library
//
// Copyright 2013 Dave Sieh
// See LICENSE.txt for details.

#include <Arduino.h>
#include "Look.h"
#include "SoftServo.h"
#include "IrSensors.h"
#include "PingSensor.h"
#include "pspc_support.h"

Look::Look(SoftServo *sweepServo, IrSensors *sensors, PingSensor *pingSensor) {
  servo = sweepServo;
  irSensors = sensors;
  ping = pingSensor;
}

void Look::begin() {
  if (servo) {
    servo->begin();
  }

  if (irSensors) {
    irSensors->begin();
  }

  if (ping) {
    ping->begin();
  }
}

boolean Look::irEdgeDetect(IrSensor sensor) {
  boolean detected = false;
  if (irSensors) {
    detected = irSensors->lowReflectionDetected(sensor);
  }
  return detected;
}

boolean Look::sensesObstacle(ObstacleType obstacle, int minDistance) {
  switch(obstacle) {
  case  OBST_FRONT_EDGE: 
    return irEdgeDetect(IrLeft) && irEdgeDetect(IrRight); 
  case  OBST_LEFT_EDGE:  
    return irEdgeDetect(IrLeft); 
  case  OBST_RIGHT_EDGE: 
    return irEdgeDetect(IrRight); 
  case  OBST_FRONT:     
    return lookAt(DIR_CENTER) <= minDistance;      
  }
  return false; 
}

int Look::lookAt(LookDirection direction) {
  int angle = servoAngles[direction];
  // wait for servo to get into position
  servo->write(angle, servoDelay);

  int distance = (ping) ? ping->getAverageDistance(4) : 0; // distaceToObstacle();

  if (angle != servoAngles[DIR_CENTER]) {
#ifdef LOOK_DEBUG
    // Print only if looking right/left
    Serial.print(P("looking at dir ")); 
    Serial.print(angle), Serial.print(P(" distance= ")); 
    Serial.println(distance); 
#endif
    
    // Re-center the servo
    servo->write(servoAngles[DIR_CENTER], servoDelay / 2);    
  } 

  return distance;   
}
