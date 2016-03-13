#!/usr/bin/env node

const readline = require('readline');
const RobotSerialInterface = require('robot-serial-iface').RobotSerialInterface;

const DRIVE = 0x89;
// When hooked through the level converter, this is the
// serial port tty
// const SERIAL_PORT = '/dev/ttyAMA0';
// When hooked through the USB, this is the serial port
// tty
const SERIAL_PORT = '/dev/ttyACM0';
const SERIAL_OPTIONS = { baudrate: 57600 };
const SENSORS = [
  {
    name: 'temperature',
    startByte: 0x03,
    numBytes: 2
  },
  {
    name: 'humidity',
    startByte: 0x02,
    numBytes: 2
  }
];

var robot = new RobotSerialInterface();

// Start the interface...
robot.start(SERIAL_PORT, SERIAL_OPTIONS, SENSORS);

robot.on('temperature', function(temp) {
  console.log("Temperature: %d", temp);
});

robot.on('humidity', function(hum) {
  console.log("Humidity: %d", hum);
});

const uB = (word) => word >> 8;

const lB = (word) => word & 0x000000ff;

const sendCommand = (speed, angle) => {
  robot.sendCommand(DRIVE, [uB(speed), lB(speed), uB(angle), lB(angle)]);
};

// #################################################
// Read commands from the command line from the user
//
const rl = readline.createInterface( {
  input: process.stdin,
  output: process.stdout
});

rl.setPrompt("Robot Command> ");
rl.prompt();

rl.on('line', (answer) => {
  if (answer === 'quit') {
    more = false;
    return rl.close();
  }

  switch (answer) {
    case 'forward':
      console.log("Drive forward");
      sendCommand(70, 0);
      break;
    case 'backward':
      console.log("Drive backward");
      sendCommand(-70, 0);
      break;
    case 'left':
      console.log("Rotate left");
      sendCommand(70, 1);
      break;
    case 'right':
      console.log("Rotate right");
      sendCommand(-70, 1);
      break;
    case 'stop':
      console.log("Stop");
      sendCommand(0, 0);
      break;
    default:
      console.log("Don't know what you're talking about: %j", answer);
  }
  rl.prompt();
}).on('close', () => {
  console.log("Thanks. Bye");
  process.exit();
});


