#ifndef __HMC5883L_H
#define __HMC5883L_H

void HMC5883L_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t HMC5883L_ReadReg(uint8_t RegAddress);
void HMC5883L_Init(void);
void HMC5883L_GetData(void);
void HMC5883L_GetAngle(void);
int16_t HMC5883L_CalculateAngle(void);
void HMC5883L_ShowAngle(void);
void HMC5883L_ShowDirection(void);

#endif
