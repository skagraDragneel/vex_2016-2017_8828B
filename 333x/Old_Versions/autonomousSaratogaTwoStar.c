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
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
		Collin Bolles
		1/4/2017
		Works anywhere on field
		Goes up to wall and dumps preload
*/

//------------------------------------Odometry Variables----------------------------------//
#define WHEEL_BASE 16.75
#define LEFT_CLICKS_PER_INCH 30
#define RIGHT_CLICKS_PER_INCH 30
float theta = PI/2;                    /* bot heading */
float X_pos=0;                    /* bot X position in inches */
float Y_pos=0;                    /* bot Y position in inches */
float traveled = 0;								//distanced traveled from set point

//Variables to control arm
int additionalPower = 0;
int lockArmPosition = 0;
bool lockArm = false;

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
Moves all of the arm motors at one PWM value
</summary>
<param id=speed> PWM value, positive and negative matter </param>
*/
void moveArm(int speed)
{
	motor[topRight] = speed;
	motor[bottomRight] = speed;
	motor[topLeft] = speed;
	motor[bottomLeft] = speed;
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
	motor[backRight] = speed;
	motor[backLeft] = speed;

	motor[frontRight] = speed;
	motor[frontLeft] = speed;
}

/*
<summary>
Has the motors move at a given PWM speed backwards only
</summary>
<param id=speed> PWM speed, positive only!</param>
*/
void driveBackward(int speed)
{
	motor[backRight] = -speed;
	motor[backLeft] = -speed;

	motor[frontRight] = -speed;
	motor[frontLeft] = -speed;
}

/*
<summary>
Moves the robot left sideways(not turning)
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void driveLeftward(int speed)
{
	motor[frontRight] = -speed;
	motor[frontLeft] = -speed;

	motor[backRight] = speed;
	motor[backLeft] = speed;
}

/*
<summary>
Moves the robot right sideways(not turning)
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void driveRightward(int speed)
{
	motor[frontRight] = speed;
	motor[frontLeft] = speed;

	motor[backRight] = -speed;
	motor[backLeft] = -speed;
}

/*
<summary>
Spins the robot right
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void turnRight(int speed)
{
	motor[frontRight] = -speed;
	motor[frontLeft] = speed;

	motor[backRight] = -speed;
	motor[backLeft] = speed;
}

/*
<summary>
Spins the robot left
</summary>
<param id = speed> PWM value, positive only!</param>
*/
void turnLeft(int speed)
{
	motor[frontRight] = speed;
	motor[frontLeft] = -speed;

	motor[backRight] = speed;
	motor[backLeft] = -speed;
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
		distance = sqrt(pow(X_pos - startPositionX, 2) + pow(Y_pos - startPositionY, 2)
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

task main()
{
	nMotorEncoder[backLeft] = 0;
	nMotorEncoder[backRight] = 0;
	nMotorEncoder[topRight] = 0;
	theta = PI/2;
	X_pos = 0;
	Y_pos = 0;
	startTask(odometry);

	//Drop Preload
	driveBackwardInches(10, 127);
	moveArmDegree(10, 60);
	lockArm = true;

	//Pick up preload
	driveForwardInches(6, 75);
	moveArmDegree(30, 60);

	//Drive up to wall
	driveBackwardInches(35, 127);

	//Dump Star
	moveArmDegree(120, 90);

	//Drop star on wall
	motor[rightPincer] = -75;
	wait1Msec(750);
	motor[rightPincer] = 0;

	moveArmDegree(-100, 75);

	wait1Msec(5000);
	stopAllTasks();

}
