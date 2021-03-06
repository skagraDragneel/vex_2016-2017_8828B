#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           claw,          tmotorVex393_HBridge, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port2,           frontRight,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           backRight,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           frontLeft,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           backLeft,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           right1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           right2,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           left2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           left1,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          claw1,         tmotorVex393_HBridge, openLoop, encoderPort, I2C_2)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*+++++++++++++++++++++++++++++++++++++++++++++| Notes |++++++++++++++++++++++++++++++++++++++++++++
Mecanum Drive - Basic
- This program allows you to remotely control a robot with mecanum wheels.
- The left joystick Y-axis controls the robot's forward and backward movement.
- The left joystick X-axis controls the robot's left and right movement.
- The right joystick X-axis controls the robot's rotation.

[I/O Port]          [Name]              [Type]                [Description]
Motor Port 2        frontRight          VEX Motor             Front Right motor
Motor Port 3        backRight           VEX Motor             Back Right motor
Motor Port 4        frontLeft           VEX Motor             Front Left motor
Motor Port 5        backLeft            VEX Motor             Back Left motor
--------------------------------------------------------------------------------------------------*/
bool clawLock = false;
task main()
{
	nMotorEncoder[claw] = 0;
	nMotorEncoder[claw1] = 0;
	bool lockClawButtonPressed = false;
	//Loop Forever
	while(1 == 1)
	{
		//Remote Control Commands
		motor[frontRight] = vexRT[Ch2];
		motor[backRight] =  vexRT[Ch2];
		motor[frontLeft] = vexRT[Ch3];
		motor[backLeft] =  vexRT[Ch3];

		if(vexRT[Btn7D] && !lockClawButtonPressed)
		{
			clawLock = !clawLock;
			lockClawButtonPressed = true;
		}
		else if(!vexRT[Btn7D])
		{
			lockClawButtonPressed = false;
		}

		if(vexRT[Btn6U])
		{
			if(nMotorEncoder[claw] < 700)
			{
				motor[claw] = 127;
			}
			else
			{
				motor[claw] = 0;
			}

			if(nMotorEncoder[claw1] < 700)
			{
				motor[claw1] = 70;
			}
			else
			{
				motor[claw1] = 0;
			}
		}
		else if(vexRT[Btn6D])
		{
			if(nMotorEncoder[claw] > 10)
			{
				motor[claw] = -127;
			}
			else
			{
				motor[claw] = 0;
			}

			if(nMotorEncoder[claw1] > 10)
			{
				motor[claw1] = -70;
			}
			else
			{
				motor[claw1] = 0;
			}
		}
		else if(clawLock)
		{
			motor[claw1] = 50;
			motor[claw] = 70;
		}
		else
		{
			motor[claw1] = 0;
			motor[claw] = 0;
		}

		if(vexRT[Btn5U] == 1)       	//If button 5U is pressed...
		{
			motor[right2] = 70;    	//...raise the arm.
		}
		else if(vexRT[Btn5D] == 1)  	//Else, if button 5D is pressed...
		{
			motor[right2] = -70;   	//...lower the arm.
		}
		else                      		//Else (neither button is pressed)...
		{
			motor[right2] = 10;      	//...stop the arm.
		}

		if(vexRT[Btn5U] == 1)       	//If button 5U is pressed...
		{
			motor[right1] = 70;    	//...raise the arm.
		}
		else if(vexRT[Btn5D] == 1)  	//Else, if button 5D is pressed...
		{
			motor[right1] = -70;   	//...lower the arm.
		}
		else                      		//Else (neither button is pressed)...
		{
			motor[right1] = 10;      	//...stop the arm.
		}

		if(vexRT[Btn5U] == 1)       	//If button 5U is pressed...
		{
			motor[left1] = 70;    	//...raise the arm.
		}
		else if(vexRT[Btn5D] == 1)  	//Else, if button 5D is pressed...
		{
			motor[left1] = -70;   	//...lower the arm.
		}
		else                      		//Else (neither button is pressed)...
		{
			motor[left1] = 10;      	//...stop the arm.
		}

		if(vexRT[Btn5U] == 1)       	//If button 5U is pressed...
		{
			motor[left2] = 70;    	//...raise the arm.
		}
		else if(vexRT[Btn5D] == 1)  	//Else, if button 5D is pressed...
		{
			motor[left2] = -70;   	//...lower the arm.
		}
		else                      		//Else (neither button is pressed)...
		{
			motor[left2] = 10;      	//...stop the arm.
		}
	}
}
