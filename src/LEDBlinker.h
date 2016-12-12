// LEDBlinker Arduino Library
//
// Copyright (c) 2013 Dave Sieh
// See LICENSE.txt for details.
//

#ifndef _LEDBlinker_h_
#define _LEDBlinker_h_

/**
   This class provides methods to make it easy to notify end-users
   of stuff via an LED display. Simply initialize an LEDBlinker object with
   the pin to control. That's it.
 */
class LEDBlinker {
 public:
  /**
     Which pin to you want to blink on?
   */
  LEDBlinker(int ledPin);

  void begin();

  /**
     This will cause the LED to blink the number of times specified.
   */
  void blinkNumber(int number);

  /**
     Percent is the percent of on time time (duty cycle)
  */
  void flash(int percent);

  /**
     Flashes SOS in morse code.
  */
  void flashCritical();
 private:
  int pin;
};

#endif
