#include "stm32f10x.h"                  // Device header
#include "math.h"
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "MyI2C.h"
#include "HMC5883L_Reg.h"

#define MagnetcDeclination -6.73 //我所在当地磁偏角
#define HMC5883L_ADDRESS		0x3C
#define PI 3.1415

int16_t X_Data, Y_Data, Z_Data; //寄存器输出数据
int16_t offsetX,offsetY,offsetZ; //修正量
int16_t Angle; //角度


void HMC5883L_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();
	MyI2C_SendByte(HMC5883L_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

uint8_t HMC5883L_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();
	MyI2C_SendByte(HMC5883L_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(HMC5883L_ADDRESS | 0x01);
	MyI2C_ReceiveAck();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendAck(1);
	MyI2C_Stop();
	
	return Data;
}

void HMC5883L_Init(void)
{
	HMC5883L_WriteReg(HMC5883L_CONFIG_A, 0x58);
	HMC5883L_WriteReg(HMC5883L_CONFIG_B, 0x60);
	HMC5883L_WriteReg(HMC5883L_MODE, 0x00); //连续测量模式
}

void HMC5883L_GetData(void) //获取寄存器数据
{
	uint8_t DataH, DataL;
	
	DataH = HMC5883L_ReadReg(HMC5883L_XOUT_H);
	DataL = HMC5883L_ReadReg(HMC5883L_XOUT_L);
	X_Data = (DataH << 8) | DataL;
	
	DataH = HMC5883L_ReadReg(HMC5883L_YOUT_H);
	DataL = HMC5883L_ReadReg(HMC5883L_YOUT_L);
	Y_Data = (DataH << 8) | DataL;
	
	DataH = HMC5883L_ReadReg(HMC5883L_ZOUT_H);
	DataL = HMC5883L_ReadReg(HMC5883L_ZOUT_L);
	Z_Data = (DataH << 8) | DataL;	
}

void HMC5883L_GetAngle(void)
{
	int16_t xMax, xMin, yMax, yMin, zMax, zMin;
	HMC5883L_GetData();
	xMax=xMin=X_Data;
	yMax=yMin=Y_Data;
	zMax=zMin=Z_Data;
	for (int i = 0; i < 100; i++)
	{
		OLED_ShowString(1, 1, "Please turn your");
		OLED_ShowString(2, 1, "device around in");
		OLED_ShowString(3, 1,"20 seconds");
		HMC5883L_GetData();
		if (X_Data > xMax)	xMax = X_Data;
		if (X_Data < xMin)	xMin = X_Data;
		if (Y_Data > yMax)	yMax = Y_Data;
		if (Y_Data < yMin)	yMin = Y_Data;
		if (Z_Data > zMax)	zMax = Z_Data;
		if (Z_Data < zMin)	zMin = Z_Data;
		Delay_ms(100);
	} //计算传感器绕xyz轴旋转时的最大值与最小值
	offsetX = (xMax + xMin) / 2;
	offsetY = (yMax + yMin) / 2;
	offsetZ = (zMax + zMin) / 2; //计算修正量
	OLED_Clear();
}

 int16_t HMC5883L_CalculateAngle(void)
{
	float Curve = atan2((double)((Y_Data) - offsetY),(double)((X_Data) - offsetX));
	if (Curve < 0)
		Curve += 2 * PI;
	Angle = Curve * 180 / PI;
	Angle += MagnetcDeclination;
	if(Angle > 360)	Angle -= 360;
	Angle += 90;//由于摆放问题。需要加上90度
	if (Angle > 360)
	{
		int16_t temp;
		temp = Angle - 360;
		Angle = temp;   
	}
	return Angle;
}

void HMC5883L_ShowAngle(void)
{
	HMC5883L_GetData();
	OLED_ShowNum(1, 7, HMC5883L_CalculateAngle(), 3);
	if(HMC5883L_CalculateAngle() <= 180)
		Servo_SetAngle(HMC5883L_CalculateAngle());
	else
		Servo_SetAngle(180 - (360 - HMC5883L_CalculateAngle()));
}

void HMC5883L_ShowDirection(void)
{
	if ((Angle > 0 && Angle < 23) || (Angle > 337))
	{
		OLED_ShowString(1, 4, "N ");
	}
	if (Angle > 22 && Angle < 68)
	{
		OLED_ShowString(1, 4, "NE");
	}
	if (Angle > 67 && Angle < 113)
	{
		OLED_ShowString(1, 4, "E ");
	}
	if (Angle > 112 && Angle < 158)
	{
		OLED_ShowString(1, 4, "SE");
	}
	if (Angle > 157 && Angle < 203)
	{
		OLED_ShowString(1, 4, "S ");
	}
	if (Angle > 22 && Angle < 68)
	{
		OLED_ShowString(1, 4, "SW");
	}
	if (Angle > 22 && Angle < 68)
	{
		OLED_ShowString(1, 4, "W ");
	}
	if (Angle > 22 && Angle < 68)
	{
		OLED_ShowString(1, 4, "NW");
	}
}
