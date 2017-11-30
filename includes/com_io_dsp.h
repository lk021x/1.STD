/*
 * com_io_dsp.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _COM_IO_DSP_H_
#define _COM_IO_DSP_H_

uint16_t IO_Receive_Data_From_DSP_By_CAN(uint8_t *bufout, uint8_t* bufin, uint8_t size);        //接收DSP发来的数据
void IO_Process_Property_From_DSP_0x04(uint8_t *data);                      //处理DSP返回的属性数据   ccg 2013.12.11
void IO_Process_Data_From_DSP_0x04(uint8_t *data);                          //处理DSP返回数据         ccg 2012.12.21
void IO_Process_Data_From_DSP_By_UART_Or_CAN(uint8_t *data,uint32_t len);   //处理DSP发来的数据       ccg 2013.1.14
uint8_t IO_Perpare_To_Send_Data_To_DSP(uint8_t *buf);                       //每秒向DSP发送数据或者查询数据
void IO_Send_Data_To_DSP_By_CAN(int32_t fd, uint8_t *buf, uint8_t size);

#endif
