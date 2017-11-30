/*
 * com_io_dsp.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#include "includes.h"

uint16_t IO_Receive_Data_From_DSP_By_CAN(uint8_t *bufout, uint8_t* bufin, uint8_t size)
{
    static uint8_t index = 0;
    uint16_t dstLen = 0;  //修订要接收的帧长度
    struct can_frame *fr = (struct can_frame *)bufin;

	//if(fr.can_id!= 0x02)  //id判断
	//{
    //  index = 0;
	//	return 0;
	//}

	if(index + fr->can_dlc <= size){
		memcpy(bufout + index, (uint8_t*)fr->data, fr->can_dlc);
		index += fr->can_dlc;
	}else{
		index = 0;
		return 0;
	}

	if((bufout[1] == 0x10)){    //功能码 0x10
		dstLen = 8;
	}else if(bufout[1] == 0x04){ //功能码 0x04
		if(index > 6){dstLen = bufout[2] + 5;}
	}else if(bufout[1] == 0x03){ //功能码 0x03
		if(index > 6){dstLen = bufout[2] + 5;}
	}

	if (dstLen > 255)  //目标长度错误
	{
		index = 0;
		return 0;
	}

	if (index >= dstLen)  //收到完整帧
	{
		index = 0;
		return dstLen;
	}

    return 0;
}

void IO_Process_Data_From_DSP_0x04(uint8_t *data)                           //处理DSP返回数据   ccg 2012.12.21
{
	static uint16_t freq_stack[40]={0};
	static uint8_t i=0;

	System_Info.EMS_DSP_Com_Count[0] = 2;  // caoxh 1s 超时报警
	DSP_Com_Err[0]=false;
	//测试DSP返回数据
	VSG_Info.Sync_Ready_Flag = GetU16(data + 3);
	VSG_Info.Grid_freq = GetU16(data + 5);
	VSG_Info.Grid_voltage_Vab = GetU16(data + 7);
	VSG_Info.Grid_voltage_Vbc = GetU16(data + 9);
	//VSG_Info.Grid_voltage = GetU16(data + 11);
	//VSG_Info.MGrid_voltage = GetU16(data + 13);
	//VSG_Info.Environment_temp = GetU16(data + 15);

	if(VSG_Info.Grid_freq<4000)
	{
		for(uint8_t j=0;j<40;j++)
			freq_stack[j]=0;
		VSG_Info.Grid_Freq_Average=0;
	}
	else
	{
		uint32_t Temp_val=0;
		freq_stack[i]=VSG_Info.Grid_freq;
		i=(i+1)%40;

		for(uint8_t m=0;m<40;m++)
		{
			if(freq_stack[m]!=0)
			{
				Temp_val+=freq_stack[m];
			}
		}
		VSG_Info.Grid_Freq_Average = (uint16_t)(Temp_val/40);
	}
}

void IO_Process_Data_From_DSP_By_UART_Or_CAN(uint8_t *data,uint32_t len)    //处理DSP发来的数据 ccg 2013.1.14
{
	uint16_t crcresult=embf_get_crc16(data,len-2);
	if(data[len-2]== LOBYTE(crcresult) && data[len-1]== HIBYTE(crcresult))
	{
		{
			if(data[1] == 0x04)
			{
				IO_Process_Data_From_DSP_0x04(data);
			}
			else if(data[1] == 0x10)
			{
				if((data[1]==0x10) && (data[2]==0x27) && (data[3]==0x42) && (data[4]==0x00) && (data[5]==0x02))
				{
					Send_Data_To_DSP_Flag = false;//说明设置数据已经收到，不用再发
				}
			}
		}
	}
}

uint8_t IO_Perpare_To_Send_Data_To_DSP(uint8_t *buf)                        //每秒向DSP发送数据或者查询数据
{
	uint8_t  length=0;
	uint16_t l_crc=0;

	if(Send_Data_To_DSP_Flag==true)
	{
		buf[0]=0x01;
		buf[1]=0x10;
		buf[2]=0x27;
		buf[3]=0x42;
		buf[4]=0x00;
		buf[5]=0x02;
		buf[6]=0x04;

		buf[7]=0x00;
		buf[8]=VSG_Info.Set_info.Grid_on_permit;
		buf[9]=0x00;
		buf[10]=VSG_Info.Sync_Grid_Contactor_State;

		l_crc=embf_get_crc16(buf,11);
		buf[11]=LOBYTE(l_crc);
		buf[12]=HIBYTE(l_crc);
		length=13;
	}
	else//查询数据（0x04命令）
	{
		buf[0]=0x01;
		buf[1]=0x04;
		buf[2]=0x27;
		buf[3]=0x42;
		buf[4]=0x00;
		buf[5]=0x04;
		l_crc=embf_get_crc16(buf,6);
		buf[6]=LOBYTE(l_crc);
		buf[7]=HIBYTE(l_crc);
		length=8;
	}
	return length;
}
void IO_Send_Data_To_DSP_By_CAN(int32_t fd, uint8_t *buf, uint8_t size)    //CAN1回复DSP数据   ccg 2013.1.14
{
    struct can_frame fr;
    uint8_t *data = buf;
    uint8_t j = 0;
	uint8_t count = 0;

	fr.can_id = (can_id & CAN_SFF_MASK);
    count = size >> 3;
    if((size % 8) == 0){
        for(j = 0; j < count; j++)
        {
        	fr.can_dlc = 8;
            memcpy(fr.data, data + (uint8_t)(j << 3), 8);
            write(fd, &fr, sizeof(fr));
        }
    }else{
        for(j = 0; j < count; j++)
        {
        	fr.can_dlc = 8;
            memcpy(fr.data, data + (uint8_t)(j << 3), 8);
            write(fd, &fr, sizeof(fr));
        }

        fr.can_dlc = size % 8;
        memcpy(fr.data, data + (size - fr.can_dlc), (uint32_t)fr.can_dlc);
        write(fd, &fr, sizeof(fr));
    }
}
