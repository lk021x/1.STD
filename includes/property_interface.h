/****************************************************************************** 
 * Copyright (C), 1997-2012, SUNGROW POWER SUPPLY CO., LTD. 
 * File name      :property_interface.h
 * Author         :wujing
 * Date           :2012.12.3
 * Description    :属性接口文件，由用户实现
 * Others         :None
 *******************************************************************************
 * 2012-11-3 : 1.0.0 : wujing
 * Modification   : 初始代码编写。
 ********************************************************************************/
#ifndef PROPERTY_INTERFACE_H
#define PROPERTY_INTERFACE_H
#include "includes.h"

#define  PROPERTY_TWO_LEVEL_MODE  0      //用来区分是否使用分级



#if PROPERTY_TWO_LEVEL_MODE
#define  MAX_DEV_NUM              1      //设备最大数量 logger作主机时下级设备数量
#define  PROPERTY_CALL_PERIOD     60000  //属性检索周期，单位：s
#endif      


/* 只读，禁止修改 ------------------------------------------------------------- */
//需返回整型数据的宏定义，按属性协议依次排列
#define PROPERTY_PROTOCOL_NUM    1     //整机属性协议号         0x00072001
#define PROPERTY_PROTOCOL_VEB    2     //整机属性协议版本号  0x1010
#define PROPERTY_ADDR_LENGTH     3     //协议地址长度      600
#define DEVICE_TYPE_CODE         5     //设备类型编码   0x26
#define APP_PROTOCOL_NUM         8     //应用协议号        0x11200001
#define APP_PROTOCOL_VEB         9     //应用协议版本号   0x1010 
#define OTHER_PROTOCOL_NUM       10    //其他协议个数     0x03 
#define IAP_PROTOCOL_NUM         11    //IAP协议号       0x11200001
#define IAP_PROTOCOL_VEB         12    //IAP协议版本      0x02000100  
#define IAC_PROTOCOL_NUM         13    //IAC协议号       0x11200001 
#define IAC_PROTOCOL_VEB         14    //IAC协议版本      0x1010 
#define IAD_PROTOCOL_NUM         15    //IAD协议号       0x11200001  
#define IAD_PROTOCOL_VEB         16    //IAD协议版本      0x1010 
#define IAT_PROTOCOL_NUM         17    //IAT协议号       0x11200001 
#define IAT_PROTOCOL_VEB         18    //IAT协议版本      0x1010   
#define OTHER_PROTOCOL_NUM1      19    //其他协议号1   0x01010101
#define OTHER_PROTOCOL_VEB1      20    //其他协议版本号1   0x1010 
#define OTHER_PROTOCOL_NUM2      21    //其他协议号2     0x02020202
#define OTHER_PROTOCOL_VEB2      22    //其他协议版本号2   0x2020  
#define OTHER_PROTOCOL_NUM3      23    //其他协议号3    0x03030303
#define OTHER_PROTOCOL_VEB3      24    //其他协议版本号30x3030  
#define OTHER_PROTOCOL_NUM4      25    //没有定义其他协议时需用全F填充        DEF_VALUE_U32
#define OTHER_PROTOCOL_VEB4      26    //没有定义其他协议时需用全F填充    DEF_VALUE_U32 
#define UPLOAD_FIRMWARE_VEB_NUM  27    //上传固件版本数量  0x02 


 //需返回字符串数据的宏定义，按属性协议依次排列
#define DEVICE_SERIAL_NUM      4      //设备序列号  120731046 
#define DEVICE_TYPE            6      //机器型号    "SG30KTL"
#define DEVICE_VEB             7      //机器版本号   "V41"     
#define MCU_PLD_FIRMWARE_VEB1  28     //MCU/PLD固件版本号1   "LCD_SG30KTL_V1_B_M" 
#define MCU_PLD_FIRMWARE_VEB2  29     //MCU/PLD固件版本号2   "DSP_SG30KTL_V1_B_M"
#define MCU_PLD_FIRMWARE_VEB3  30     //MCU/PLD固件版本号3
#define MCU_PLD_FIRMWARE_VEB4  31     //MCU/PLD固件版本号4
#define MCU_PLD_FIRMWARE_VEB5  32     //MCU/PLD固件版本号5
#define MCU_PLD_FIRMWARE_VEB6  33     //MCU/PLD固件版本号6
#define MCU_PLD_FIRMWARE_VEB7  34     //MCU/PLD固件版本号7
#define MCU_PLD_FIRMWARE_VEB8  35     //MCU/PLD固件版本号8
/* ------------------------------------------------------------------------------- */


typedef struct
{
    uint16_t comu_channel_num; //通讯通道号
    uint16_t dev_addr;         //从机设备地址
} TChannel_addr;




void load_property_int_data(uint32_t* mem, uint32_t opt);
void load_property_str_data(char buf[], uint32_t opt);
uint8_t get_dev_addr(TChannel_addr buf[]);//获取设备通信地址，由用户来实现
void get_mac(uint8_t *reg);           //获取MAC地址，由用户实现
void property_do_call_slave_send(uint8_t buf[], uint16_t len);
#endif
