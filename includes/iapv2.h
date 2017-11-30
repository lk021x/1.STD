/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : iapv2.h
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : IAP-V2升级模块头文件，用于声明操作接口
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#ifndef IAP_V2_H__
#define IAP_V2_H__
#include <stdint.h>
//#include "embf_92.h"
#include "property.h"
#include "iapv2_interface.h"


#define IAP_BOTH_HOST_SLAVE   1  /* IAP模式设定，是否既做主又做从，只有末端设备如DSP只做从 */


bool    iapv2_updating(void);
uint8_t iapv2_init(void);        /* 返回1表示本次上电成功执GG了一次IAP蒳级，否则返回0 */
void    iapv2_10ms_proc(void);
uint8_t iapv2_read_data(uint16_t addr, uint8_t sbuf[], uint8_t len);
uint8_t iapv2_write_data(uint16_t addr, uint8_t rbuf[], uint8_t len);
uint8_t iapv2_range_chk(uint16_t beginaddr, uint8_t len, uint8_t cmd);

#if IAP_BOTH_HOST_SLAVE
void iapv2_call_slave(uint8_t sendbuf[], uint8_t len);
void iapv2_slave_return(uint8_t buf[]);
bool iapv2_slave_updating(void);
#endif


#endif

