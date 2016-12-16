# Set up the dependencies we need

default: lib/AFMotor lib/RobotMotor lib/SoftServo lib/pspc_support lib/TelepPacket

lib/AFMotor: ../Adafruit-Motor-Shield-library
	(cd lib && ln -s ../../Adafruit-Motor-Shield-library AFMotor)

../Adafruit-Motor-Shield-library:
	(cd .. && \
		git clone git@github.com:FellowRoboticists/Adafruit-Motor-Shield-library.git &&\
		cd Adafruit-Motor-Shield-library && \
		git fetch origin robot_kit:robot_kit && \
		git checkout robot_kit)

lib/RobotMotor: ../RobotMotor
	(cd lib && ln -s ../../RobotMotor)

../RobotMotor:
	(cd .. && \
		git clone git@github.com:FellowRoboticists/RobotMotor)

lib/SoftServo: ../SoftServo
	(cd lib && ln -s ../../SoftServo)

../SoftServo:
	(cd .. && \
		git clone git@github.com:FellowRoboticists/SoftServo)

lib/pspc_support: ../pspc_support
	(cd lib && ln -s ../../pspc_support)

../pspc_support:
	(cd .. && \
		git clone git@github.com:FellowRoboticists/pspc_support)

lib/TelepPacket:
	(cd lib && ln -s ../../TelePacket/lib/TelepPacket)

../TelepPacket:
	(cd .. && \
		git clone git@github.com:FellowRoboticists/TelepPacket)
