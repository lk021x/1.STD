/*
 * property.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

/******************************************************************************
 * Copyright (C), 1997-2012, SUNGROW POWER SUPPLY CO., LTD.
 * File name      :property.h
 * Author         :wujing
 * Date           :2012.10.11
 * Description    :整机属性协议相关宏的定义
 * Others         :None
 *******************************************************************************
 * 2012-10-11 : 1.0.0 : wujing
 * Modification   : 初始代码编写。
 ********************************************************************************/
#ifndef  PROPERTY_H
#define  PROPERTY_H
#include <stdint.h>
#include <string.h>
#include "global.h"
#include "property_interface.h"

#define  DEF_VALUE_U32   0XFFFFFFFF  //协议无效数据定义
#define  DEF_VALUE_S32   0X7FFFFFFF
#define  DEF_VALUE_U16   0XFFFF
#define  DEF_VALUE_S16   0X7FFF
#define  DEF_VALUE_UTF_8 0X00

#define  MODBUS_CMD_04      0x04       //读输入寄存器
#define  MODBUS_CMD_10      0x10       //读输入寄存器




uint8_t property_range_chk(uint16_t beginaddr, uint8_t num, uint8_t cmd);
void get_firmversion(uint8_t level, uint16_t devaddr, uint8_t idx, char *firmver);
uint8_t get_firmnum(uint8_t level, uint16_t devaddr);
uint8_t property_return_pc_cmd_04(uint16_t addr, uint8_t *sbuf, uint8_t num);


#if PROPERTY_TWO_LEVEL_MODE
//作从
void property_return_pc_cmd_10(uint16_t start_address, uint8_t rcvbuf[], uint8_t num);
uint8_t property_return_pc_cmd_04_slave(uint8_t dev_addr, uint16_t reg_addr, uint8_t *sbuf, uint16_t num);

//作主
void property_pause(void);
void property_resume(void);
bool call_slave_property_data_return(uint8_t rcvbuf[], uint16_t len);
void call_slave_property_data(void);
void property_sec_proc(void);
bool property_slave_searching(void);
#endif								   //logger作从结束


#endif


