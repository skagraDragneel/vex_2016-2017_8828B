#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl1,  topLimit,       sensorTouch)
#pragma config(Sensor, dgtl2,  bottomLimit,    sensorTouch)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           bottomRight,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           topLeft,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           FR,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           LE,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           RI,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           BK,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           bottomLeft,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           topRight,      tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
	12/3/2016
	Collin Bolles
	+ Drive control
	arm motion
	arm lock control
*/


int lockArmPosition = nMotorEncoder[topRight];
int additionPower = 0;

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

//Basic up down moving, when not suppose to move, adds power to make sure it wont move
void dumpControl()
{
	if(!SensorValue[bottomLimit]) //While not touching bottom sensor
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
		else if(vexRT[Btn5U])
		{
			additionPower = 0;
			moveShooter(30);
		}
		else if(vexRT[Btn5D])
		{
			additionPower = 0;
			moveShooter(-30);
		}
		else
		{
			lockArm();
		}
	}
	else //If bottom sensor pressed
	{
		moveShooter(20);
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
		wait10Msec(2); //Motors can only be updated every 20ms
	}
}
