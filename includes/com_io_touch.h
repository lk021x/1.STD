/*
 * com_to_touch.h
 *
 *  Created on: 2017-1-16
 *      Author: root
 */

#ifndef COM_TO_TOUCH_H_
#define COM_TO_TOUCH_H_

#ifndef _COM_IO_TOUCH_H_
#define _COM_IO_TOUCH_H_

uint8_t Run_Status(uint32_t run_status);                                            //DSP与触摸屏的状态值转换
uint8_t Run_Status_Monitor(uint32_t run_status);
uint8_t Run_Status_BMS(uint16_t run_status);
uint8_t Run_Status_For_PV(uint8_t id, uint32_t run_status);
void Touch_Picture_Disp(void);
void Touch_Load_History_Event_Record(void);                                         //加载历史事件记录
void Touch_Load_History_Fault_Record(void);                                         //装载历史故障记录
void Touch_Get_Data_From_EMS_0x01(uint16_t addr,uint16_t len,uint8_t *data);         //触摸屏处理0x01命令
void Touch_Get_Data_From_EMS_0x03(uint16_t addr,uint16_t len,uint8_t *data);         //触摸屏处理0x03命令
void Touch_Get_Data_From_EMS_0x04(uint16_t addr,uint16_t len,uint8_t *data);         //触摸屏处理0x04命令
void Touch_Set_Data_To_EMS_0x10(uint16_t start_address,uint8_t  len,uint8_t *data);  //触摸屏处理0x10命令
int EMS_Receive_Data_From_Touch_By_UART(uint8_t *pbuf, uint32_t buflen, uint8_t *sbuf); //接收触摸屏发来的信息
void Write_HisResetRecord_Init(void);												//写复位记录初始化
void Write_HisResetRecord_Process(void);											//写复位记录
void Write_HisEventRecord_Init(void);                                               //写事件记录初始化
void Write_HisEventRecord_Process(void);                                            //写事件记录
void Read_OnConditionNB_Record(uint16_t ptr,uint16_t total_nub,uint16_t serial_nb,uint32_t base_address,uint8_t *array); //读取历史记录
void Write_HisFaultRecord_Init(void);                                               //写故障记录初始化
void Write_HisFaultRecord_Process(void);                                            //写故障记录
void Write_HisAlarmRecord_Init(void);												//写告警记录初始化
void Write_HisAlarmRecord_Process(void);											//写告警记录
uint8_t Return_Max_Count(uint8_t *data,uint16_t num);											//返回最大的通讯计数器值

#endif



#endif /* COM_TO_TOUCH_H_ */
