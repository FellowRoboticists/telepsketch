#include <Packet.h>
#include <CommandPacket.h>

// Comment out the following if you want to stub out the
// movement stuff. Uncomment if this is the real robot.
// #define STUB_MINION true

#ifndef STUB_MINION

#include "RobotMotor.h"
#include "LEDBlinker.h"
#include <SoftServo.h>
#include "IrSensors.h"
#include "PingSensor.h"
#include "Look.h"
#include "Move.h"
#include <pspc_support.h>

#endif

#define SERIAL_BAUD_RATE 9600

#ifdef STUB_MINION

// Define the digital pins for the LEDs
#define FORWARD 13
#define BACKWARD 12
#define LEFT 11
#define RIGHT 10

#else

// Analog Pins
#define LEFT_IR_SENSOR_PIN 0
#define RIGHT_IR_SENSOR_PIN 1
#define CENTER_IR_SENSOR_PIN 2

// Digital Pins
#define PING_SENSOR_PIN 10

#define SWEEP_SERVO_PIN 9
#define LED_PIN 13
#endif

// The incoming commands
#define DRIVE 0x89

#define COMMAND_PACKET_LENGTH 5

// The sensor output looks like:
// START_BYTE LEN_BYTE SENSOR_TYPE SENSOR_VALUE1 [SENSOR_VALUE2] [SENSOR_TYPE...]* CHECKSUM

CommandPacket inboundPkt(DRIVE, COMMAND_PACKET_LENGTH);
Packet outboundPkt;

#ifdef STUB_MINION

#define HUMIDITY_SENSOR 0x02
#define TEMPERATURE_SENSOR 0x03

#else

// Here's where we want to put the sensor values
// once we have them defined.
#define PROXIMITY_SENSOR 0x01

#endif

#ifndef STUB_MINION

LEDBlinker blinker(LED_PIN);
SoftServo sweepServo(SWEEP_SERVO_PIN);
IrSensors irSensors(LEFT_IR_SENSOR_PIN, CENTER_IR_SENSOR_PIN, RIGHT_IR_SENSOR_PIN);
PingSensor pingSensor(PING_SENSOR_PIN);
Look looker(&sweepServo, &irSensors, &pingSensor);
Move mover(&looker);

#endif

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

#ifdef STUB_MINION

  // Set the digital pins to OUTPUT mode
  pinMode(FORWARD, OUTPUT);
  pinMode(BACKWARD, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);

#else

  blinker.begin();
  blinker.blinkNumber(8); // We do this for Leonardo (not sure why)

  // Prep the movement system
  mover.begin();

  // Set the initial speed of the robot - 10% above minimum speed
  mover.setSpeed(MIN_SPEED + 10);

  // Center the servo
  sweepServo.write(90, 2000);

  // Seed the random number generator from the ambient light
  // level on the left sensor
  randomSeed(irSensors.getAmbientLevel(IrLeft));

#endif

  inboundPkt.reset();
}

void loop()
{

#ifdef STUB_MINION

  float h = 40;
  float t = 22;

  float tf = t * 1.8 + 32; // Convert fromn C to F

  sendTempHumidValues(h, tf);

  delay(2000);

#else

  if (looker.sensesObstacle(OBST_FRONT_EDGE, 0) ||
      looker.sensesObstacle(OBST_FRONT, 18)) {
    // Regardless of why, just stop.
    mover.stop();
    sendStop();
  }

#endif
}

void serialEvent() {
  if (CommandPacket::read(Serial, inboundPkt)) {
    processCompletePacket();
  } else {
    // This was a bad or incomplete packet. Moving on.
    inboundPkt.reset();
#ifdef STUB_MINION
    badPacket();
#endif
  }
}

# ifdef STUB_MINION
void badPacket() {
  // Show that we got a bad command by
  // turing all the LED's on.
  ledsOff();

  digitalWrite(FORWARD, true);
  digitalWrite(BACKWARD, false);
  digitalWrite(LEFT, false);
  digitalWrite(RIGHT, true);
}
#endif

void processCompletePacket() {
  int speed = (int) inboundPkt.nextValue(2);
  uint16_t direction = inboundPkt.nextValue(2);
  inboundPkt.reset();

#ifdef STUB_MINION

  ledsOff();

  if (speed > 0) {
    if (direction == 0) {
      digitalWrite(FORWARD, true);
    } else {
      digitalWrite(LEFT, true);
    }
  } else if (speed < 0) {
    // Speed is negative
    if (direction == 0) {
      digitalWrite(BACKWARD, true);
    } else {
      digitalWrite(RIGHT, true);
    }
  } else {
    ledsOff(); // Same as stop
  }

#else

  /*
   * So, everything here is deteremined by the speed/direction values
   * that came in with the command. The command really just says,
   * DRIVE and here's what I want you to do.
   */
  if (speed > 0) {
    mover.setSpeed(speed);
    if (direction == 0) {
      mover.forward();
    } else {
      mover.spinLeft();
    }
  } else if (speed < 0) {
    mover.setSpeed(abs(speed));
    if (direction == 0) {
      mover.backward();
    } else {
      mover.spinRight();
    }
  } else {
    // Speed is 0; just stop
    mover.stop();
  }

#endif

}

#ifdef STUB_MINION
void ledsOff() {
  digitalWrite(FORWARD, false);
  digitalWrite(BACKWARD, false);
  digitalWrite(LEFT, false);
  digitalWrite(RIGHT, false);
}
#endif

#ifndef STUB_MINION
void sendStop() {
  outboundPkt.reset();

  outboundPkt.append(0x03); // Length
  outboundPkt.append(PROXIMITY_SENSOR);
  outboundPkt.append(0x00);
  outboundPkt.append(0x00);
  outboundPkt.complete();

  outboundPkt.write(Serial);
}
#endif

#ifdef STUB_MINION
void sendTempHumidValues(uint16_t humidity, uint16_t temperature) {
  outboundPkt.reset();

  outboundPkt.append(0x06);
  outboundPkt.append(HUMIDITY_SENSOR);
  outboundPkt.append(humidity >> 8);
  outboundPkt.append(humidity & 0x00ff);
  outboundPkt.append(TEMPERATURE_SENSOR);
  outboundPkt.append(temperature >> 8);
  outboundPkt.append(temperature & 0x00ff);
  outboundPkt.complete(); // Deal with the checksum

  outboundPkt.write(Serial);
}
#endif

