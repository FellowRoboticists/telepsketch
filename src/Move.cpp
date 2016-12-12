// Move Arduino Library
//
// Copyright (c) 2012 Michael Margolis
// Copyright (c) 2013,2014 Dave Sieh
// See LICENSE.txt for details

#include <Arduino.h>
#include "RobotMotor.h"
#include "Move.h"
#include "Look.h"
#include "pspc_support.h"

#ifdef MOVE_DEBUG
// Build up the string table for the states
#define STATE_NAME(i) STRING_FROM_TABLE(states,i)

// Build up a string table for the sensor names
const char left_state[] PROGMEM = "Left";
const char right_state[] PROGMEM = "Right";
const char forward_state[] PROGMEM = "Forward";
const char back_state[] PROGMEM = "Back";
const char rotate_state[] PROGMEM = "Rotate";
const char stop_state[] PROGMEM = "Stop";

PGM_P const states[] PROGMEM = {
  left_state,
  right_state,
  forward_state,
  back_state,
  rotate_state,
  stop_state
};
#endif

Move::Move(Look *look) {
  state = MOV_STOP;
  speed = 0;
  speedIncrement = 10;
  looker = look;
}

void Move::begin() {
  looker->begin();

  motorBegin(MOTOR_LEFT);
  motorBegin(MOTOR_RIGHT);
  stop();
}

boolean Move::isPathClear() {
  boolean isClear = true; // default return value if no obstacles

  if (getState() == MOV_FORWARD) {
    isClear = ! looker->sensesObstacle(OBST_FRONT, MIN_DISTANCE);
  }
  return isClear; 
}

void Move::left() {
  changeMoveState(MOV_LEFT);
  motorForward(MOTOR_LEFT,  0);
  motorForward(MOTOR_RIGHT, speed);
}

void Move::right() {
  changeMoveState(MOV_RIGHT);
  motorForward(MOTOR_LEFT,  speed);
  motorForward(MOTOR_RIGHT, 0);
}

void Move::forward() {
  changeMoveState(MOV_FORWARD);
  motorForward(MOTOR_LEFT,  speed);
  motorForward(MOTOR_RIGHT, speed);
}

void Move::backward() {
  changeMoveState(MOV_BACK);
  motorReverse(MOTOR_LEFT, speed);
  motorReverse(MOTOR_RIGHT, speed);  
}

void Move::rotate(int angle) {
  changeMoveState(MOV_ROTATE);

#ifdef MOVE_DEBUG
  Serial.print(P("Rotating ")); 
  Serial.println(angle);
#endif

  if (angle < 0) {
#ifdef MOVE_DEBUG
    Serial.println(P(" (left)")); 
#endif
    motorReverse(MOTOR_LEFT,  speed); 
    motorForward(MOTOR_RIGHT, speed);  
  } else if (angle > 0) {
#ifdef MOVE_DEBUG
    Serial.println(P(" (right)"));
#endif
    motorForward(MOTOR_LEFT,  speed);
    motorReverse(MOTOR_RIGHT, speed);
  }  

  int ms = rotationAngleToTime(angle);

  movingDelay(ms); 
  brake();   
}

void Move::stop() {
  changeMoveState(MOV_STOP);
  motorStop(MOTOR_LEFT);
  motorStop(MOTOR_RIGHT);
}

void Move::brake() {
  changeMoveState(MOV_STOP);
  motorBrake(MOTOR_LEFT);
  motorBrake(MOTOR_RIGHT);
}

void Move::setSpeed(int newSpeed) {
  motorSetSpeed(MOTOR_LEFT,  newSpeed) ;
  motorSetSpeed(MOTOR_RIGHT, newSpeed) ;
  speed = newSpeed; // save the value
}

void Move::slower(int decrement) {
#ifdef MOVE_DEBUG
  Serial.print(P(" Slower: ")); 
#endif

  if (speed >= speedIncrement + MIN_SPEED) {
    speed -= speedIncrement;
  } else {
    speed = MIN_SPEED;
  }

  setSpeed(speed); 
}

void Move::faster(int increment) {
#ifdef MOVE_DEBUG
  Serial.print(P(" Faster: "));
#endif
  speed += speedIncrement; 

  if (speed > 100) {
    speed = 100;
  }

  setSpeed(speed);      
}

long Move::rotationAngleToTime(int angle) {
  int fullRotationTime; // time to rotate 360 degrees at given speed

  if (speed < MIN_SPEED) {
    return 0; // ignore speeds slower then the first table entry
  }

  angle = abs(angle);

  if (speed >= 100) {

    fullRotationTime = rotationTime[NBR_SPEEDS - 1]; // the last entry is 100%

  } else { 

    // index into speed and time tables
    int index = (speed - MIN_SPEED) / SPEED_TABLE_INTERVAL ; 

    int t0 =  rotationTime[index];
    int t1 = rotationTime[index+1];    // time of the next higher speed 

    fullRotationTime = map(speed, speedTable[index], speedTable[index+1], t0, t1);

  }

  long result = map(angle, 0, 360, 0, fullRotationTime);

  return result; 
}

void Move::changeMoveState(MoveState newState) {
  if (newState != state) {
#ifdef MOVE_DEBUG
    Serial.print(P("Changing move state from ")); Serial.print(STATE_NAME(state));
    Serial.print(P(" to ")); Serial.println(STATE_NAME(newState));
#endif
    state = newState;
  } 
}

MoveState Move::getState() {
  return state;
}

void Move::timedMove(MoveState direction, int duration) {
#ifdef MOVE_DEBUG
  Serial.print(P("Timed move "));
#endif
  if (direction == MOV_FORWARD) {

#ifdef MOVE_DEBUG
    Serial.println(P("forward"));
#endif
    forward();    

  } else if (direction == MOV_BACK) {

#ifdef MOVE_DEBUG
    Serial.println(P("back"));
#endif
    backward();     

  } else {

#ifdef MOVE_DEBUG
    Serial.println(P("?"));
#endif

  }
    
  movingDelay(duration);

  stop();
}

void Move::movingDelay(long duration) {
  long startTime = millis();
  long elapsedTime = 0;
  
  while (elapsedTime < duration) {
    // function in Look module checks for obstacle in direction of movement 
    if (isPathClear()) { 
      if (state != MOV_ROTATE) {
	// rotate is only valid movement
#ifdef MOVE_DEBUG
	Serial.println(P("Stopping in moving Delay()")); 
#endif
	brake(); 
      }
    }  
    elapsedTime = millis() - startTime;
  }  
}

void Move::roam() {
  int forwardDistance = looker->lookAt(DIR_CENTER);

  if (forwardDistance == 0) { 
    // no sensor
    stop();
#ifdef MOVE_DEBUG
    Serial.println(P("No front sensor"));
#endif
    return;  // No point in continuing...
  }
  
  if (forwardDistance <= MIN_DISTANCE) {
    moveToAvoidObstacle();
  }
}

void Move::avoidEdge() {
  if (looker->sensesObstacle(OBST_FRONT_EDGE, MIN_DISTANCE)) {

#ifdef MOVE_DEBUG
    Serial.println(P("left and right sensors detected edge"));
#endif
    timedMove(MOV_BACK, 300);
    rotate(120);
    while (looker->sensesObstacle(OBST_FRONT_EDGE, MIN_DISTANCE)) {
      stop(); // stop motors if still over cliff
    }

  } else if (looker->sensesObstacle(OBST_LEFT_EDGE, MIN_DISTANCE)) {

#ifdef MOVE_DEBUG
    Serial.println(P("left sensor detected edge"));
#endif
    timedMove(MOV_BACK, 100);
    rotate(30);

  } else if (looker->sensesObstacle(OBST_RIGHT_EDGE, MIN_DISTANCE)) {

#ifdef MOVE_DEBUG
    Serial.println(P("right sensor detected edge"));
#endif
    timedMove(MOV_BACK, 100);
    rotate(-30); 

  }
}

int Move::chanceRotationAngle() {
  return ((random(300) % 2) == 0) ? 90 : -90;
}

int Move::appropriateRotationAngle(int leftDistance, int rightDistance) {
  int angle = 90; // Turn right
  if (leftDistance == rightDistance) {
    // Roll the dice to see which way we should turn...
    angle = chanceRotationAngle();
  } else if (leftDistance > rightDistance) {
    angle = -90; // Turn left
  }
  return angle;
}

int Move::moveToAvoidObstacle() {
  // Need to stop and look around to see if 
  // there is a clear path
  stop();

  // You learned this when you first got your driver's 
  // license: look both ways at a stop sign.
  int leftDistance  = looker->lookAt(DIR_LEFT);
  delay(500);
  int rightDistance = looker->lookAt(DIR_RIGHT);

  int maxDistance = max(leftDistance, rightDistance);

  if (maxDistance > CLEAR_DISTANCE) {

    // We got clearance. Rotate as appropriate and move on
    rotate(appropriateRotationAngle(leftDistance, rightDistance));
      
  } else if (maxDistance < (CLEAR_DISTANCE / 2)) {

    // If the maximum clearance we have is less than half
    // the acceptable clearance, move backwards for
    // one second, then turn around and go the other direction
    timedMove(MOV_BACK, 1000);
    rotate(-180); 

  } else {

    // The maximum clearance was more than half the acceptable
    // clearance. Go right or left depending on which had the
    // most clearance
    rotate(appropriateRotationAngle(leftDistance, rightDistance));
  }
}

