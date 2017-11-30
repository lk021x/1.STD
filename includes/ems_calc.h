/*
 * ems_calc.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef __EMS_CLAC_H__
#define __EMS_CALC_H__

void EMS_Calc(void);
void SOC_Status(void);
void SOC_Control(void);
void AGC_Control(void);
void RunMode_Control(void);
uint8_t CaculateWeekDay(uint16_t y,uint8_t m, uint8_t d);
void PidReg3Calc(uint8_t p);
void Power_Balancing(uint8_t PCS_Num ,int16_t P_Aver,int16_t P_Max,uint8_t Status);

float Pid_PowerCtrl(float Ref, float Fdb, float Uplimit , float Lowlimit,uint8_t p);
float Pid_VoltCtrl(float Ref, float Fdb, float Uplimit , float Lowlimit,uint8_t p);

extern int32_t EMS_Run_Power_Active;
#endif

