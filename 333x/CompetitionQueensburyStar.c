#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    rightPot,       sensorPotentiometer)
#pragma config(Sensor, in2,    leftPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  topLimit,       sensorTouch)
#pragma config(Sensor, dgtl2,  bottomLimit,    sensorTouch)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           rightPincer,   tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           bottomRight,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           topLeft,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           frontRight,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           frontLeft,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           backRight,     tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port7,           backLeft,      tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port8,           bottomLeft,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           topRight,      tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_3)
#pragma config(Motor,  port10,          leftPincer,    tmotorVex393_HBridge, openLoop, reversed)
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*        Description: Competition template for VEX EDR                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// This code is for the VEX cortex platform
#pragma platform(VEX2)

// Select Download method as "competition"
#pragma competitionControl(Competition)

//Main competition background code...do not modify!
#include "Vex_Competition_Includes.c"


int additionalPower = 0;
int lockArmPosition = 0;
bool lockArm = false;
int lockArmPositionUser = 0;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the cortex has been powered on and    */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks
	// running between Autonomous and Driver controlled modes. You will need to
	// manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	// Set bDisplayCompetitionStatusOnLcd to false if you don't want the LCD
	// used by the competition include file, for example, you might want
	// to display your team name on the LCD in this function.
	// bDisplayCompetitionStatusOnLcd = false;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

void moveArm(int speed)
{
	motor[topRight] = speed;
	motor[bottomRight] = speed;
	motor[topLeft] = speed;
	motor[bottomLeft] = speed;
	lockArmPosition = nMotorEncoder[topRight];
	lockArmPositionUser = nMotorEncoder[topRight];
}

void lockArmUser()
{
	if(lockArmPositionUser > nMotorEncoder[topRight]+3)
	{
		additionalPower += 2; //Increment by 2
	}
	if(lockArmPosition > nMotorEncoder[topRight] + 100)
	{
		lockArmPositionUser = nMotorEncoder[topRight];
	}
	moveArm(additionalPower); //Zero if locked it on its own
}

#define WHEEL_BASE 16.75
#define LEFT_CLICKS_PER_INCH 30
#define RIGHT_CLICKS_PER_INCH 30
float theta = PI/2;                    /* bot heading */
float X_pos=0;                    /* bot X position in inches */
float Y_pos=0;                    /* bot Y position in inches */
float traveled = 0;								//distanced traveled from set point

//Variables to control arm


/*
<summary>
Takes in a degree value and returns the required ticks to move the
robot arm that given degrees
</summary>
<param id=degrees> Degree value in degrees not radians</param>
*/
int degreesToTicks(int degrees)
{
	return (int)(degrees / 0.078);//0079 degrees per tick
}

/*
<summary>
Moves the robot arm based off of a given degree and speed
</summary>
<param id=degrees>
Value in degrees, represents a value that the arm moves, not
a location the arm move to</param>
<param id = speed> PWM value, positive values only!</param>
*/
void moveArmDegree(float degrees, int speed)
{
	nMotorEncoder[topRight] = 0;
	if(degrees < 0)
	{
		while(nMotorEncoder[topRight] > degreesToTicks(degrees))
		{
			moveArm(-speed);
		}
	}
	else
	{
		while(nMotorEncoder[topRight] < degreesToTicks(degrees))
		{
			moveArm(speed);
		}
	}
	moveArm(0);
	lockArmPosition = nMotorEncoder[topRight];
	additionalPower = 0;
}

/*
<summary>
Has the motors move at a given PWM speed forward only
</summary>
<param id=speed> PWM value, only positive!!</param>
*/
void driveForward(int speed)
{
	motor[backRight] = -speed;
	motor[backLeft] = -speed;

	motor[frontRight] = -speed;
	motor[frontLeft] = -speed;
}

/*
<summary>
Has the motors move at a given PWM speed backwards only
</summary>
<param id=speed> PWM speed, positive only!</param>
*/
void driveBackward(int speed)
{
	motor[backRight] = speed;
	motor[backLeft] = speed;

	motor[frontRight] = speed;
	motor[frontLeft] = speed;
}

/*
<summary>
Moves the robot left sideways(not turning)
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void driveLeftward(int speed)
{
	motor[frontRight] = speed;
	motor[frontLeft] = speed;

	motor[backRight] = -speed;
	motor[backLeft] = -speed;
}

/*
<summary>
Moves the robot right sideways(not turning)
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void driveRightward(int speed)
{
	motor[frontRight] = -speed;
	motor[frontLeft] = -speed;

	motor[backRight] = speed;
	motor[backLeft] = speed;
}

/*
<summary>
Spins the robot right
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void turnRight(int speed)
{
	motor[frontRight] = speed;
	motor[frontLeft] = -speed;

	motor[backRight] = speed;
	motor[backLeft] = -speed;
}

/*
<summary>
Spins the robot left
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void turnLeft(int speed)
{
	motor[frontRight] = -speed;
	motor[frontLeft] = speed;

	motor[backRight] = -speed;
	motor[backLeft] = speed;
}

/*
<summary>
Stops all motors on the base
</summary>
*/
void stopBase()
{
	motor[frontRight] = 0;
	motor[frontLeft] = 0;

	motor[backRight] = 0;
	motor[backLeft] = 0;
}

/*
<summary>
Keeps track of the x,y,and theta of the robot as each commmand is given
</summary>
*/
/*
<summary>
This task keeps the x,y,and theta for the robot as it moves keeping making control of robot easier
</summary>
*/
task odometry()
{
	nMotorEncoder[backRight] = 0;
	nMotorEncoder[backLeft] = 0;
	int lsamp = 0;
	int rsamp = 0;
	int L_ticks = 0;
	int R_ticks = 0;
	int last_left = 0;
	int last_right = 0;
	float x1,y1;
	float left_inches, right_inches, inches;
	while (true)
	{
		lsamp = nMotorEncoder[backRight];
		rsamp = nMotorEncoder[backLeft];

		/* determine how many ticks since our last sampling? */
		L_ticks = lsamp - last_left;
		R_ticks = rsamp - last_right;

		/* and update last sampling for next time */
		last_left = lsamp;
		last_right = rsamp;

		/* convert longs to floats and ticks to inches */
		left_inches = (float)L_ticks/LEFT_CLICKS_PER_INCH;
		right_inches = (float)R_ticks/RIGHT_CLICKS_PER_INCH;

		/* calculate distance we have traveled since last sampling */
		inches = (left_inches + right_inches) / 2.0;

		/* accumulate total rotation around our center */
		theta += (left_inches - right_inches) / WHEEL_BASE;

		if(theta >= 2*PI) //resets at 360 degrees to 0
		{
			theta = theta - 2*PI;
		}

		else if (theta < -2*PI)
		{
			theta = theta + 2*PI;
		}

		y1 = Y_pos;
		x1= X_pos;
		/* now calculate and accumulate our position in inches */
		Y_pos += inches * cos(theta);
		X_pos += inches * sin(theta);

		traveled += sqrt(pow(X_pos-x1,2) + pow(Y_pos-y1,2));//distance formula

		if(lockArm)
		{
			if(lockArmPositionUser > nMotorEncoder[topRight]+3)
			{
				additionalPower += 2; //Increment by 2
			}
			if(lockArmPosition > nMotorEncoder[topRight] + 100)
			{
				lockArmPositionUser = nMotorEncoder[topRight];
			}
			moveArm(additionalPower); //Zero if locked it on its own
		}

		wait1Msec(50);//allow for new values to come in
	}
}

/*
<summary>
Turns the robot a specific angle to the right
</summary>
<param id = targetTheta>Angle the robot should turn to IN RADIANS</param>
<param id = speed> PWM value, positive only!</param>
*/
void turnRightDegree(int targetTheta, int speed)
{
	while(theta > targetTheta)
	{
		turnRight(speed);
	}
	stopBase();
}

/*
<summary>
Turns the robot a specific angle to the left
</summary>
<param id = targetTheta>Angle the robot should turn to IN RADIANS</param>
<param id = speed> PWM value, positive only!</param>
*/
void turnLeftDegree(int targetTheta, int speed)
{
	while(theta < targetTheta)
	{
		turnLeft(speed);
	}
	stopBase();
}

/*
<summary>
Moves the robot a specific number of inches forward
</summary>
<param id= targetDistance> Distance the robot will travle in inches</param>
<param id= speed> PWM value, positive only!</param>
*/
void driveForwardInches(int targetDistance, int speed)
{
	float startPositionX = X_pos;
	float startPositionY = Y_pos;
	float distance = 0;
	while(distance < targetDistance)
	{
		driveForward(speed);
		distance = sqrt(pow(X_pos - startPositionX, 2) + pow(Y_pos - startPositionY, 2));
		wait10Msec(2);
	}
	stopBase();
}


/*
<summary>
Moves the robot a specific number of inches backwards
</summary>
<param id= targetDistance> Distance the robot will travle in inches</param>
<param id= speed> PWM value, positive only!</param>
*/
void driveBackwardInches(int targetDistance, int speed)
{
	float startPositionX = X_pos;
	float startPositionY = Y_pos;
	float distance = 0;
	while(distance < targetDistance)
	{
		driveBackward(speed);
		distance = sqrt(pow(X_pos - startPositionX, 2) + pow(Y_pos - startPositionY, 2));
		wait10Msec(2);
	}
	stopBase();
}

/*
<summary>
Tells the robot to move to a specific point on the field
</summary>
<param id=x> Target x position</param>
<param id=y> Target y position</param>
<param id=speed> PWM value, positive only!</param>
*/
void goToPoint(float x, float y, int speed)
{
	float xDistance = x - X_pos;
	float yDistance = y - Y_pos;
	float angleToPoint = acos(yDistance/xDistance);

	float distanceToPoint = sqrt(pow(X_pos - x, 2) + pow(Y_pos - y, 2));

	if(theta - angleToPoint > 0)
	{
		turnRightDegree(theta - angleToPoint, speed);
	}
	else if(theta - angleToPoint < 0)
	{
		turnLeftDegree(angleToPoint - theta, speed);
	}
	driveForwardInches(distanceToPoint, speed);
}

void openPincer(int speed)
{
	int zeroRightPot = 180; //Values to set the potentiometer value to zero
	//int zeroLeftPot = 400;

	int positionFarRight = 280;
	//int positionFarLeft = 790;

	int tolorance = 100;

	while(SensorValue[rightPot] - zeroRightPot > positionFarRight)
	{
		motor[rightPincer] = speed;
		motor[leftPincer] = -speed;
	}

}

void closePincer(int speed)
{
	int zeroRightPot = 180; //Values to set the potentiometer value to zero
	//int zeroLeftPot = 400;

	int positionFarRight = 280;
	//int positionFarLeft = 790;

	int tolorance = 100;

	while(SensorValue[rightPot] - zeroRightPot < positionFarRight-300)
	{
		motor[rightPincer] = -speed;
		motor[leftPincer] = speed;
	}

}


task autonomous()
{
	nMotorEncoder[backLeft] = 0;
	nMotorEncoder[backRight] = 0;
	nMotorEncoder[topRight] = 0;
	additionalPower = 0;
	theta = PI/2;
	X_pos = 0;
	Y_pos = 0;
	startTask(odometry);

	//Drop Preload
	driveBackwardInches(7, 127);
	moveArmDegree(10, 60);

	driveForwardInches(2, 25);
	moveArmDegree(30, 60);

	lockArm = true;
	lockArmPosition = nMotorEncoder[topRight];
	additionalPower = 0;

	//Drive up to wall
	driveBackwardInches(35, 127);
	lockArm = false;

	//Dump Star
	moveArmDegree(110, 90);

	//Drop star on wall
	motor[leftPincer] = -75;
	motor[rightPincer] = -75;
	wait1Msec(1000);
	motor[leftPincer] = 0;
	motor[rightPincer] = 0;

	moveArmDegree(-100, 75);


	wait1Msec(5000);
	stopTask(odometry);
}

//Gets Joystick Values with dead zone
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/
void driveControl()
{
	//Create "deadzone" variables. Adjust threshold value to increase/decrease deadzone
	int X2 = 0, Y1 = 0, X1 = 0, threshold = 15;

	//Loop Forever
	//Create "deadzone" for Y1/Ch3
	if(abs(vexRT[Ch3]) > threshold)
		Y1 = vexRT[Ch3];
	else
		Y1 = 0;
	//Create "deadzone" for X1/Ch4
	if(abs(vexRT[Ch4]) > threshold)
		X1 = -vexRT[Ch4];
	else
		X1 = 0;
	//Create "deadzone" for X2/Ch1
	if(abs(vexRT[Ch1]) > threshold)
		X2 = vexRT[Ch1];
	else
		X2 = 0;

	//Remote Control Commands
	motor[frontRight] = Y1 - X2 - X1;
	motor[backRight] =  Y1 - X2 + X1;
	motor[frontLeft] = Y1 + X2 + X1;
	motor[backLeft] =  Y1 + X2 - X1;
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
			additionalPower = 0;
			moveArm(75);
		}
		else if(vexRT[Btn6D])
		{
			additionalPower = 0;
			moveArm(-75);
		}
		else
		{
			lockArmUser();
		}
	}
	else //If bottom sensor pressed
	{
		moveArmDegree(1, 50);
		additionalPower = 0;
		lockArmPositionUser = nMotorEncoder[topRight];
	}
}

task usercontrol()
{
	nMotorEncoder[topRight] = 0;
	lockArmPositionUser = nMotorEncoder[topRight];
	additionalPower = 0;
	stopBase();
	moveArm(0);
	stopTask(odometry);
	while(true)
	{
		driveControl();
		dumpControl();
		movePincher();
		pincherOpenClose();
		wait10Msec(2); //Motors can only be updated every 20ms
	}
}
