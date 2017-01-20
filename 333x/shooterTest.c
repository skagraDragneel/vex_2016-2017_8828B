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

task main()
{
	while(true)
	{
		if(vexRT[Btn7D])
		{
			motor[bottomLeft] = 75;
		}
		else if(vexRT[Btn7R])
		{
			motor[topLeft] = 75;
		}
		else if(vexRT[Btn8D])
		{
			motor[bottomRight] = 75;
		}
		else if(vexRT[Btn8U])
		{
			motor[topRight] = 75;
		}
		else
		{
			motor[bottomLeft] = 0;
			motor[bottomRight] = 0;
			motor[topLeft] = 0;
			motor[topRight] = 0;
		}
	}
}
