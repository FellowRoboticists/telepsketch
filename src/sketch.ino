// Define the digital pins for the LEDs
#define FORWARD 13
#define BACKWARD 12
#define LEFT 11
#define RIGHT 10

// The incoming commands
#define DRIVE 0x89

#define COMMAND_PACKET_LENGTH 5

uint8_t commandPacket [COMMAND_PACKET_LENGTH];
int currentPacketLength = 0;

void setup()
{
  Serial.begin(57600);
  // Serial.begin(9600);

  // Set the digital pins to OUTPUT mode
  pinMode(FORWARD, OUTPUT);
  pinMode(BACKWARD, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);
}

void loop()
{
  if (currentPacketLength >= COMMAND_PACKET_LENGTH) {
    processCompletePacket();
    currentPacketLength = 0;
  }
  // delay(300);
}

void serialEvent() {
  Serial.println("Got serial event");
  while (Serial.available()) {
    uint8_t b = Serial.read();
    Serial.print("Byte read: ");
    Serial.println(b);
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
}

void processCompletePacket() {
  int speed = commandPacket[1] << 8 | commandPacket[2];
  int direction = commandPacket[3] << 8 | commandPacket[4];
  //uint16_t speed = commandPacket[1] << 8 | commandPacket[2];
  //uint16_t direction = commandPacket[3] << 8 | commandPacket[4];

  Serial.print("Speed: ");
  Serial.println(speed);
  Serial.print("Direction: ");
  Serial.println(direction);

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
