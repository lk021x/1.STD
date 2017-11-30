/*
 * ems.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef __EMS_H__
#define __EMS_H__

void Grid_Connected_Start(void);
void Island_Start(void);
void Self_Check(void);
void System_Stop(void);
void System_Reset(void);
void Alarm_Check(void);
void Fault_Check(void);
void System_Logic(void);
void Com_Process(void);
void Running_Control(void);
void Switch_to_Grid(void);
void Switch_to_Island(void);
void EMG_Stop_Proc(void);

#endif

