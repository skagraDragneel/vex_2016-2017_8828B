#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    rightPot,       sensorPotentiometer)
#pragma config(Sensor, in2,    leftPot,        sensorPotentiometer)
#pragma config(Sensor, in3,    autoControl,    sensorNone)
#pragma config(Sensor, in4,    accelRight,     sensorAccelerometer)
#pragma config(Sensor, in5,    accelLeft,      sensorAccelerometer)
#pragma config(Sensor, in6,    accelRightZ,    sensorAccelerometer)
#pragma config(Sensor, in7,    accelLeftZ,     sensorAccelerometer)
#pragma config(Sensor, dgtl1,  autoSensor,     sensorTouch)
#pragma config(Sensor, dgtl2,  bottomLimit,    sensorNone)
#pragma config(Sensor, dgtl3,  autoSensor,     sensorNone)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           frontLeft,     tmotorVex393_HBridge, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port2,           frontRight,    tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_3)
#pragma config(Motor,  port9,           backRight,     tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_4)
#pragma config(Motor,  port10,          backLeft,      tmotorVex393_HBridge, openLoop, encoderPort, I2C_1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*--------------------------------General Variables ------------------------------*/
float WHEEL_BASE = 30;
float LEFT_CLICKS_PER_INCH = 27;
float RIGHT_CLICKS_PER_INCH = 27;
float traveled = 0;								//distanced traveled from set point
float timeBetweenSampling = 60; //How many millisecs before the position is upgraded

/*--------------------------------------------------------------------------------*/

/*--------------------------------Odometry ---------------------------------------*/
//Variables
float thetaOdo = PI/2;                    /* bot heading */
float xOdo=0;                    /* bot X position in inches */
float yOdo=0;                    /* bot Y position in inches */

int lsamp = 0;
int rsamp = 0;
int L_ticks = 0;
int R_ticks = 0;
int last_left = 0;
int last_right = 0;
float x1,y1;
float left_inches, right_inches, inches;

//Odomery
void odometry()
{
	lsamp = nMotorEncoder[backRight];
	rsamp = nMotorEncoder[backLeft];

	//Change in encoder values
	L_ticks = lsamp - last_left;
	R_ticks = rsamp - last_right;

	//Update past encodervalues for next pass
	last_left = lsamp;
	last_right = rsamp;

	//Calculate the distance each wheel has travled
	left_inches = (float)L_ticks/LEFT_CLICKS_PER_INCH;
	right_inches = (float)R_ticks/RIGHT_CLICKS_PER_INCH;

	//calculate distance the total robot has travled
	inches = (left_inches + right_inches) / 2.0;

	//change the angle of the robot
	thetaOdo += (left_inches - right_inches) / WHEEL_BASE;

	//Keeps the theta value within 0 and 2*PI radians
	if(thetaOdo >= 2*PI)
	{
		thetaOdo = thetaOdo - 2 * PI;
	}

	else if (thetaOdo < 0)
	{
		thetaOdo =  thetaOdo + 2 * PI;
	}

	y1 = yOdo;
	x1 = xOdo;
	/* now calculate and accumulate our position in inches */
	yOdo += inches * sin(thetaOdo);
	xOdo += inches * cos(thetaOdo);
}
/*-----------------------------------------------------------------------------------------*/

/*--------------------------------------Accelerometer--------------------------------------*/
//Variables
float thetaAcc = PI/2;                    /* bot heading */
float xAcc=0;                    /* bot X position in inches */
float yAcc=0;                    /* bot Y position in inches */
float initialLeftAccelVal = 0;
float initialRightAccelVal = 0;
float calibrateAccelLeft = 0;
float calibrateAccelRight = 0;

float pastVelocityLeft;
float pastVelocityRight;

//Support functions
float calculateDistanceTraveled(float time, float acceleration, float pastVelocity)
{
	return 0.5*acceleration*pow(time,2) + pastVelocity*time;
}

//Accelerometer task control
void accelerometerPosition()
{
	float instaAccelRight = (SensorValue[accelRight]-initialRightAccelVal)*calibrateAccelRight;
	float instaAccelLeft = (SensorValue[accelLeft]-initialLeftAccelVal)*calibrateAccelLeft;

	float distanceRight = calculateDistanceTraveled((timeBetweenSampling/1000), instaAccelRight, pastVelocityRight);
	float distanceLeft = calculateDistanceTraveled((timeBetweenSampling/1000), instaAccelLeft, pastVelocityLeft,);

	pastVelocityRight = instaAccelRight * (timeBetweenSampling / 1000);
	pastVelocityLeft = instaAccelLeft * (timeBetweenSampling / 1000);

		//change the angle of the robot
	thetaAcc += (distanceLeft - distanceRight) / WHEEL_BASE;

	//Keeps the theta value within 0 and 2*PI radians
	if(thetaAcc >= 2*PI)
	{
		thetaAcc = thetaAcc - 2 * PI;
	}

	else if (thetaAcc < 0)
	{
		thetaAcc =  thetaAcc + 2 * PI;
	}
	/* now calculate and accumulate our position in inches */
	float totalDistance = (distanceLeft + distanceRight) / 2.0;
	yAcc += totalDistance * sin(thetaAcc);
	xAcc += totalDistance * cos(thetaAcc);
}
/*-----------------------------------------------------------------------------------------*/

/*--------------------------------------Kalman Filter--------------------------------------*/

//Variables
float x_pos = 0;
float y_pos = 0;
float theta = PI/2;

//Supporting functions

void resetValue()
{
	//Odometry Values
	xOdo = x_pos;
	yOdo = y_pos;
	thetaOdo = theta;

	//Acceleromter Values
	xAcc = x_pos;
	yAcc = y_pos;
	thetaAcc = theta;
}

task kalmanFilter()
{
	while(true)
	{
		odometry();
		accelerometerPosition();
		/*
		In here I will take the values from the two tasks to come up with one final x,y value
		*/
		wait1Msec(timeBetweenSampling); //Allow values to repopulate
	}
}
/*-----------------------------------------------------------------------------------------*/

void setDefault()
{
	//Clear all motor encoders
	nMotorEncoder[backLeft] = 0;
	nMotorEncoder[backRight] = 0;
	nMotorEncoder[frontLeft] = 0;
	nMotorEncoder[frontRight] = 0;

	//Reselt Odometry values
	xOdo = 0;
	yOdo = 0;
	thetaOdo = PI/2;

	//Reset Acceleromer values
	xAcc = 0;
	yAcc = 0;
	thetaAcc = PI/2;
	initialLeftAccelVal = SensorValue[accelLeft];
	initialRightAccelVal = SensorValue[accelRight];
	float accelerationDueToGravity = 385.9;
	calibrateAccelLeft = accelerationDueToGravity / (SensorValue[accelLeftZ] - initialLeftAccelVal); //Find out how many "units" represent a single g
	calibrateAccelRight = accelerationDueToGravity / (SensorValue[accelRightZ] - initialRightAccelVal);
	pastVelocityRight = 0;
	pastVelocityLeft = 0;
	//Reset all Kalman values
	x_pos = 0;
	y_pos = 0;
	theta = PI/2;
}

task main()
{
	setDefault();
	startTask(kalmanFilter);
	while(true)
	{
		motor[backLeft] = vexRT[Ch3];
		motor[backRight] = vexRT[Ch2];
	}

}
