/*
 * com_io_dcdc.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _COM_IO_DCDC_H_
#define _COM_IO_DCDC_H_

void EMS_Process_Data_From_DCDC_0x04(uint8_t *data,uint8_t which);		//处理DCDC返回数据
void EMS_Process_Data_From_DCDC_By_UART(uint8_t *data,uint32_t len); 	//处理DCDC发来的数据
uint8_t EMS_Perpare_To_Send_Data_To_DCDC(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID);     //向DCDC发送数据或者查询数据

#endif

