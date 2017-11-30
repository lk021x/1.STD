/*
 * com_io_bms.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _COM_IO_BMS_H_
#define _COM_IO_BMS_H_

void BMS_Set_Data_To_EMS_0x06(uint16_t addr,uint8_t *data);                                      //处理BMS  0x06命令  ccg 2013.5.9
uint16_t EMS_Receive_Data_From_BMS_By_CAN(uint8_t *buf,uint32_t size);                           //接收BMS发来的数据  ccg 2013.1.14
uint16_t EMS_Process_Data_From_BMS_By_CAN(uint8_t *pbuf, uint8_t buflen, uint8_t *sbuf); //处理BMS返回数据    ccg 2013.5.9
void EMS_Send_Data_To_BMS_By_CAN(int32_t CAN, uint8_t *buf, uint32_t size);                      //使用CAN2回复BMS    ccg 2013.1.14
void EMS_Process_Data_From_BMS_By_UART(uint8_t *data,uint32_t len);
uint8_t EMS_Perpare_To_Send_Data_To_BMS(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID);

#endif


