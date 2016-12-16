// IrSensors Arduino library
//
// Copyright (c) 2013 Dave Sieh
// See LICENSE.txt for details.

#ifndef _IrSensors_h_
#define _IrSensors_h_

#define NUM_IR_SENSORS 3

enum IrSensor {
  IrLeft,
  IrCenter,
  IrRight
};

class IrSensors {
 public:

  /**
     Initialize with the pins for each of the IR Reflectance
     sensors connected. If there is a sensor that isn't 
     connected (center sensor, for example), just specify
     a -1 for the pin number.
   */
  IrSensors(int leftPin, int centerPin, int rightPin);

  /**
     Initializes the sensor system.
   */
  void begin();

  /**
     Returns true if the specified sensor has a valid pin.
   */
  boolean hasValidPin(IrSensor sensor);

  /**
     Returns true if the sensor detected a reflection
     higher than the ambient reflection.
   */
  boolean highReflectionDetected(IrSensor sensor);

  /**
     Returns true if the sensor detected a reflection
     lower than the ambient reflection.
  */
  boolean lowReflectionDetected(IrSensor sensor);

  int getAmbientLevel(IrSensor sensor);

 private:
  /**
     Calibrates the specified sensor 
   */
  void calibrate(IrSensor sensor);

  byte irSensorPins[NUM_IR_SENSORS];
  int irSensorAmbient[NUM_IR_SENSORS];
  int irSensorReflect[NUM_IR_SENSORS];
  int irSensorEdge[NUM_IR_SENSORS];
  boolean isDetected[NUM_IR_SENSORS];

  const int irReflectThreshold = 10; // % level below ambient to trigger reflection
  const int irEdgeThreshold    = 90; // % level above ambient to trigger edge

};

#endif
