/*
 * com_io_pv.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _COM_IO_PV_H_
#define _COM_IO_PV_H_

void EMS_Process_Data_From_PV_0x03(uint8_t *data,uint8_t which);                 	//处理PV返回数据
void EMS_Process_Data_From_PV_0x04(uint8_t *data,uint8_t which);		//处理PV返回数据
int EMS_Process_Data_From_PV_By_UART(uint8_t *data,uint32_t len); 	//处理PV发来的数据
void EMS_Process_Data_From_PV_By_Net(uint8_t *data,uint32_t len);
uint8_t EMS_Perpare_To_Send_Data_To_PV_By_UART(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID);              	//向PV发送数据或者查询数据
uint8_t EMS_Perpare_To_Send_Data_To_PV_By_Net(uint8_t *buf, uint16_t id);
uint8_t EMS_Perpare_To_Send_Para_To_PV_By_Net(uint8_t *buf, uint16_t id);

#endif
