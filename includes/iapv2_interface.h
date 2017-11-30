/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : iapv2_interface.h
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : IAP升级模块的接口头文件，包括模式设置，设备类型定义，升级数
 *                  据在flash中的存储地址定义，由用户设置
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#ifndef __IAPV2_INTERFACE_H
#define __IAPV2_INTERFACE_H
//ccg 2013.2.28---------START----------------------------------------------------
#include "includes.h"
#include "MThread.h"

/* 用户配置项 */
#define IAP_TWO_LEVEL_MODE    0               /* 模式选择，整机内接口设备如液晶或IO板为0，Logger或Wifi为1 */
#define PKG_CNT_BUFLEN        12000           /* 位表大小，按MCU片内flash大小设定，60M<-->12000 1M<-->200, 512KB<-->100, 256KB<-->50, 128KB<-->25 */
#define IAP_ATTR_BASE         (EEPROM_ADDRESS_BASE 	+ 40000)          /* 升级模块属性存储首地址，属性大小为(62 + PKG_CNT_BUFLEN*4)字节 */
#define IAP_FLASH_BASE        0x0       		/* 升级文件存储首地址 0*/
#define IAP_FLASE_LEN         0x3C00000         /* 升级文件存储结尾，60M */

uint16_t iapv2_get_broadcast_period(void);
uint8_t iapv2_getlevel0_devaddr(char ver[], uint16_t *devaddr);
void iapv2_do_attr_store(uint32_t addr, uint8_t *data, uint16_t len);
void iapv2_do_attr_extract(uint32_t addr, uint8_t *data, uint16_t len);
void iapv2_do_program_store(uint32_t addr, uint8_t *data, uint16_t len);
void iapv2_do_program_extract(uint32_t addr, uint8_t *data, uint16_t len);
void iapv2_erase_all_program(void);
void iapv2_do_call_slave(uint8_t buf[], uint16_t len);
void iap_pkg_decrypt(uint8_t *buf, uint8_t size);

#endif


