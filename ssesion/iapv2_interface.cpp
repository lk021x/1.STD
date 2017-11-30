/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : iapv2_interface.c
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : IAP升级模块的接口文件，实现设备交互
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#include "includes.h"

/********************************************************************************
 * Function       : iapv2_getlevel0_devaddr
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 根据固件版本获取本级非自身设备的地址列表，如DSP，并返回设备个数 
 * Calls          : None
 * Input          : ver: 待升级的固件版本，可以以此确定是对哪些设备升级
 *                  devaddr：保存设备地址
 * Output         : None
 * Return         : 设备个数
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint8_t iapv2_getlevel0_devaddr(char ver[], uint16_t *devaddr)
{
#if IAP_TWO_LEVEL_MODE
    return 0;
#else
    if(devaddr != NULL)
    {
        devaddr[0] = 0x01;
    }
    return 1;
#endif
}


/********************************************************************************
 * Function       : iapv2_get_broadcast_period
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 根据和本级非自身设备通信的波特率决定数据包广播周期，单位100ms
 * Calls          : None
 * Input          : None
 * Output         : None
 * Return         : 广播周期，单位100ms
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.12.04 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint16_t iapv2_get_broadcast_period(void)
{
    uint8_t period = 0;
    uint32_t baudrate = 0; 
    drv_ioctl(gt_glp.fserial1, USART_GET_BAUD, &baudrate);
    switch(baudrate)
    {
    case 38400:
    case 57600:
    case 115200:
    case 256000:
        period = 1;  /* 100ms */
        break;
    case 19200:
        period = 2;  /* 200ms */
        break;
    default:
        period = 3;  /* 300ms */
        break;
    }
    return period;  
}


/********************************************************************************
 * Function       : iapv2_do_call_slave
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 从机数据发送接口 
 * Calls          : None
 * Input          : buf：待发送数据
 *                  len: 待发送数据长度
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/

void iapv2_do_call_slave(uint8_t buf[], uint16_t len)
{
//     if(System_Info.Com_Info.IO_To_DSP_Mode==COM_DSP_MODE_CAN)
//     {
//         IO_Send_Data_To_DSP_By_CAN(gt_glp.fdcan1, buf, (((len-1)/8)+1)*8);
//     }
//     else if(System_Info.Com_Info.IO_To_DSP_Mode==COM_DSP_MODE_UART485)
//     {
//         drv_write(gt_glp.fserial3, buf, len);
//     }
}

/********************************************************************************
 * Function       : iap_pkg_decrypt
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 数据AES解密接口 
 * Calls          : None
 * Input          : buf：待解密数据
 *                  len: 待解密数据长度
 * Output         : buf：解密后数据
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iap_pkg_decrypt(uint8_t *buf, uint8_t size)
{
	int ret = -1;
#if 0
    /* 硬件AES解密 */
    uint8_t skey[] = {"sdkjsdkj12987391"};
    uint8_t uOutBuf[16] = {0};
    for(uint8_t i = 0; i < (size >> 4); ++i)
    {
        CRYP_AES_ECB(MODE_DECRYPT, skey, 128, buf + (i << 4), 16, uOutBuf);
        memcpy(buf + (i << 4), uOutBuf, 16);
    }
#else
    /* 软件AES解密，无硬件解密时使用 */
    aes_decrypt_ctx g_ctx[1];
    char const skey[] = {"sdkjsdkj12987391"};
    uint8_t uOutBuf[16] = {0};

    aes_decrypt_key((const uint8_t *)skey, 16, g_ctx);
    for (uint8_t i = 0; i<(size >> 4); ++i)
    {
        ret = aes_decrypt(buf + (i << 4), uOutBuf, g_ctx);
        if(ret != 0){
        	//printf("aes decrypt err\n");
        }else{
        	memcpy(buf + (i << 4), uOutBuf, 16);
        }
    }
#endif
}

/********************************************************************************
 * Function       : iapv2_do_attr_store
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : IAP-V2升级属性信息存储接口
 * Calls          : None
 * Input          : addr：数据存储地址
 *                  data：待存储的数据
 *                  len：待存储的数据字节数
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iapv2_do_attr_store(uint32_t addr, uint8_t *data, uint16_t len)
{
    if(data != NULL)
    {
        write_eeprom(addr, data, len);
    }
}

/********************************************************************************
 * Function       : iapv2_do_attr_extract
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 日志属性信息提取接口
 * Calls          : None
 * Input          : addr：待读取的数据首地址
 *                  data：保存待读取数据的缓冲区首地址
 *                  len：缓冲区字节数
 * Output         : data：保存读取的数据
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iapv2_do_attr_extract(uint32_t addr, uint8_t *data, uint16_t len)
{
    if(data != NULL)
    {
        read_eeprom(addr, data, len);
    }
}

/********************************************************************************
 * Function       : iapv2_do_program_store
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 记录存储的执行接口，将记录存储到FLASH或其他非易失存储体中
 * Calls          : None
 * Input          : addr：数据存储地址
 *                  data：待存储的数据
 *                  len：待存储的数据字节数
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iapv2_do_program_store(uint32_t addr, uint8_t *data, uint16_t len)
{
	write_update_file(addr, data, len);
}

/********************************************************************************
 * Function       : iapv2_do_program_extract
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 记录提取的执行接口，将记录从FLASH或其他非易失存储体中提取出来
 * Calls          : None
 * Input          : addr：待读取的数据首地址
 *                  data：保存待读取数据的缓冲区首地址
 *                  len：缓冲区字节数
 * Output         : data：保存读取的数据
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iapv2_do_program_extract(uint32_t addr, uint8_t *data, uint16_t len)
{
	read_update_file(addr, data, len);
}

/********************************************************************************
 * Function       : iapv2_erase_all_program
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 擦除所有IAP-V2升级程序的存储空间数据
 * Calls          : None
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iapv2_erase_all_program(void)
{
	clear_update_file();
}
