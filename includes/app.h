/*
 * ems.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef __APP_H__
#define __APP_H__

int32_t Peak_Valley_Shift(void);
int32_t ACSide_PVLimit(void);
int32_t Ramp_rate_Control(void);
int32_t FM_Control(void);

extern int16_t step;
extern int32_t Ramp_Rate_Power;
extern int32_t Peak_Valley_Power;
extern uint16_t pcs_charge_delay,pcs_power_fault_delay;
extern int32_t ppk2;

#endif
