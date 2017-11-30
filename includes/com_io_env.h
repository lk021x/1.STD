/*
 * com_io_pvs.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _COM_IO_ENV_H_
#define _COM_IO_ENV_H_

void EMS_Process_Data_From_ENV_0x04(uint8_t *data,uint8_t which);		//处理ENV返回数据
void EMS_Process_Data_From_ENV_By_UART(uint8_t *data,uint32_t len); 	//处理ENV发来的数据
uint8_t EMS_Perpare_To_Send_Data_To_ENV(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID);

#endif

