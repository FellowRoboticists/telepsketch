#include <Packet.h>
#include <CommandPacket.h>
// #include "DHT.h"

#define SERIAL_BAUD_RATE 9600

// Define the digital pins for the LEDs
#define FORWARD 13
#define BACKWARD 12
#define LEFT 11
#define RIGHT 10

// The incoming commands
#define DRIVE 0x89

// Sensor output constants
// #define START_BYTE 0x13

#define COMMAND_PACKET_LENGTH 5

// The sensor output looks like:
// START_BYTE LEN_BYTE SENSOR_TYPE SENSOR_VALUE1 [SENSOR_VALUE2] [SENSOR_TYPE...]* CHECKSUM

CommandPacket inboundPkt(DRIVE, 5);
Packet outboundPkt;
// uint8_t commandPacket [COMMAND_PACKET_LENGTH];
int currentPacketLength = 0;

//#define SENSOR_PACKET_LENGTH 6
//uint8_t sensorPacket [SENSOR_PACKET_LENGTH];
//#define ALT_SENSOR_PACKET_LENGTH 9
//uint8_t altSensorPacket [ALT_SENSOR_PACKET_LENGTH];

#define HUMIDITY_SENSOR 0x02
#define TEMPERATURE_SENSOR 0x03

// Sensor settings
//#define DHTPIN 4
//#define UNIT 0 // Fahrenheit
//#define DHTTYPE DHT11

// DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  // Set the digital pins to OUTPUT mode
  pinMode(FORWARD, OUTPUT);
  pinMode(BACKWARD, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);

  // dht.begin();
  inboundPkt.reset();
}

void loop()
{

  float h = 40; // dht.readHumidity();
  float t = 22; // dht.readTemperature();

  float tf = t * 1.8 + 32; // Convert fromn C to F

  sendTempHumidValues(h, tf);

  delay(2000);
}

void serialEvent() {
  if (CommandPacket::read(Serial, inboundPkt)) {
    processCompletePacket();
  } else {
    // This was a bad or incomplete packet. Moving on.
    inboundPkt.reset();
  }
  //if (Packet::read(Serial, inboundPkt)) {
    //// If it returns true, we have a complete and valid packet ready to go.
    //processCompletePacket();
  //} else {
    //// Well, just roll into the next serial event and gather
    //// more data for the packet.
    //// Skipping this for now
    //// badPacket();
  //}

  //while (Serial.available()) {
    //uint8_t b = Serial.read();
    //if (currentPacketLength == 0) {

      //if (b == DRIVE) {
        //// This is a valid command
        //commandPacket[currentPacketLength++] = b;
      //} // else skip reading until we get a valid command

    //} else if (currentPacketLength >= COMMAND_PACKET_LENGTH) {

      //// We have a complete packet; parse out the speed and
      //// direction
      //processCompletePacket();

      //// Clear the packet for the next command
      //currentPacketLength = 0;

    //} else {
      //// The packet's not full and we're collecting goodies
      //commandPacket[currentPacketLength++] = b;
    //}
  //}
  //if (currentPacketLength >= COMMAND_PACKET_LENGTH) {
    //processCompletePacket();
    //currentPacketLength = 0;
  //}
}

void badPacket() {
  // Show that we got a bad command by
  // turing all the LED's on.
  ledsOff();

  digitalWrite(FORWARD, true);
  digitalWrite(BACKWARD, false);
  digitalWrite(LEFT, false);
  digitalWrite(RIGHT, true);
}

void badCommand() {
  // Show that we got a bad command by
  // turing all the LED's on.
  ledsOff();

  digitalWrite(FORWARD, true);
  digitalWrite(BACKWARD, true);
  digitalWrite(LEFT, true);
  digitalWrite(RIGHT, true);
}

void processCompletePacket() {
  //uint8_t cmd = commandPacket[0]; // inboundPkt.nextValue(1); // What's the command?
  //if (cmd != DRIVE) {
    //// Invalid command
    //badCommand();
    //return;
  //}

  int speed = (int) inboundPkt.nextValue(2); // (commandPacket[1] << 8 | commandPacket[2]);
  uint16_t direction = inboundPkt.nextValue(2); // commandPacket[3] << 8 | commandPacket[4];

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

}

void ledsOff() {
  digitalWrite(FORWARD, false);
  digitalWrite(BACKWARD, false);
  digitalWrite(LEFT, false);
  digitalWrite(RIGHT, false);
}

void sendTempHumidValues(uint16_t humidity, uint16_t temperature) {
  outboundPkt.reset();

  // altSensorPacket[0] = START_BYTE;
  outboundPkt.append(0x06);
  // altSensorPacket[1] = 0x06;
  outboundPkt.append(HUMIDITY_SENSOR);
  // altSensorPacket[2] = HUMIDITY_SENSOR;
  outboundPkt.append(humidity >> 8);
  // altSensorPacket[3] = humidity >> 8;
  outboundPkt.append(humidity & 0x00ff);
  // altSensorPacket[4] = humidity & 0x00ff;
  outboundPkt.append(TEMPERATURE_SENSOR);
  // altSensorPacket[5] = TEMPERATURE_SENSOR;
  outboundPkt.append(temperature >> 8);
  // altSensorPacket[6] = temperature >> 8;
  outboundPkt.append(temperature & 0x00ff);
  // altSensorPacket[7] = temperature & 0x00ff;
  outboundPkt.complete(); // Deal with the checksum

  // Calculate the checksum for the packet
  //int total = 0;
  //for (int i=0; i<ALT_SENSOR_PACKET_LENGTH - 1; i++) {
    //total += altSensorPacket[i];
  //}

  //altSensorPacket[8] = -total; // Checksum

  outboundPkt.write(Serial);
  // Serial.write(altSensorPacket, ALT_SENSOR_PACKET_LENGTH);
}

