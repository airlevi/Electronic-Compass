#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle(int16_t Angle)
{
	PWM_SetCompare1((float)Angle / 180.0 * 2000 + 500);
}
