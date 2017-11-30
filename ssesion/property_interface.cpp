/*
 * property_interface.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

/******************************************************************************
 * Copyright (C), 1997-2012, SUNGROW POWER SUPPLY CO., LTD.
 * File name      :property_interface.c
 * Author         :wujing
 * Date           :2012.12.3
 * Description    :属性接口文件，由用户实现
 * Others         :None
 *******************************************************************************
 * 2012-11-3 : 1.0.0 : wujing
 * Modification   : 初始代码编写。
 ********************************************************************************/
#include "includes.h"

#if PROPERTY_TWO_LEVEL_MODE
/******************************************************************************
* Function       :get_dev_addr
* Author         :wujing
* Date           :2012-10-10
* Description    :获取从设备地址和通道号，并返回设备个数
* Calls          :None
* Input          :None
* Output         :TChannel_addr：保存设备通信地址和通道号
* Return         :设备个数
*******************************************************************************
* History:        2012-10-10
*------------------------------------------------------------------------------
* 2012-10-10 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/
uint8_t get_dev_addr(TChannel_addr buf[])
{
    buf[0].dev_addr = 1;
    buf[0].comu_channel_num = 1;
    return 1;
}


/* 从机数据检索命令的发送接口 */
void property_do_call_slave_send(uint8_t buf[], uint16_t len)
{
//     if(System_Info.IO_To_DSP_Mode==COM_DSP_MODE_CAN)
//     {
//         IO_Send_Data_To_DSP_By_CAN(gt_glp.fdcan1, buf, (((len-1)/8)+1)*8);
//     }
//     else if(System_Info.IO_To_DSP_Mode==COM_DSP_MODE_UART485)
//     {
//         drv_write(gt_glp.fserial3, buf, len);
//     }
}


#endif


//获取MAC地址，由用户实现
void get_mac(uint8_t *reg)
{
    memcpy(reg, System_Info.Net.mac, 6);
}




/******************************************************************************
* Function       :load_property_int_data
* Author         :wujing
* Date           :2012-10-10
* Description    :读取属性信息
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :opt - 读取相应属性的宏
* Output         :mem - 存储读取的属性值
* Return         :无
*******************************************************************************
* History:        2012-10-10
*------------------------------------------------------------------------------
* 2012-10-10 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/
void load_property_int_data(uint32_t* mem, uint32_t opt)
{
    switch(opt)
    {
        case PROPERTY_PROTOCOL_NUM:    //整机属性协议号
            *mem = Get_Protocol_ID(PROPERTY_PROTOCOL_NUM);
            break;
        case PROPERTY_PROTOCOL_VEB:    //整机属性协议版本
            *mem = Get_Protocol_Version(PROPERTY_PROTOCOL_VEB);
            break;
        case PROPERTY_ADDR_LENGTH:     //协议地址长度
            *mem = 600;
            break;
        case DEVICE_TYPE_CODE:         //设备类型编码
            *mem = Device_Type_Code[System_Info.Config.PCS_Type];
            break;
        case APP_PROTOCOL_NUM:         //应用协议号
            *mem = Get_Protocol_ID(APP_PROTOCOL_NUM);
            break;
        case APP_PROTOCOL_VEB:         //应用协议版本号
            *mem = Get_Protocol_Version(APP_PROTOCOL_VEB);
            break;
        case OTHER_PROTOCOL_NUM:       //其他协议个数
            *mem = 4;
            break;
        case IAP_PROTOCOL_NUM:         //IAP协议号
            *mem = Get_Protocol_ID(IAP_PROTOCOL_NUM);
            break;
        case IAP_PROTOCOL_VEB:         //IAP协议版本
            *mem = Get_Protocol_Version(IAP_PROTOCOL_VEB);
            break;
        case IAC_PROTOCOL_NUM:         //IAC协议号
            *mem = Get_Protocol_ID(IAC_PROTOCOL_NUM);
            break;
        case IAC_PROTOCOL_VEB:         //IAC协议版本
            *mem = Get_Protocol_Version(IAC_PROTOCOL_VEB);
            break;
        case IAD_PROTOCOL_NUM:         //IAD协议号
            *mem = Get_Protocol_ID(IAD_PROTOCOL_NUM);
            break;
        case IAD_PROTOCOL_VEB:         //IAD协议版本
            *mem = Get_Protocol_Version(IAD_PROTOCOL_VEB);
            break;
        case IAT_PROTOCOL_NUM:         //IAT协议号
            *mem = Get_Protocol_ID(IAT_PROTOCOL_NUM);
            break;
        case IAT_PROTOCOL_VEB:         //IAT协议版本
            *mem = Get_Protocol_Version(IAT_PROTOCOL_VEB);
            break;
        case OTHER_PROTOCOL_NUM1:      //其他协议号1
            *mem = 0;
            break;
        case OTHER_PROTOCOL_VEB1:      //其他协议版本号1
            *mem = 0;
            break;
        case OTHER_PROTOCOL_NUM2:      //其他协议号2
            *mem = 0;
            break;
        case OTHER_PROTOCOL_VEB2:      //其他协议版本号2
            *mem = 0;
            break;
        case OTHER_PROTOCOL_NUM3:      //其他协议号3
            *mem = 0;
            break;
        case OTHER_PROTOCOL_VEB3:      //其他协议版本号3
            *mem = 0;
            break;
        case OTHER_PROTOCOL_NUM4:      //其他协议号4
            *mem = 0;
            break;
        case OTHER_PROTOCOL_VEB4:      //其他协议版本号4
            *mem = 0;
            break;
        case UPLOAD_FIRMWARE_VEB_NUM:  //上传固件版本数量
            *mem = 2;
            break;
        default:
            *mem = 0;                  //其他情况读取值默认为0
            break;
    }
}

/******************************************************************************
* Function       :load_property_str_data
* Author         :wujing
* Date           :2012-10-10
* Description    :读取属性信息
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :opt - 读取相应字符串类型属性的宏
* Output         :mem - 存储读取的属性值
* Return         :无
*******************************************************************************
* History:        2012-10-10
*------------------------------------------------------------------------------
* 2012-10-10 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/

void load_property_str_data(char buf[], uint32_t opt)
{
    memset(buf,0,30);

    switch(opt)
    {
        case DEVICE_SERIAL_NUM:        //设备序列号
//             memcpy(buf, IAT_Info.Set_Info.SN_Number,20);
        break;
        case DEVICE_TYPE:              //机器型号
            memcpy(buf, Device_Info[DEVICE_NAME],strlen(Device_Info[DEVICE_NAME]));
        break;
        case DEVICE_VEB:               //机器版本号
            memcpy(buf, Device_Info[DEVICE_VERSION],strlen(Device_Info[DEVICE_VERSION]));
        break;
        case MCU_PLD_FIRMWARE_VEB1:    //MCU/PLD固件版本号1
            memcpy(buf, Device_Info[FIRMWARE_VEB1],strlen(Device_Info[FIRMWARE_VEB1]));
        break;
        case MCU_PLD_FIRMWARE_VEB2:    //MCU/PLD固件版本号2
            memcpy(buf, Device_Info[FIRMWARE_VEB2],strlen(Device_Info[FIRMWARE_VEB2]));
        break;
        case MCU_PLD_FIRMWARE_VEB3:    //MCU/PLD固件版本号3

        break;
        case MCU_PLD_FIRMWARE_VEB4:    //MCU/PLD固件版本号4

        break;
        case MCU_PLD_FIRMWARE_VEB5:    //MCU/PLD固件版本号5

        break;
        case MCU_PLD_FIRMWARE_VEB6:    //MCU/PLD固件版本号6

        break;
        case MCU_PLD_FIRMWARE_VEB7:    //MCU/PLD固件版本号7

        break;
        case MCU_PLD_FIRMWARE_VEB8:    //MCU/PLD固件版本号8

        break;
        default:                       //其他情况读取值默认为0

        break;
    }
}




