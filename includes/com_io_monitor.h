/*
 * com_io_monitor.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _COM_IO_PC_H_
#define _COM_IO_PC_H_

void Monitor_Get_Data_From_EMS_0x03(uint16_t addr,uint16_t len,uint8_t *data);                			//处理PC  0x03命令
void Monitor_Get_Data_From_EMS_0x04(uint16_t addr,uint16_t len,uint8_t *data);             		//处理PC  0x04命令
void Monitor_Set_Data_To_EMS_0x06(uint16_t addr,uint8_t *data);                            		//处理PC  0x06命令
uint8_t EMS_Receive_Data_From_Monitor_By_UART_Modbus(uint8_t *pbuf, uint32_t buflen, uint8_t *sbuf); 		//处理来自PC串口的数据
uint8_t EMS_Receive_Data_From_Monitor_By_NET_Modbus(uint8_t *pbuf, uint16_t buflen, uint8_t *sbuf);  	//处理来自PC网络接口的数据//ccg 2012.12.17
uint16_t iapv2_comdata_proc(uint8_t *data, uint16_t len, uint8_t *sbuf);                		//IAPV2升级(UART)
uint16_t iapv2_netdata_proc(uint8_t *data, uint16_t len, uint8_t *sbuf);                		//IAPV2升级(NET)

#endif

