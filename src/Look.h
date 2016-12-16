// Look Arduino library
//
// Copyright 2013 Dave Sieh
// See LICENSE.txt for details.

#ifndef _Look_h_
#define _Look_h_

#include "IrSensors.h"

enum ObstacleType {
  OBST_NONE,        // no obstacle detected
  OBST_LEFT_EDGE,   // left edge detected 
  OBST_RIGHT_EDGE,  // right edge detected
  OBST_FRONT_EDGE,  // edge detect at both left & right sensors
  OBST_FRONT,       // obstacle in front
  OBST_REAR         // obstacle behind 
};

enum LookDirection {
  DIR_LEFT,
  DIR_RIGHT,
  DIR_CENTER 
};

class SoftServo;
class IrSensors;
class PingSensor;

class Look {
 public:
  Look(SoftServo *sweepServo, IrSensors *sensors, PingSensor *pingSensor);

  void begin();
  boolean sensesObstacle(ObstacleType obstacle, int minDistance);
  int lookAt(LookDirection direction);

 private:
  boolean irEdgeDetect(IrSensor sensor);

  SoftServo *servo;
  IrSensors *irSensors;
  PingSensor *ping;
  const int servoAngles[3] = { 150, 30, 90 };
  const int servoDelay = 500;
};

#endif
