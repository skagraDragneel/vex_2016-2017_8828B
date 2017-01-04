#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    rightPot,       sensorPotentiometer)
#pragma config(Sensor, in2,    leftPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  topLimit,       sensorTouch)
#pragma config(Sensor, dgtl2,  bottomLimit,    sensorTouch)
#pragma config(Sensor, I2C_5,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           rightPincer,   tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           bottomRight,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           topLeft,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           FR,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           LE,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           RI,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           BK,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           bottomLeft,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           topRight,      tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_5)
#pragma config(Motor,  port10,          leftPincer,    tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
	12/23/2016
	Collin Bolles
	+ Drive control
	arm motion
	arm lock control
	pincher motion open and close control
*/


int lockArmPosition = nMotorEncoder[topRight];
int additionPower = 0;

int degreesToTicks(int degrees)
{
	return (int)(degrees / 0.078);//.0078 degrees per tick
}



//Gets Joystick Values with dead zone
int * getJoystick()
{
	int joystickValues[] = {0,0,0};
	int minVal = 30;

	//Geting C1LX or ch4
	int currentVal = abs(vexRT[Ch4]);
	if(currentVal >= minVal)
	{
		joystickValues[0] = vexRT[Ch4];
	}
	else
	{
		joystickValues[0]= 0;
	}

	//Getting C1LY of ch3
	currentVal = abs(vexRT[Ch3]);
	if(currentVal >= minVal)
	{
		joystickValues[1] = vexRT[Ch3];
	}
	else
	{
		joystickValues[1] = 0;
	}

	//Getting C1Rx or ch1
	currentVal = abs(vexRT[Ch1]);
	if(currentVal >= minVal)
	{
		joystickValues[2] = vexRT[Ch1];
	}
	else
	{
		joystickValues[2] = 0;
	}
	return joystickValues;
}

void moveShooter(int speed)
{
	motor[bottomLeft] = speed;
	motor[bottomRight] = speed;
	motor[topLeft] = speed;
	motor[topRight] = speed;
	lockArmPosition = nMotorEncoder[topRight];
}

void moveShooterDegree(float degrees, int speed)
{
	nMotorEncoder[topRight] = 0;
	if(degrees < 0)
	{
		while(nMotorEncoder[topRight] > degreesToTicks(degrees))
		{
			moveShooter(-speed);
		}
	}
	else
	{
		while(nMotorEncoder[topRight] < degreesToTicks(degrees))
		{
			moveShooter(speed);
		}
	}
	moveShooter(0);
}

void lockArm()
{
	if(lockArmPosition > nMotorEncoder[topRight]+3)
	{
		additionPower += 2; //Increment by 2
	}
	moveShooter(additionPower); //Zero if locked it on its own
}

void driveControl()
{
	int *joystickValues = getJoystick();
	int C1LX = joystickValues[0];
	int C1LY = joystickValues[1];
	int C1RX = joystickValues[2];

	motor[FR] = C1LX + C1RX;
	motor[LE] =  -C1LY + C1RX;
	motor[RI] =  C1LY + C1RX;
	motor[BK] = -C1LX + C1RX;
}

//Basic open and close of pincher claws
void movePincher()
{
	int motorSpeed = 50;
	if(vexRT[Btn8L])
	{
		motor[leftPincer] = motorSpeed;
	}
	else if(vexRT[Btn8R])
	{
		motor[leftPincer] = -motorSpeed;
	}
	else
	{
		motor[leftPincer] = 0;
	}

	if(vexRT[Btn7R])
	{
		motor[rightPincer] = -motorSpeed;
	}
	else if (vexRT[Btn7L])
	{
		motor[rightPincer] = motorSpeed;
	}
	else
	{
		motor[rightPincer] = 0;
	}
}

void pincherOpenClose()
{
	int zeroRightPot = 180; //Values to set the potentiometer value to zero
	int zeroLeftPot = 400;

	int positionOpenRight = 1700; //Potentiometer readings for each of the three positions and each arm
	int positionOpenLeft = 1700;
	int positionCloseRight = 3066;
	int positionCloseLeft = 3300;
	int positionFarRight = 280;
	int positionFarLeft = 790;

	int pincerSpeed = 80;

	int tolorance = 100;

	//Open Position control
	if(vexRT[Btn5D])
	{
		//right pincher
		if(SensorValue[rightPot]-zeroRightPot > positionOpenRight + tolorance)
		{
			motor[rightPincer] = pincerSpeed;
		}
		else if(SensorValue[rightPot]-zeroRightPot < positionOpenRight-tolorance)
		{
			motor[rightPincer] = -pincerSpeed;
		}
		else
		{
			motor[rightPincer] = 0;
		}

		//left pincher
		if(SensorValue[leftPot]-zeroLeftPot < positionOpenLeft-tolorance)
		{
			motor[leftPincer] = pincerSpeed;
		}
		else if(SensorValue[leftPot]-zeroLeftPot > positionOpenLeft+tolorance)
		{
			motor[leftPincer] = -pincerSpeed;
		}
		else
		{
			motor[leftPincer] = 0;
		}
	}

	//Close control
	else if(vexRT[Btn5U])
	{
		//right pincer
		if(SensorValue[rightPot]-zeroRightPot > positionCloseRight+tolorance)
		{
			motor[rightPincer] = pincerSpeed;
		}
		else if(SensorValue[rightPot]-zeroRightPot < positionCloseRight-tolorance)
		{
			motor[rightPincer] = -pincerSpeed;
		}
		else
		{
			motor[rightPincer] = 0;
		}

		//left pincher
		if(SensorValue[leftPot]-zeroLeftPot < positionCloseLeft-tolorance)
		{
			motor[leftPincer] = pincerSpeed;
		}
		else if(SensorValue[leftPot]-zeroLeftPot > positionCloseLeft+tolorance)
		{
			motor[leftPincer] = -pincerSpeed;
		}
		else
		{
			motor[leftPincer] = 0;
		}
	}
	else if(vexRT[Btn8D])//Getting into tight corners
	{
				//right pincer
		if(SensorValue[rightPot]-zeroRightPot > positionFarRight+tolorance)
		{
			motor[rightPincer] = pincerSpeed;
		}
		else if(SensorValue[rightPot]-zeroRightPot < positionFarRight-tolorance)
		{
			motor[rightPincer] = -pincerSpeed;
		}
		else
		{
			motor[rightPincer] = 0;
		}

		//left pincher
		if(SensorValue[leftPot]-zeroLeftPot < positionFarLeft-tolorance)
		{
			motor[leftPincer] = pincerSpeed;
		}
		else if(SensorValue[leftPot]-zeroLeftPot > positionFarLeft+tolorance)
		{
			motor[leftPincer] = -pincerSpeed;
		}
		else
		{
			motor[leftPincer] = 0;
		}
	}
}

//Basic up down moving, when not suppose to move, adds power to make sure it wont move
void dumpControl()
{
	if(!vexRT[Btn7U]) //While not touching bottom sensor
	{
		if(vexRT[Btn6U])
		{
			additionPower = 0;
			moveShooter(75);
		}
		else if(vexRT[Btn6D])
		{
			additionPower = 0;
			moveShooter(-75);
		}
		else
		{
			lockArm();
		}
	}
	else //If bottom sensor pressed
	{
		moveShooterDegree(1, 50);
		additionPower = 0;
	}
}

task main()
{
	nMotorEncoder[topRight] = 0;
	lockArmPosition = nMotorEncoder[topRight];
	while(true)
	{
		driveControl();
		dumpControl();
		movePincher();
		pincherOpenClose();
		wait10Msec(2); //Motors can only be updated every 20ms
	}
}
