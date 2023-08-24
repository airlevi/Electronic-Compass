#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyI2C.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "Servo.h"

int main(void)
{
	OLED_Init();
	MyI2C_Init();
	HMC5883L_Init();
	Servo_Init();
	HMC5883L_GetAngle();
	OLED_ShowString(1, 1, "Angle:");
	
	while (1)
	{
		HMC5883L_ShowAngle();
	}
}
