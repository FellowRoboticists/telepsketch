#include "DHT.h"

// Define the digital pins for the LEDs
#define FORWARD 13
#define BACKWARD 12
#define LEFT 11
#define RIGHT 10

// The incoming commands
#define DRIVE 0x89

// Sensor output constants
#define START_BYTE 0x13

#define COMMAND_PACKET_LENGTH 5

// The sensor output looks like:
// START_BYTE LEN_BYTE SENSOR_TYPE SENSOR_VALUE1 [SENSOR_VALUE2] [SENSOR_TYPE...]* CHECKSUM

uint8_t commandPacket [COMMAND_PACKET_LENGTH];
int currentPacketLength = 0;

#define SENSOR_PACKET_LENGTH 6
uint8_t sensorPacket [SENSOR_PACKET_LENGTH];
#define ALT_SENSOR_PACKET_LENGTH 9
uint8_t altSensorPacket [ALT_SENSOR_PACKET_LENGTH];

#define HUMIDITY_SENSOR 0x02
#define TEMPERATURE_SENSOR 0x03

// Sensor settings
#define DHTPIN 4
#define UNIT 0 // Fahrenheit
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(57600);

  // Set the digital pins to OUTPUT mode
  pinMode(FORWARD, OUTPUT);
  pinMode(BACKWARD, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);

  dht.begin();
}

void loop()
{

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  float tf = t * 1.8 + 32; // Convert fromn C to F

  sendTempHumidValues(h, tf);

  delay(2000);
}

void serialEvent() {
  while (Serial.available()) {
    uint8_t b = Serial.read();
    if (currentPacketLength == 0) {

      if (b == DRIVE) {
        // This is a valid command
        commandPacket[currentPacketLength++] = b;
      } // else skip reading until we get a valid command

    } else if (currentPacketLength >= COMMAND_PACKET_LENGTH) {

      // We have a complete packet; parse out the speed and
      // direction
      processCompletePacket();

      // Clear the packet for the next command
      currentPacketLength = 0;

    } else {
      // The packet's not full and we're collecting goodies
      commandPacket[currentPacketLength++] = b;
    }
  }
  if (currentPacketLength >= COMMAND_PACKET_LENGTH) {
    processCompletePacket();
    currentPacketLength = 0;
  }
}

void processCompletePacket() {
  int speed = commandPacket[1] << 8 | commandPacket[2];
  int direction = commandPacket[3] << 8 | commandPacket[4];

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
  altSensorPacket[0] = START_BYTE;
  altSensorPacket[1] = 0x06;
  altSensorPacket[2] = HUMIDITY_SENSOR;
  altSensorPacket[3] = humidity >> 8;
  altSensorPacket[4] = humidity & 0x00ff;
  altSensorPacket[5] = TEMPERATURE_SENSOR;
  altSensorPacket[6] = temperature >> 8;
  altSensorPacket[7] = temperature & 0x00ff;

  // Calculate the checksum for the packet
  int total = 0;
  for (int i=0; i<ALT_SENSOR_PACKET_LENGTH - 1; i++) {
    total += altSensorPacket[i];
  }

  altSensorPacket[8] = -total; // Checksum

  Serial.write(altSensorPacket, ALT_SENSOR_PACKET_LENGTH);
}

