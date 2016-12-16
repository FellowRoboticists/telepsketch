// PingSensor Arduino Library
//
// Copyright (c) 2013 Dave Sieh
// See LICENSE.txt for details.

#ifndef _PingSensor_h_
#define _PingSensor_h_

/**
   This class provides some methods to make it easy to get
   distance measurements from the Ping sensor. Simply
   initialize a PingSensor object with the pin to use. 
   That's it.
*/
class PingSensor {
 public:

  /**
     Which pin is the Ping sensor on?
  */
  PingSensor(int pingPin);

  void begin();

  /**
     Returns the distance in inches. It will return 0 if no
     Ping sensor is connected or the distance is
     greater than around 10 feet.
  */
  int getDistance();

  /**
     Returns the distance in inches. The distance returned is
     the result of taking a specified number of distance samples
     and averaging them.
   */
  int getAverageDistance(int numSamples);

 private:
  long microsecondsToCentimeters(long microseconds);

  int pin;
};

#endif
