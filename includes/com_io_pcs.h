/*
 * com_io_pcs.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _COM_IO_PCS_H_
#define _COM_IO_PCS_H_

void EMS_Process_Data_From_PCS_0x03(uint8_t *data,uint8_t which);                 	//处理PCS返回数据
void EMS_Process_Data_From_PCS_0x04(uint8_t *data,uint8_t num,uint8_t which);		//处理PCS返回数据
int EMS_Process_Data_From_PCS_By_UART(uint8_t *data,uint32_t len); 	            //处理PCS发来的数据
uint8_t EMS_Perpare_To_Send_Data_To_PCS_By_UART(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID);          //每秒向PCS发送数据或者查询数据
uint8_t EMS_Perpare_To_Send_Data_To_PCS_By_Net(uint8_t *buf, uint16_t id);   		//PCS网口查询数据
uint8_t EMS_Perpare_To_Send_Para_To_PCS_By_Net(uint8_t *buf, uint16_t id);
void EMS_Process_Data_From_PCS_By_Net(uint8_t *data,uint32_t len);                  //处理DSP发来的数据
uint8_t EMS_Perpare_To_Send_Data_To_PCS_By_Net_Not_FM(uint8_t *buf, uint16_t id);  //PCS网口查询数据


#endif
