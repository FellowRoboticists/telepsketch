#!/usr/bin/env node

const readline = require('readline');
const SerialPort = require('serialport').SerialPort;
const parser = require('./packet-parser');

const DRIVE = 0x89;
const SERIAL_PORT = '/dev/ttyACM0';

var serial = new SerialPort(SERIAL_PORT, { baudrate: 57600 });
// var serial = new SerialPort(SERIAL_PORT, { baudrate: 9600 });

const uB = (word) => word >> 8;

const lB = (word) => word & 0x000000ff;

const sendCommand = (speed, angle) => {
  serial.write(new Buffer([DRIVE, uB(speed), lB(speed), uB(angle), lB(angle)]));
  // serial.flush();
};

serial.on('data', (data) => {
  // console.log("There's data!!");
  // This was if the Arduino was sending text back
  // console.log(data.toString('utf8'));
  var currPkt = parser.parse(data);
  if (currPkt) {
    var idx = 2;  // Start after the LEN byte
    while (idx < currPkt.length - 1) { // skip chksum
      switch (currPkt[idx]) {
        case 0x01:
          value = (currPkt[idx+1] << 8) | currPkt[idx+2];
          console.log("Proximity: %d", value);
          idx += 3;
          break;
        case 0x02:
          value = (currPkt[idx+1] << 8) | currPkt[idx+2];
          console.log("Humidity: %d", value);
          idx += 3;
          break;
        case 0x03:
          value = (currPkt[idx+1] << 8) | currPkt[idx+2];
          console.log("Temperature: %d", value);
          idx += 3;
          break;
        default:
          console.log("Didn't recognize the sensor");
          break;
      }
    }
    parser.reset();
  }
});

serial.on('close', (err) => {
  console.log('Serial port closed');
});

serial.on('error', (err) => {
  console.log('Error on serial port: %j', err);
});

serial.on('open', () => {
  console.log('Serial port opened successfully');
});

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
      sendCommand(10, 0);
      break;
    case 'backward':
      console.log("Drive backward");
      sendCommand(-10, 0);
      break;
    case 'left':
      console.log("Rotate left");
      sendCommand(10, 1);
      break;
    case 'right':
      console.log("Rotate right");
      sendCommand(-10, 1);
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


