/*
 * property.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

/******************************************************************************
 * Copyright (C), 1997-2012, SUNGROW POWER SUPPLY CO., LTD.
 * File name      :property.c
 * Author         :wujing
 * Date           :2012.10.11
 * Description    :整机属性协议的实现
 * Others         :None
 *******************************************************************************
 * 2012-10-11 : 1.0.0 : wujing
 * Modification   : 初始代码编写。
 ********************************************************************************/
#include "includes.h"

#if PROPERTY_TWO_LEVEL_MODE		         //logger作从
#define LOGGER_STATE_COMU_FAILURE  1            //定义应用搜索下级状态：通讯不上。
#define LOGGER_STATE_READ_PROCESS  2            //定义应用搜索下级状态：正在读。
#define LOGGER_STATE_READ_COMPLETE 3            //定义应用搜索下级状态：读完。
#endif

uint8_t mac_addr[2][6] = {{0},{0}};                   //MAC地址，由用户实现获取


#if PROPERTY_TWO_LEVEL_MODE		         //logger作主

uint32_t logger_read_state = LOGGER_STATE_READ_PROCESS;//应用搜索下级状态，默认为正在读。
uint16_t total_device_num   = 0;                        //总下级数目，初始为0。
uint16_t real_device_num    = 0;                        //实际有效设备数量
uint16_t device_num_count   = 1;                        //条数计数，默认从1开始。
uint16_t comu_channel_num   = 1;                        //通讯通道号，默认为RS485,地址范围：1～299。
uint16_t device_address     = 0;                        //从机设备地址，初始为0，不可用。

//pc机发送0x10命令
uint16_t pc_cmd  =  0;                        //命令，暂时保留
uint16_t device_num = 1;                      //指定条数，即指定读取logger下级设备的哪一台属性数据

TChannel_addr dev_addr_array[MAX_DEV_NUM];                 //从机设备通信地址，由logger用户设置地址范围，支持离散地址。
uint8_t dev_data_state[MAX_DEV_NUM] = {0};                 //从机设备通讯状态，对应于地址数组，相应值为1表示数据收到且有效，为0表示数据没收到。
uint8_t dev_property[MAX_DEV_NUM][284] = {0};              //从机设备属性信息存储，液晶属性寄存器地址范围：2500~2641(MCU/PLD固件版本号后四个不读)（不计算2702~2799保留）
uint8_t send_04_cmd[8] = {0};
uint8_t search_over_flag = false;
uint8_t property_searchcmd = 1;                            // linfei 20121207 检索命令：1，启动检索，0，停止检索
uint8_t timeup_flag = false;
uint8_t cmd_flag = 0;    //命令状态 ，为0时表明发送04命令数组1，为1时表明发送04命令数组2.
uint8_t add_idx  = 0;    //用来索引从机设备通信地址。
uint8_t delay_flag_count = 0;  //2s延时计数，当一次命令发送轮循结束后，等待2s,直接将应用搜索下级设备状态置为读完，正常情况下2s的时间可以读完。
uint16_t property_call_cnt = 0; // 属性获取周期计时


//重发机制实现：单台设备收到数据则转为下一台，收不到数据则超时1S后重发，最多搜索3次。

uint8_t dev_data_state_inter[MAX_DEV_NUM] = {0}; //从机设备通讯状态，内部用，每次5MIN设备搜索初始化为0，收到一半数据置状态为0x0f/0xf0，两包全收到方置接收标志为1
uint8_t recv_state = true;                       //接收状态，由发送方置false，接收方置true。
uint8_t time_out_flag_cnt = 0;                   //发送完1秒超时计数
uint8_t retry_count       = 0;                   //设备没有回复时的重试次数，最多3次搜索


uint16_t embf_get_crc16(volatile uint8_t *ptr, uint16_t len);
uint8_t real_online_devnum(void);

/******************************************************************************
* Function       :call_slave_property_data
* Author         :wujing
* Date           :2012-10-10
* Description    :发送命令获取液晶信息
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :
* Output         :如果轮循结束，则返回true，未结束则返回false
* Return         :
*******************************************************************************
* History:        2012-10-18
*------------------------------------------------------------------------------
* 2012-10-18 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/
void call_slave_property_data(void)
{
    if((search_over_flag != true) && (timeup_flag == true))
    {
        timeup_flag = false;
        uint16_t crc_tmp;
        if(add_idx == 0)
        {
            total_device_num = get_dev_addr(dev_addr_array);
        }

        if(total_device_num == 0)
        {
            return;
        }
        if(true == recv_state)    //只有接收状态为true时才执行发送命令
        {
            if(cmd_flag == 0)                                                //发送获取上半部分属性协议命令
            {
                if(dev_addr_array[add_idx].dev_addr != 0)
                {
                    send_04_cmd[0] = dev_addr_array[add_idx].dev_addr; //属性协议须分两次读完，第一次读2500~2581,寄存器数目：82
                    send_04_cmd[1] = 0x04;
                    send_04_cmd[2] = 0x09;
                    send_04_cmd[3] = 0xc3;
                    send_04_cmd[4] = 0x00;
                    send_04_cmd[5] = 0x52;
                    crc_tmp        = embf_get_crc16(send_04_cmd, 6); //获取CRC计算值。
                    send_04_cmd[6] = LOBYTE(crc_tmp);
                    send_04_cmd[7] = HIBYTE(crc_tmp);

                }
                property_do_call_slave_send(send_04_cmd, 8);
                cmd_flag = 1;                                               //供下次调用时使用
            }
            else
            {
                if(dev_addr_array[add_idx].dev_addr != 0)
                {
                    send_04_cmd[0] = dev_addr_array[add_idx].dev_addr; //第二次读2582~2641，寄存器数目：60
                    send_04_cmd[1] = 0x04;
                    send_04_cmd[2] = 0x0A;
                    send_04_cmd[3] = 0x15;
                    send_04_cmd[4] = 0x00;
                    send_04_cmd[5] = 0x3c;
                    crc_tmp        = embf_get_crc16(send_04_cmd, 6);
                    send_04_cmd[6] = LOBYTE(crc_tmp);
                    send_04_cmd[7] = HIBYTE(crc_tmp);
                }
                property_do_call_slave_send(send_04_cmd, 8);
                recv_state = false;                          //发送完置接收标志为false
                time_out_flag_cnt = 2;                       //发送完启动1秒超时计数
                cmd_flag = 0;
            }
        }
    }
}

bool property_slave_searching(void)
{
    return (bool)((logger_read_state == LOGGER_STATE_READ_PROCESS) && (property_searchcmd == 1));
}


void property_pause(void)
{
    search_over_flag = true;
    property_searchcmd = 0;
}

void property_resume(void)
{
    property_searchcmd = 1;
}

void property_start(void)
{
    logger_read_state = LOGGER_STATE_READ_PROCESS;
    search_over_flag = false;
    memset(dev_data_state_inter, 0, MAX_DEV_NUM); //在搜索前将设备状态(内部用)清0
}


/* 定时处理 */
void property_sec_proc(void)
{
    if(property_searchcmd == 1) // linfei 20121207 检索在启动状态才允许计时动作
    {
        if((false == recv_state) && (time_out_flag_cnt != 0)) //只有未收到计数且计时开启时才有效
        {
            time_out_flag_cnt--;
            if((0 == time_out_flag_cnt) && (false == recv_state))
            {
                retry_count++;
                recv_state = true;			             //接收标志置位，以便下一个设备继续搜索
                if(3 == retry_count)//搜索3次到了，寻到下一台设备
                {
                    retry_count = 0;    //retry_count 清0
                    dev_data_state[add_idx] = 0; // linfei 20121214 更新未回复属性信息的设备状态
                    add_idx = (add_idx + 1) % total_device_num;
                    if(add_idx == 0)                         //轮循结束,将dev_addr_array[]中的地址轮循了一遍。
                    {
                        search_over_flag = true;
                        real_device_num = real_online_devnum();
                        logger_read_state = LOGGER_STATE_READ_COMPLETE; //延时2s后，将 应用搜索下级状态 置为读完。
                    }
                }
            }
        }

        // linfei 20121207 定时启动属性检索
        if(search_over_flag == true)
        {
            property_call_cnt++;
            if(property_call_cnt == PROPERTY_CALL_PERIOD)
            {
                property_call_cnt = 0;
                property_start();
            }
        }
        timeup_flag = true;
    }
}



/* 返回已经在线的设备数量 */
uint8_t real_online_devnum(void)
{
    uint8_t devnum = 0;
    for(uint8_t i = 0; i < total_device_num; i++)
    {
        if(dev_data_state[i] != 0)
        {
            devnum++;
        }
    }
    return devnum;
}

/******************************************************************************
* Function       :call_slave_property_data_return
* Author         :wujing
* Date           :2012-10-10
* Description    :读取液晶属性信息
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :待处理的数组，例如：usart的接收缓冲区gBuf.Com1RBuf。
                  数据读取的长度，实际传入的是usart_read()的返回值。
* Output         :
* Return         :读取成功true,读取失败为false。
*******************************************************************************
* History:        2012-10-18
*------------------------------------------------------------------------------
* 2012-10-18 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/

bool call_slave_property_data_return(uint8_t rcvbuf[], uint16_t len)
{
    uint8_t i;
    uint8_t array_idx = 0;                                   //用来索引数据在数组中存储的位置，防止离散地址时数据溢出
    bool ret = false;
    if(((len - 5) == 0xA4) || ((len - 5) == 0x78))   //长度判断，防止CRC校验正确，但是数据长度不对。
    {
        if(rcvbuf[1] == 0x04)
        {
            for(i = 0; i < total_device_num; i++)             //轮循数组，找到其索引。
            {
                if(rcvbuf[0] == dev_addr_array[i].dev_addr)
                {
                    array_idx = i;
                    break;
                }
            }
            if(i != total_device_num)
            {
                if(rcvbuf[2] == 0xA4) //第一次读到的是82个寄存器，164个字节
                {
                    memcpy(&dev_property[array_idx][0], &rcvbuf[3], 164);
                    dev_data_state_inter[array_idx] |= 0x0F;	  //收到一半数据
                    ret = true;
                }
                else if(rcvbuf[2] == 0x78) //第二次读到的是60个寄存器，120个字节，共284个字节
                {
                    memcpy(&dev_property[array_idx][164], &rcvbuf[3], 120);
                    dev_data_state_inter[array_idx] |= 0xF0;	  //收到另一半数据
                    ret = true;
                }
                else
                {
                    //do nothing 返回出错。
                }
                if(UINT8_MAX == dev_data_state_inter[array_idx])
                {
                    dev_data_state[array_idx] = 1;//将相应从机 设备通讯状态置位。
                    recv_state = true; 			//将接收标志置为接收成功。
                    time_out_flag_cnt = 0;        //关闭超时计数
                    if(real_device_num < real_online_devnum())
                    {
                        real_device_num = real_online_devnum();
                    }
                    add_idx = (add_idx + 1) % total_device_num;
                    if(add_idx == 0)                         //轮循结束,将dev_addr_array[]中的地址轮循了一遍。
                    {
                        search_over_flag = true;
                        logger_read_state = LOGGER_STATE_READ_COMPLETE; //延时2s后，将 应用搜索下级状态 置为读完。
                        real_device_num = real_online_devnum();
                    }
                }
            }
        }
    }
    return ret;
}

#endif	   								 //logger作主结束
#if PROPERTY_TWO_LEVEL_MODE		         //logger作从
/******************************************************************************
* Function       :logger_return_pc_cmd_10
* Author         :wujing
* Date           :2012-10-10
* Description    :logger作为从机解析PC机传来的0x10命令并进行处理
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :start_address:0x04命令的起始地址
*                :len:寄存器数目
*                :rcvbuf: 接收缓冲区，有数据需要提取
*                :sendbuf:待写入的发送缓冲区，函数只负责填充，发送在函数外实现
* Output         :
* Return         :读取成功DSUCCESS,读取失败为OPFAULT。
*******************************************************************************
* History:        2012-10-18
*------------------------------------------------------------------------------
* 2012-10-18 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/

void property_return_pc_cmd_10(uint16_t start_address, uint8_t rcvbuf[], uint8_t num)
{
    uint8_t index = 0;
    uint16_t curaddr = start_address;
    uint8_t len = num << 1;
    if(curaddr == 2799)
    {
        pc_cmd = (rcvbuf[index] << 8) | rcvbuf[index + 1];       //储存设置的 “命令”
        index += 2;
    }
    if (index < len)
    {
        curaddr = (index >> 1) + start_address;
    }
    else
    {
        return;
    }

    if(curaddr == 2800)
    {
        uint8_t tmpnum = (rcvbuf[index] << 8) | rcvbuf[index + 1];
        if((tmpnum >= 1) && (tmpnum <= real_device_num))
        {
            device_num = tmpnum;       //储存设置的指定条数
        }
        else
        {
            device_num = 1;
        }
        index += 2;
    }
}
#endif	   //logger作从结束



uint8_t property_range_chk(uint16_t beginaddr, uint8_t num, uint8_t cmd)
{
    uint8_t ret = 0;
    if(cmd == 0x04)
    {
        if(beginaddr >= 2499 && beginaddr + num - 1 <= 2798)
        {
            ret = 1;
        }
#if PROPERTY_TWO_LEVEL_MODE
        if(beginaddr >= 2799 && beginaddr + num - 1 <= 3116)
        {
            ret = 1;
        }
#endif
    }
    else if(cmd == 0x10)
    {
        if(beginaddr >= 2799 && beginaddr + num - 1 <= 2800)
        {
            ret = 1;
        }
    }
    return ret;
}

/******************************************************************************
* Function       :logger_return_pc_cmd_04
* Author         :wujing
* Date           :2012-10-10
* Description    :读取属性信息
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :addr - modbus起始地址
                  num - 需要读取的寄存器个数
                  cmd - 需要操作的命令
                  sbuf - 属性的存放地址
* Output         :
* Return         :返回操作数据的总长度
*******************************************************************************
* History:        2012-10-10
*------------------------------------------------------------------------------
* 2012-10-10 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/

uint8_t property_return_pc_cmd_04(uint16_t addr, uint8_t *sbuf, uint8_t num)
{
    uint16_t curaddr = addr;
    uint8_t len = num << 1;
    uint16_t index = 0;
    uint16_t i = 0;
//     uint8_t size = 0;
    get_mac(mac_addr[0]);                            //由用户实现MAC地址的读取
    uint32_t read_data;                              //从property_interface中读取的整型属性
    char str_data[31] = {0};						 //从property_interface中读取的字符串型属性
    if (curaddr >= 2499 && curaddr <= 2500)  //整机属性协议号
    {
        load_property_int_data(&read_data, PROPERTY_PROTOCOL_NUM);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2501 && curaddr <= 2502)   //整机属性协议版本
    {
        load_property_int_data(&read_data, PROPERTY_PROTOCOL_VEB);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2503)                       //协议地址长度
    {
        load_property_int_data(&read_data, PROPERTY_ADDR_LENGTH);
        sbuf[index++] = HIBYTE((uint16_t)read_data);
        sbuf[index++] = LOBYTE((uint16_t)read_data);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2504 && curaddr <= 2513)     //SN序列号
    {
        load_property_str_data(str_data, DEVICE_SERIAL_NUM);
        sbuf[index++] = str_data[0];
        sbuf[index++] = str_data[1];
        sbuf[index++] = str_data[2];
        sbuf[index++] = str_data[3];
        sbuf[index++] = str_data[4];
        sbuf[index++] = str_data[5];
        sbuf[index++] = str_data[6];
        sbuf[index++] = str_data[7];
        sbuf[index++] = str_data[8];
        sbuf[index++] = str_data[9];
        sbuf[index++] = str_data[10];
        sbuf[index++] = str_data[11];
        sbuf[index++] = str_data[12];
        sbuf[index++] = str_data[13];
        sbuf[index++] = str_data[14];
        sbuf[index++] = str_data[15];
        sbuf[index++] = str_data[16];
        sbuf[index++] = str_data[17];
        sbuf[index++] = str_data[18];
        sbuf[index++] = str_data[19];
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2514)                         //设备类型编码
    {
        load_property_int_data(&read_data, DEVICE_TYPE_CODE);
        sbuf[index++] = HIBYTE((uint16_t)read_data);
        sbuf[index++] = LOBYTE((uint16_t)read_data);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2515 && curaddr <= 2522)        //机器型号
    {
        load_property_str_data(str_data, DEVICE_TYPE);
        for(i = 0; i < strlen(Device_Info[DEVICE_NAME]); i++)
        {
            sbuf[index++] = str_data[i];
        }
        for(i = strlen(Device_Info[DEVICE_NAME]); i < 16 ; i++)                //将无效数据填充完整
        {
            sbuf[index++] = 0;
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2523 && curaddr <= 2532)      //机器版本号
    {
        load_property_str_data(str_data, DEVICE_VEB);
        for(i = 0; i < strlen(Device_Info[DEVICE_VERSION]); i++)
        {
            sbuf[index++] = str_data[i];
        }
        for(i = strlen(Device_Info[DEVICE_VERSION]); i < 20 ; i++)                //将无效数据填充完整
        {
            sbuf[index++] = 0;
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2533 && curaddr <= 2534)         //应用协议号
    {
        load_property_int_data(&read_data, APP_PROTOCOL_NUM);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2535 && curaddr <= 2536)       //应用协议版本号
    {
        load_property_int_data(&read_data, APP_PROTOCOL_VEB);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2537 && curaddr <= 2540)       //保留
    {
        for(i = 0; i < 8; i++)
        {
            sbuf[index++] = UINT8_MAX;       //无效数据用全F填充
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2541 && curaddr <= 2543)       // MAC-1
    {
        sbuf[index++] = mac_addr[0][0];
        sbuf[index++] = mac_addr[0][1];
        sbuf[index++] = mac_addr[0][2];
        sbuf[index++] = mac_addr[0][3];
        sbuf[index++] = mac_addr[0][4];
        sbuf[index++] = mac_addr[0][5];
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2544 && curaddr <= 2546)       // MAC-2
    {
        sbuf[index++] = mac_addr[1][0];
        sbuf[index++] = mac_addr[1][1];
        sbuf[index++] = mac_addr[1][2];
        sbuf[index++] = mac_addr[1][3];
        sbuf[index++] = mac_addr[1][4];
        sbuf[index++] = mac_addr[1][5];
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2547)                         //其他协议个数
    {
        load_property_int_data(&read_data, OTHER_PROTOCOL_NUM);
        sbuf[index++] = HIBYTE((uint16_t)read_data);
        sbuf[index++] = LOBYTE((uint16_t)read_data);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2548 && curaddr <= 2549)         //IAP协议号
    {
        load_property_int_data(&read_data, IAP_PROTOCOL_NUM);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2550 && curaddr <= 2551)         //IAP协议版本
    {
        load_property_int_data(&read_data, IAP_PROTOCOL_VEB);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2552 && curaddr <= 2553)         //IAC协议号
    {
        load_property_int_data(&read_data, IAC_PROTOCOL_NUM);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2554 && curaddr <= 2555)         //IAC协议版本
    {
        load_property_int_data(&read_data, IAC_PROTOCOL_VEB);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2556 && curaddr <= 2557)         //IAD协议号
    {
        load_property_int_data(&read_data, IAD_PROTOCOL_NUM);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2558 && curaddr <= 2559)         //IAD协议版本
    {
        load_property_int_data(&read_data, IAD_PROTOCOL_VEB);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2560 && curaddr <= 2561)         //IAT协议号
    {
        load_property_int_data(&read_data, IAT_PROTOCOL_NUM);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2562 && curaddr <= 2563)         //IAT协议版本
    {
        load_property_int_data(&read_data, IAT_PROTOCOL_VEB);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2564 && curaddr <= 2579)         //其他协议号、协议版本号，最多支持4个其他协议
    {
        load_property_int_data(&read_data, OTHER_PROTOCOL_NUM1);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
        load_property_int_data(&read_data, OTHER_PROTOCOL_VEB1);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
        load_property_int_data(&read_data, OTHER_PROTOCOL_NUM2);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
        load_property_int_data(&read_data, OTHER_PROTOCOL_VEB2);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
        load_property_int_data(&read_data, OTHER_PROTOCOL_NUM3);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
        load_property_int_data(&read_data, OTHER_PROTOCOL_VEB3);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
        load_property_int_data(&read_data, OTHER_PROTOCOL_NUM4);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
        load_property_int_data(&read_data, OTHER_PROTOCOL_VEB4);
        sbuf[index++] = HIBYTE(LOWORD(read_data));
        sbuf[index++] = LOBYTE(LOWORD(read_data));
        sbuf[index++] = HIBYTE(HIWORD(read_data));
        sbuf[index++] = LOBYTE(HIWORD(read_data));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2580)                         //上传固件版本数量
    {
        load_property_int_data(&read_data, UPLOAD_FIRMWARE_VEB_NUM);
        sbuf[index++] = HIBYTE((uint16_t)read_data);
        sbuf[index++] = LOBYTE((uint16_t)read_data);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2581 && curaddr <= 2595)        //MCU/PLD固件版本号1
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB1);
        if(curaddr == 2581)
        {
            sbuf[index++] = str_data[0];
            sbuf[index++] = str_data[1];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2582)
        {
            sbuf[index++] = str_data[2];
            sbuf[index++] = str_data[3];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2583)
        {
            sbuf[index++] = str_data[4];
            sbuf[index++] = str_data[5];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2584)
        {
            sbuf[index++] = str_data[6];
            sbuf[index++] = str_data[7];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2585)
        {
            sbuf[index++] = str_data[8];
            sbuf[index++] = str_data[9];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2586)
        {
            sbuf[index++] = str_data[10];
            sbuf[index++] = str_data[11];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2587)
        {
            sbuf[index++] = str_data[12];
            sbuf[index++] = str_data[13];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2588)
        {
            sbuf[index++] = str_data[14];
            sbuf[index++] = str_data[15];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2589)
        {
            sbuf[index++] = str_data[16];
            sbuf[index++] = str_data[17];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2590)
        {
            sbuf[index++] = str_data[18];
            sbuf[index++] = str_data[19];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2591)
        {
            sbuf[index++] = str_data[20];
            sbuf[index++] = str_data[21];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2592)
        {
            sbuf[index++] = str_data[22];
            sbuf[index++] = str_data[23];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2593)
        {
            sbuf[index++] = str_data[24];
            sbuf[index++] = str_data[25];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2594)
        {
            sbuf[index++] = str_data[26];
            sbuf[index++] = str_data[27];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2595)
        {
            sbuf[index++] = str_data[28];
            sbuf[index++] = str_data[29];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
    }
    if (curaddr >= 2596 && curaddr <= 2610)        //MCU/PLD固件版本号2
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB2);
        if(curaddr == 2596)
        {
            sbuf[index++] = str_data[0];
            sbuf[index++] = str_data[1];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2597)
        {
            sbuf[index++] = str_data[2];
            sbuf[index++] = str_data[3];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2598)
        {
            sbuf[index++] = str_data[4];
            sbuf[index++] = str_data[5];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2599)
        {
            sbuf[index++] = str_data[6];
            sbuf[index++] = str_data[7];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2600)
        {
            sbuf[index++] = str_data[8];
            sbuf[index++] = str_data[9];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2601)
        {
            sbuf[index++] = str_data[10];
            sbuf[index++] = str_data[11];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2602)
        {
            sbuf[index++] = str_data[12];
            sbuf[index++] = str_data[13];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2603)
        {
            sbuf[index++] = str_data[14];
            sbuf[index++] = str_data[15];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2604)
        {
            sbuf[index++] = str_data[16];
            sbuf[index++] = str_data[17];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2605)
        {
            sbuf[index++] = str_data[18];
            sbuf[index++] = str_data[19];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2606)
        {
            sbuf[index++] = str_data[20];
            sbuf[index++] = str_data[21];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2607)
        {
            sbuf[index++] = str_data[22];
            sbuf[index++] = str_data[23];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2608)
        {
            sbuf[index++] = str_data[24];
            sbuf[index++] = str_data[25];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2609)
        {
            sbuf[index++] = str_data[26];
            sbuf[index++] = str_data[27];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
        if(curaddr == 2610)
        {
            sbuf[index++] = str_data[28];
            sbuf[index++] = str_data[29];
        }
        if (index < len)
            curaddr = (index >> 1) + addr;
        else
            return index;
    }
    if (curaddr >= 2611 && curaddr <= 2625)        //MCU/PLD固件版本号3
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB3);
        memcpy(&sbuf[index], str_data, 30);
        index += 30;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2626 && curaddr <= 2640)        //MCU/PLD固件版本号4
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB4);
        memcpy(&sbuf[index], str_data, 30);
        index += 30;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2641 && curaddr <= 2655)        //MCU/PLD固件版本号5
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB5);
        memcpy(&sbuf[index], str_data, 30);
        index += 30;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2656 && curaddr <= 2670)        //MCU/PLD固件版本号6
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB6);
        memcpy(&sbuf[index], str_data, 30);
        index += 30;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2671 && curaddr <= 2685)        //MCU/PLD固件版本号7
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB7);
        memcpy(&sbuf[index], str_data, 30);
        index += 30;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2686 && curaddr <= 2700)        //MCU/PLD固件版本号8
    {
        load_property_str_data(str_data, MCU_PLD_FIRMWARE_VEB8);
        memcpy(&sbuf[index], str_data, 30);
        index += 30;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2701 && curaddr <= 2798)        //保留
    {
        for(i = 0; i < 196; i++)
        {
            sbuf[index++] = UINT8_MAX;
            if(index >= len)
            {
                break;
            }
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;

#if PROPERTY_TWO_LEVEL_MODE
    uint16_t icounter = 0;             //用来根据指定条数计算属性数组的索引值。
    uint16_t array_index = 0;          //用于索引下位机属性数组的元素，由指定条数来计算。
    for(i = 0; i < total_device_num; i++)   //根据0x10命令“指定条数”、设备通信状态数组来计算属性数组的索引值。
    {
        if(dev_data_state[i] != 0)
        {
            icounter++;
            if(icounter == device_num)
            {
                array_index = i;
                break;
            }
        }
    }
    if (curaddr >= 2799 && curaddr <= 2800)  //应用搜索下级状态，有三种：通讯不上、正在读、读完。
    {
        sbuf[index++] = HIBYTE(LOWORD(logger_read_state));
        sbuf[index++] = LOBYTE(LOWORD(logger_read_state));
        sbuf[index++] = HIBYTE(HIWORD(logger_read_state));
        sbuf[index++] = LOBYTE(HIWORD(logger_read_state));
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2801)                    //总下级数量
    {
        sbuf[index++] = HIBYTE(total_device_num);
        sbuf[index++] = LOBYTE(total_device_num);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2802)                    //保留,填充为UINT8_MAX
    {
        sbuf[index++] = HIBYTE(real_device_num);
        sbuf[index++] = LOBYTE(real_device_num);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2803)                    //保留,填充为UINT8_MAX
    {
        sbuf[index++] = UINT8_MAX;
        sbuf[index++] = UINT8_MAX;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2804)                    //保留,固定值1
    {
        sbuf[index++] = 0x00;
        sbuf[index++] = 0x01;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2805)                    //保留,固定值1
    {
        sbuf[index++] = 0x00;
        sbuf[index++] = 0x01;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2806)                    //保留,填充为UINT8_MAX
    {
        sbuf[index++] = UINT8_MAX;
        sbuf[index++] = UINT8_MAX;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2807)                    //保留,固定值1
    {
        sbuf[index++] = 0x00;
        sbuf[index++] = 0x01;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2808)                    //保留,填充为UINT8_MAX
    {
        sbuf[index++] = UINT8_MAX;
        sbuf[index++] = UINT8_MAX;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2809)                    //保留,填充为UINT8_MAX
    {
        sbuf[index++] = UINT8_MAX;
        sbuf[index++] = UINT8_MAX;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2810)
    {
        device_num_count = device_num;          //条数计数，由PC机所发0x10命令所设指定条数赋值。
        sbuf[index++] = HIBYTE(device_num_count);
        sbuf[index++] = LOBYTE(device_num_count);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2811)                    //数据有效状态，填充为保留值
    {
        sbuf[index++] = UINT8_MAX;
        sbuf[index++] = UINT8_MAX;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2812)                    //通讯通道号,最初也是由用户来实现获取
    {
        comu_channel_num = dev_addr_array[array_index].comu_channel_num;
        sbuf[index++] = HIBYTE(comu_channel_num);
        sbuf[index++] = LOBYTE(comu_channel_num);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2813)                    //从机设备地址，由0x10设置的指定条数计算出的索引值来计算。
    {
        device_address = dev_addr_array[array_index].dev_addr;
        sbuf[index++] = HIBYTE(device_address);
        sbuf[index++] = LOBYTE(device_address);
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
#if 0
    if (curaddr == 2814)                    //保留,填充为UINT8_MAX
    {
        sbuf[index++] = UINT8_MAX;
        sbuf[index++] = UINT8_MAX;
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;

    //下面的数据是从属性数组中读取，直接赋值即可
    if (curaddr >= 2815 && curaddr <= 2816)                  //整机属性协议号
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][0+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2817 && curaddr <= 2818)                 //整机属性协议版本
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][4+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2819)                                     //协议地址长度
    {
        sbuf[index++] = dev_property[array_index][8];
        sbuf[index++] = dev_property[array_index][9];
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2820 && curaddr <= 2829)                   //SN序列号
    {
        for(i = 0; i < 20; i++)
        {
            sbuf[index++] = dev_property[array_index][10+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2830)                                      //设备类型编码
    {
        sbuf[index++] = dev_property[array_index][30];
        sbuf[index++] = dev_property[array_index][31];
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2831 && curaddr <= 2838)                  //机器型号
    {
        for(i = 0; i < 16; i++)
        {
            sbuf[index++] = dev_property[array_index][32+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2839 && curaddr <= 2848)                  //机器版本号
    {
        for(i = 0; i < 20; i++)
        {
            sbuf[index++] = dev_property[array_index][48+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2849 && curaddr <= 2850)                 //应用协议号
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][68+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2851 && curaddr <= 2852)                 //应用协议版本号
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][72+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2853 && curaddr <= 2856)                 //保留
    {
        for(i = 0; i < 8; i++)
        {
            sbuf[index++] = dev_property[array_index][76+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2857 && curaddr <= 2859)                 // MAC-1
    {
        for(i = 0; i < 6; i++)
        {
            sbuf[index++] = dev_property[array_index][84+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2860 && curaddr <= 2862)                 // MAC-2
    {
        for(i = 0; i < 6; i++)
        {
            sbuf[index++] = dev_property[array_index][90+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2863)                                    //其他协议个数
    {
        sbuf[index++] = dev_property[array_index][96];
        sbuf[index++] = dev_property[array_index][97];
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2864 && curaddr <= 2865)                  //IAP协议号
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][98+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2866 && curaddr <= 2867)                 //IAP协议版本
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][102+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2868 && curaddr <= 2869)         //IAC协议号
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][106+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2870 && curaddr <= 2871)         //IAC协议版本
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][110+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2872 && curaddr <= 2873)         //IAD协议号
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][114+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2874 && curaddr <= 2875)         //IAD协议版本
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][118+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2876 && curaddr <= 2877)         //IAT协议号
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][122+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2878 && curaddr <= 2879)         //IAT协议版本
    {
        for(i = 0; i < 4; i++)
        {
            sbuf[index++] = dev_property[array_index][126+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2880 && curaddr <= 2895)         //其他协议号、协议版本号，最多支持4个其他协议
    {
        for(i = 0; i < 32; i++)
        {
            sbuf[index++] = dev_property[array_index][130+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr == 2896)                         //上传固件版本数量
    {
        sbuf[index++] = dev_property[array_index][162+i];
        sbuf[index++] = dev_property[array_index][163+i];
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2897 && curaddr <= 2911)        //MCU/PLD固件版本号1
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = dev_property[array_index][164+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2912 && curaddr <= 2926)        //MCU/PLD固件版本号2
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = dev_property[array_index][194+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2927 && curaddr <= 2941)        //MCU/PLD固件版本号3
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = dev_property[array_index][224+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2942 && curaddr <= 2956)        //MCU/PLD固件版本号4
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = dev_property[array_index][254+i];
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;                              //下位机上传时最大固件版本数量为4，为保证协议完整性，固件版本5～８按照保留时处理。
    if (curaddr >= 2957 && curaddr <= 2971)        //MCU/PLD固件版本号5
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = UINT8_MAX;
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2972 && curaddr <= 2986)        //MCU/PLD固件版本号6
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = UINT8_MAX;
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 2987 && curaddr <= 3001)        //MCU/PLD固件版本号7
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = UINT8_MAX;
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 3002 && curaddr <= 3016)        //MCU/PLD固件版本号8
    {
        for(i = 0; i < 30; i++)
        {
            sbuf[index++] = UINT8_MAX;
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
    if (curaddr >= 3017 && curaddr <= 3116)        //保留
    {
        for(i = 0; i < 200; i++)
        {
            sbuf[index++] = UINT8_MAX;
        }
    }
    if (index < len)
        curaddr = (index >> 1) + addr;
    else
        return index;
#endif
#endif
    return index;
}





/******************************************************************************
* Function       :get_firmnum
* Author         :wujing
* Date           :2012-10-10
* Description    :获取固件版本数目，
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :level:0 本机，level:1 从机
                  devaddr:从机设备地址
* Output         :
* Return         :成功返回固件版本数目，失败返回0
*******************************************************************************
* History:        2012-10-18
*------------------------------------------------------------------------------
* 2012-10-18 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/

uint8_t get_firmnum(uint8_t level, uint16_t devaddr)
{
    uint32_t firm_num = 0;       //存放固件版本数目
    if(0 == level)          //获取自身的固件版本，返回为1
    {
        load_property_int_data(&firm_num, UPLOAD_FIRMWARE_VEB_NUM);
    }
#if PROPERTY_TWO_LEVEL_MODE
    else if(1 == level)     //获取从机的固件版本数目，只能为logger，故从缓冲区中读取并计算。
    {
        uint8_t i;
        uint8_t array_idx = 0;      //数组索引
        for(i = 0; i < total_device_num; i++)            //轮循数组，找到其索引。
        {
            if(devaddr == dev_addr_array[i].dev_addr)
            {
                array_idx = i;
                break;
            }
        }
        if(i != total_device_num)
        {
            for(i = 0; i < 4; i++) //固件版本号最大数目为4
            {
                if(dev_property[array_idx][164+i*30] != 0) //存放有固件版本号
                {
                    firm_num++;
                }
            }
        }
    }
    else
    {
        //do nothing
    }
#endif
    return (uint8_t)firm_num;
}
/******************************************************************************
* Function       :get_firmversion
* Author         :wujing
* Date           :2012-10-10
* Description    :获取指定设备固件版本号
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :level:0 本机，level:1 从机
*                 devaddr:从机设备地址
*                 idx:要读的固件版本索引值，从0开始
*                 firmver:固件版本信息的存储地址。
* Output         :
* Return         :
*******************************************************************************
* History:        2012-10-18
*------------------------------------------------------------------------------
* 2012-10-18 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/
void get_firmversion(uint8_t level, uint16_t devaddr, uint8_t idx, char *firmver)
{
    if(0 == level)          //获取自身的固件版本号,根据上传的固件版本数量来实现
    {
        uint32_t firmnum = 0;
        load_property_int_data(&firmnum, UPLOAD_FIRMWARE_VEB_NUM);
        if(idx <= firmnum)
        {
            if(0 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB1);
            }
            else if(1 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB2);
            }
            else if(2 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB3);
            }
            else if(3 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB4);
            }
            else if(4 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB5);
            }
            else if(5 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB6);
            }
            else if(6 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB7);
            }
            else if(7 == idx)
            {
                load_property_str_data(firmver, MCU_PLD_FIRMWARE_VEB8);
            }
            else
            {
                memset(firmver, 0, 30);
            }
        }

    }
#if PROPERTY_TWO_LEVEL_MODE
    else if(1 == level)     //获取从机的固件版本数目，只能为logger，故从缓冲区中读取并计算。
    {
        if(idx < 4)         //idx 取值范围为[0,3]
        {
            uint8_t i;
            uint8_t array_idx = 0;      //数组索引
            for(i = 0; i < total_device_num; i++)            //轮循数组，找到其索引。
            {
                if(devaddr == dev_addr_array[i].dev_addr)
                {
                    array_idx = i;
                    break;
                }
            }
            if(i != total_device_num)
            {
                memcpy(firmver, &dev_property[array_idx][164+idx*30], 30); //将命令要求的指定地址、指定偏移量的固件版本上传。
            }
            else
            {
                memset(firmver, 0, 30);
            }
        }
    }
    else
    {
        //do nothing
    }
#endif

}

/******************************************************************************
* Function       :property_return_pc_cmd_04_slave
* Author         :wujing
* Date           :2012-10-10
* Description    :logger回复PC机索取从机属性信息，直接传入从机设备地址，从logger中
*				 :属性缓冲区中获取
* Calls          :无
* Table Accessed :无
* Table Updated  :无
* Input          :dev_addr:从机设备地址
*                :reg_addr:寄存器起始地址
*                :num:读取寄存器的个数
* Output         :sbuf:数据存储缓冲
* Return         :返回回复数据长度，出错则返回0
*******************************************************************************
* History:        2012-10-18
*------------------------------------------------------------------------------
* 2012-10-18 : 1.0.0 : wujing
* Modification   : 创建
*------------------------------------------------------------------------------
******************************************************************************/
#if PROPERTY_TWO_LEVEL_MODE
uint8_t property_return_pc_cmd_04_slave(uint8_t dev_addr, uint16_t reg_addr, uint8_t *sbuf, uint16_t num)
{
    uint8_t i = 0;
    uint8_t array_idx = 0;
    uint8_t index = 0;
    for(i = 0; i < total_device_num; i++)             //轮循数组，找到其索引。
    {
        if((dev_addr == dev_addr_array[i].dev_addr) && (dev_data_state[i] == 1))
        {
            array_idx = i;
            break;
        }
    }
    if(i != total_device_num)
    {
        //找到索引，开始从数组中进行读取
        uint16_t tmp = reg_addr - 2499;
        if(reg_addr < 2641)
        {
            if(reg_addr + num > 2641)
            {
                memcpy(sbuf, &dev_property[array_idx][tmp*2], (2641 - reg_addr) << 1);
                memset(&sbuf[(2641 - reg_addr) << 1], 0, (reg_addr + num - 2641) << 1);
            }
            else
            {
                memcpy(sbuf, &dev_property[array_idx][tmp*2], num * 2);
            }
        }
        else
        {
            memset(sbuf, 0, num * 2);
        }
        index = num << 1;
    }
    return index;
}
#endif



