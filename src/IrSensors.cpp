// IrSensors Arduino library
//
// Copyright (c) 2013,2014 Dave Sieh
// See LICENSE.txt for details.

#include <Arduino.h>
#include "IrSensors.h"
#include "pspc_support.h"

#ifdef IRSENSORS_DEBUG
#define SENSOR_NAME(i) STRING_FROM_TABLE(sensorNames,i)

// Build up a string table for the sensor names
const char left_name[] PROGMEM = "Left";
const char center_name[] PROGMEM = "Center";
const char right_name[] PROGMEM = "Right";

PGM_P const sensorNames[] PROGMEM = {
  left_name,
  center_name,
  right_name
};
#endif

IrSensors::IrSensors(int leftPin, int centerPin, int rightPin) {
  irSensorPins[IrLeft] = leftPin;
  irSensorPins[IrCenter] = centerPin;
  irSensorPins[IrRight] = rightPin;
}

void IrSensors::begin() {
  for (int sensor = 0; sensor < NUM_IR_SENSORS; sensor++) {
    calibrate((IrSensor)sensor);
  }
}

boolean IrSensors::hasValidPin(IrSensor sensor) {
  return irSensorPins[sensor] >= 0;
}

boolean IrSensors::highReflectionDetected(IrSensor sensor) {
  boolean result = false; // default value

  if (! hasValidPin(sensor)) {
    return false;
  }

  int value = analogRead(irSensorPins[sensor]); // get IR light level

  if (value <= irSensorReflect[sensor]) {
    // Object detected (lower value means more reflection)
    result = true; 
#ifdef IRSENSORS_DEBUG
    if (! isDetected[sensor]) { 
      // Only print on initial detection
      Serial.print(SENSOR_NAME(sensor));         
      Serial.println(P(" object detected"));
    }
#endif
  }

  isDetected[sensor] = result; 
  return result;
}

boolean IrSensors::lowReflectionDetected(IrSensor sensor) {
  boolean result = false; // default value

  if (! hasValidPin(sensor)) {
    return false;
  }

  int value = analogRead(irSensorPins[sensor]); // get IR light level

  if (value >= irSensorEdge[sensor]) {
    // edge detected (higher value means less reflection)
    result = true; 
#ifdef IRSENSORS_DEBUG
    if (isDetected[sensor] == false) { 
      // only print on initial detection
      Serial.print(SENSOR_NAME(sensor));         
      Serial.println(P(" edge detected"));
    }
#endif
  }

  isDetected[sensor] = result; 
  return result;
}

void IrSensors::calibrate(IrSensor sensor) {

  if (hasValidPin(sensor)) {
    int ambient = analogRead(irSensorPins[sensor]); // get ambient level
    irSensorAmbient[sensor] = ambient; 

    // precalculate the levels for object and edge detection  
    irSensorReflect[sensor] = (ambient * (long)(100 - irReflectThreshold)) / 100;
    irSensorEdge[sensor]    = (ambient * (long)(100 + irEdgeThreshold)) / 100; 
  }
}

int IrSensors::getAmbientLevel(IrSensor sensor) {
  return irSensorAmbient[sensor];
}
