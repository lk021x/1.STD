/*
 * com_io_pv.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#include "includes.h"

void EMS_Process_Data_From_PV_0x03(uint8_t *data, uint8_t which)                           //处理DSP返回数据   ccg 2012.12.21
{
	PV_Info[which].PV4x.ParaSet.Year = GetU16(data + 3);
	PV_Info[which].PV4x.ParaSet.Month = GetU16(data + 5);
	PV_Info[which].PV4x.ParaSet.Day = GetU16(data + 7);
	PV_Info[which].PV4x.ParaSet.Hour = GetU16(data + 9);
	PV_Info[which].PV4x.ParaSet.Minute = GetU16(data + 11);
	PV_Info[which].PV4x.ParaSet.Second = GetU16(data + 13);
	PV_Info[which].PV4x.ParaSet.Power_ONOFF = GetU16(data + 15);
	PV_Info[which].PV4x.ParaSet.PowerLimit_Switch = GetU16(data + 17);
	PV_Info[which].PV4x.ParaSet.PowerLimit_Set = GetU16(data + 19);
}
void EMS_Process_Data_From_PV_0x04(uint8_t *data, uint8_t which)                           //处理DSP返回数据   ccg 2012.12.21
{
	PV_Info[which].PV3x.RunParameter.Type_Code = GetU16(data + 3);
	PV_Info[which].PV3x.RunParameter.Rated_Output_Power = GetU16(data + 5);
	PV_Info[which].PV3x.RunParameter.Output_Type = GetU16(data + 7);
	PV_Info[which].PV3x.RunParameter.Day_Generation = GetU16(data + 9);
	PV_Info[which].PV3x.RunParameter.Total_Generation = GetU32(data + 11);
	PV_Info[which].PV3x.RunParameter.Total_Run_Time = GetU32(data + 15);
	PV_Info[which].PV3x.RunParameter.Inside_Temper = GetU16(data + 19);

	PV_Info[which].PV3x.RunParameter.DC_Voltage1 = GetU16(data + 25);
	PV_Info[which].PV3x.RunParameter.DC_Current1 = GetU16(data + 27);
	PV_Info[which].PV3x.RunParameter.DC_Voltage2 = GetU16(data + 29);
	PV_Info[which].PV3x.RunParameter.DC_Current2 = GetU16(data + 31);

	PV_Info[which].PV3x.RunParameter.u32_Total_DC_Power = GetU32(data + 37);
	PV_Info[which].PV3x.RunParameter.VA = GetU16(data + 41);
	PV_Info[which].PV3x.RunParameter.VB = GetU16(data + 43);
	PV_Info[which].PV3x.RunParameter.VC = GetU16(data + 45);
	PV_Info[which].PV3x.RunParameter.IA = GetU16(data + 47);
	PV_Info[which].PV3x.RunParameter.IB = GetU16(data + 49);
	PV_Info[which].PV3x.RunParameter.IC = GetU16(data + 51);

	PV_Info[which].PV3x.RunParameter.u32_Total_Active_Power = GetU32(data + 65);
	PV_Info[which].PV3x.RunParameter.s32_Reactive_Power = GetU32(data + 69);
	PV_Info[which].PV3x.RunParameter.Power_Factor = GetU16(data + 73);
	PV_Info[which].PV3x.RunParameter.Frequency = GetU16(data + 75);
	PV_Info[which].PV3x.RunParameter.Inverter_Efficiecy = GetU16(data + 77);
	PV_Info[which].PV3x.RunParameter.Inverter_Status = GetU16(data + 79);
	PV_Info[which].PV3x.RunParameter.StatusTime_Year = GetU16(data + 81);
	PV_Info[which].PV3x.RunParameter.StatusTime_Month = GetU16(data + 83);
	PV_Info[which].PV3x.RunParameter.StatusTime_Day = GetU16(data + 85);
	PV_Info[which].PV3x.RunParameter.StatusTime_Hour = GetU16(data + 87);
	PV_Info[which].PV3x.RunParameter.StatusTime_Minute = GetU16(data + 89);
	PV_Info[which].PV3x.RunParameter.StatusTime_Second = GetU16(data + 91);
	PV_Info[which].PV3x.RunParameter.Status_Data1 = GetU16(data + 93);

	PV_Info[which].PV3x.RunParameter.Rated_Reactive_Power_Output = GetU16(data + 101);
	PV_Info[which].PV3x.RunParameter.Fault_Status1 = GetU32(data + 103);
	PV_Info[which].PV3x.RunParameter.Fault_Status2 = GetU32(data + 107);

	PV_Info[which].PV3x.RunParameter.Total_DC_Power = (uint32_t) (PV_Info[which].PV3x.RunParameter.u32_Total_DC_Power / 100);
	PV_Info[which].PV3x.RunParameter.Total_Active_Power = (uint32_t) (PV_Info[which].PV3x.RunParameter.u32_Total_Active_Power / 100);
	PV_Info[which].PV3x.RunParameter.Reactive_Power = (int32_t) (PV_Info[which].PV3x.RunParameter.s32_Reactive_Power / 100);

	//仅以下型号支持故障1和故障2
	if((PV_Info[which].PV3x.RunParameter.Type_Code!=0x0098)&&(PV_Info[which].PV3x.RunParameter.Type_Code!=0x00B0)&&\
	   (PV_Info[which].PV3x.RunParameter.Type_Code!=0x009F)&&(PV_Info[which].PV3x.RunParameter.Type_Code!=0x00A0)&&\
	   (PV_Info[which].PV3x.RunParameter.Type_Code!=0x0095))
	{
		PV_Info[which].PV3x.RunParameter.Fault_Status1					= 0;
		PV_Info[which].PV3x.RunParameter.Fault_Status2					= 0;
	}

}
int EMS_Process_Data_From_PV_By_UART(uint8_t *data, uint32_t len)    //处理DSP发来的数据 ccg 2013.1.14
{
	uint16_t crcresult = embf_get_crc16(data, len - 2);
	if (data[len - 2] == LOBYTE(crcresult) && data[len - 1] == HIBYTE(crcresult) )
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
		{
			if (data[0] == EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address)
			{
				System_Info.EMS_PV_Com_Count[i] = SIGNAL_FULL;
				PV_Com_Err[i] = false;

				if (data[1] == 0x03)
				{
					EMS_Process_Data_From_PV_0x03(data, i);
					Search_PV_Info_0x03[i] = false;
				}
				else if (data[1] == 0x04)
				{
					EMS_Process_Data_From_PV_0x04(data,i);
					Search_PV_Info_0x04[i]=false;
				}
				else if (data[1] == 0x10)
				{
					if ((data[1] == 0x10) && (data[2] == 0x13) && (data[3] == 0x87) && (data[4] == 0x00) && (data[5] == 0x06))
					{
						Send_Data_To_PV_0x10_01[i] = false;    //说明设置数据已经收到，不用再发
					}

					if ((data[1] == 0x10) && (data[2] == 0x13) && (data[3] == 0x8D) && (data[4] == 0x00) && (data[5] == 0x03))
					{
						Send_Data_To_PV_0x10_02[i] = false;    //说明设置数据已经收到，不用再发
					}
				}
			}
		}
	}else{
		return -1;
	}
	return 0;
}

void EMS_Process_Data_From_PV_By_Net(uint8_t *data,uint32_t len)    //
{
    uint16_t Modbus_type    = (uint16_t)(((uint16_t)data[2] << 8) | data[3]);
    uint16_t data_amount    = (uint16_t)(((uint16_t)data[4] << 8) | data[5]);
	if(Modbus_type==0)
	{
		if(data_amount==(len-6))
		{
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
			{
				if(data[6]==EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address)
				{
					System_Info.EMS_PV_Com_Count[i]=SIGNAL_FULL;
					PV_Com_Err[i]=false;
					if(data[7] == 0x03)
					{
						EMS_Process_Data_From_PV_0x03(data+6, i);
						Search_PV_Info_0x03[i] = false;
					}
					else if(data[7] == 0x04)
					{
						EMS_Process_Data_From_PV_0x04(data+6, i);
						Search_PV_Info_0x04[i] = false;
					}
					else if(data[7] == 0x10)
					{
						if ((data[7] == 0x10) && (data[8] == 0x13) && (data[9] == 0x87) && (data[10] == 0x00) && (data[11] == 0x06))
						{
							Send_Data_To_PV_0x10_01[i] = false;    //说明设置数据已经收到，不用再发
						}

						if ((data[7] == 0x10) && (data[8] == 0x13) && (data[9] == 0x8D) && (data[10] == 0x00) && (data[11] == 0x03))
						{
							Send_Data_To_PV_0x10_02[i] = false;    //说明设置数据已经收到，不用再发
						}
					}
				}
			}
		}
	}
}

//注意：设备的最大台数与起始结束ID没有关系，起始结束ID与设备ID有对应关系，例如： 当  2 =< id <= 4, 表明理论3台设备，对应ID:2,3,4，这个ID与默认ID不同需要手动改
uint8_t EMS_Perpare_To_Send_Data_To_PV_By_UART(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)                        //每秒向PV发送数据或者查询数据
{
	uint8_t length = 0;
	uint16_t l_crc = 0;
	static uint8_t i[GENERAL_SERIAL_NUM]={0};

	for(;;)
	{
		if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i[id]].Modubus_Address>=stID &&
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i[id]].Modubus_Address<=endID)
		{

			if (Search_PV_Info_0x04[i[id]] == true)
			{
				buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i[id]].Modubus_Address;
				buf[1] = 0x04;
				buf[2] = 0x13;
				buf[3] = 0x87;
				buf[4] = 0x00;
				buf[5] = 0x53;
				l_crc = embf_get_crc16(buf, 6);
				buf[6] = LOBYTE(l_crc);
				buf[7] = HIBYTE(l_crc);
				length = 8;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;
			}
			else if(Search_PV_Info_0x03[i[id]] == true)
			{
				buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i[id]].Modubus_Address;
				buf[1] = 0x03;
				buf[2] = 0x13;
				buf[3] = 0x87;
				buf[4] = 0x00;
				buf[5] = 0x09;
				l_crc = embf_get_crc16(buf, 6);
				buf[6] = LOBYTE(l_crc);
				buf[7] = HIBYTE(l_crc);
				length = 8;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;
			}
			else if (Send_Data_To_PV_0x10_01[i[id]] == true)
			{
				buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i[id]].Modubus_Address;
				buf[1] = 0x10;
				buf[2] = 0x13;
				buf[3] = 0x87;
				buf[4] = 0x00;
				buf[5] = 0x06;
				buf[6] = 0x0C;

				buf[7] = HIBYTE((showTime.tm_year)+2000);
				buf[8] = LOBYTE((showTime.tm_year)+2000);
				buf[9] = HIBYTE((showTime.tm_mon));
				buf[10] = LOBYTE((showTime.tm_mon));
				buf[11] = HIBYTE((showTime.tm_day));
				buf[12] = LOBYTE((showTime.tm_day));
				buf[13] = HIBYTE((showTime.tm_hour));
				buf[14] = LOBYTE((showTime.tm_hour));
				buf[15] = HIBYTE((showTime.tm_min));
				buf[16] = LOBYTE((showTime.tm_min));
				buf[17] = HIBYTE((showTime.tm_sec));
				buf[18] = LOBYTE((showTime.tm_sec));

				l_crc = embf_get_crc16(buf, 19);
				buf[19] = LOBYTE(l_crc);
				buf[20] = HIBYTE(l_crc);
				length = 21;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;
			}
			else if((Send_Data_To_PV_Broadcast == true)&&(ENABLE == Send_Data_To_PV_Broadcast_Enable))
			{
				Send_Data_To_PV_Broadcast=false;
				buf[0]=0x00;
				buf[1]=0x10;
				buf[2]=0x13;
				buf[3]=0x8E;   //5006
				buf[4]=0x00;
				buf[5]=0x02;
				buf[6]=0x04;

				buf[7]=0x00;                                   // 5006 限功率开关--使能
				buf[8]=0xAA;
				buf[9]=HIBYTE(PV_Ctrl_Data[0].PowerLimit_Set); //限功率比例
				buf[10]=LOBYTE(PV_Ctrl_Data[0].PowerLimit_Set);
				l_crc=embf_get_crc16(buf,11);
				buf[11]=LOBYTE(l_crc);
				buf[12]=HIBYTE(l_crc);
				length=13;

				for(uint8_t j=0;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;j++)
				{
					Search_PV_Info_0x03[j]=true;
					Search_PV_Info_0x04[j]=true;
				}
				i[id]=0;
			}
			else if (Send_Data_To_PV_0x10_02[i[id]] == true)
			{
				buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i[id]].Modubus_Address;
				buf[1] = 0x10;
				buf[2] = 0x13;
				buf[3] = 0x8D;
				buf[4] = 0x00;
				buf[5] = 0x03;
				buf[6] = 0x06;

				buf[7] = HIBYTE(PV_Ctrl_Data[i[id]].Power_ONOFF);
				buf[8] = LOBYTE(PV_Ctrl_Data[i[id]].Power_ONOFF);
				buf[9] = 0;
				buf[10] = 0xAA;
				buf[11] = HIBYTE(PV_Ctrl_Data[i[id]].PowerLimit_Set);
				buf[12] = LOBYTE(PV_Ctrl_Data[i[id]].PowerLimit_Set);
				l_crc = embf_get_crc16(buf, 13);
				buf[13] = LOBYTE(l_crc);
				buf[14] = HIBYTE(l_crc);
				length = 15;
				Search_PV_Info_0x03[i[id]] = true;
				Search_PV_Info_0x04[i[id]] = true;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;
			}
			else
			{
				buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i[id]].Modubus_Address;
				buf[1] = 0x04;
				buf[2] = 0x13;
				buf[3] = 0x87;
				buf[4] = 0x00;
				buf[5] = 0x53;
				l_crc  = embf_get_crc16(buf, 6);
				buf[6] = LOBYTE(l_crc);
				buf[7] = HIBYTE(l_crc);
				length = 8;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;
			}
			break;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number)
			{
				i[id]=0;
				break;
			}
		}
	}
	return length;
}

uint8_t EMS_Perpare_To_Send_Data_To_PV_By_Net(uint8_t *buf, uint16_t id)  //PV网口查询数据
{
	uint8_t length=0;
	uint16_t i=id;
	static uint16_t TCPClient_CNT[PV_NUM]={0};

	TCPClient_CNT[i]++; //tcp modbus包号

	if(Search_PV_Info_0x04[i] == true)
	{
		buf[6] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address;
		buf[7] = 0x04;
		buf[8] = 0x13;
		buf[9] = 0x87;
		buf[10] = 0x00;
		buf[11] = 0x53;
		length = 12;
	}
	else if(Search_PV_Info_0x03[i] == true)
	{
		buf[6] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address;
		buf[7] = 0x03;
		buf[8] = 0x13;
		buf[9] = 0x87;
		buf[10] = 0x00;
		buf[11] = 0x09;
		length = 12;
	}
	else if (Send_Data_To_PV_0x10_01[i] == true)
	{
		buf[6] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address;
		buf[7] = 0x10;
		buf[8] = 0x13;
		buf[9] = 0x87;   //地址10050  ccg 2013.1.16  状态切换随运行参数一起下发
		buf[10] = 0x00;
		buf[11] = 0x06;
		buf[12] = 0x0C;

		buf[13] = HIBYTE((showTime.tm_year)+2000);
		buf[14] = LOBYTE((showTime.tm_year)+2000);
		buf[15] = HIBYTE((showTime.tm_mon));
		buf[16] = LOBYTE((showTime.tm_mon));
		buf[17] = HIBYTE((showTime.tm_day));
		buf[18] = LOBYTE((showTime.tm_day));
		buf[19] = HIBYTE((showTime.tm_hour));
		buf[20] = LOBYTE((showTime.tm_hour));
		buf[21] = HIBYTE((showTime.tm_min));
		buf[22] = LOBYTE((showTime.tm_min));
		buf[23] = HIBYTE((showTime.tm_sec));
		buf[24] = LOBYTE((showTime.tm_sec));
		length = 25;
		buf[0]=HIBYTE(TCPClient_CNT[i]);
		buf[1]=LOBYTE(TCPClient_CNT[i]);
		buf[2]=0;
		buf[3]=0;
		buf[4]=HIBYTE(length-6);
		buf[5]=LOBYTE(length-6);

		return length;
	}
	else if (Send_Data_To_PV_0x10_02[i] == true)
	{
		buf[6] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address;
		buf[7] = 0x10;
		buf[8] = 0x13;
		buf[9] = 0x8D;   //地址10050  ccg 2013.1.16  状态切换随运行参数一起下发
		buf[10] = 0x00;
		buf[11] = 0x03;
		buf[12] = 0x06;

		buf[13] = HIBYTE(PV_Ctrl_Data[i].Power_ONOFF);
		buf[14] = LOBYTE(PV_Ctrl_Data[i].Power_ONOFF);
		buf[15] = 0;
		buf[16] = 0xAA;
		buf[18] = HIBYTE(PV_Ctrl_Data[i].PowerLimit_Set);
		buf[18] = LOBYTE(PV_Ctrl_Data[i].PowerLimit_Set);
		length = 19;
		Search_PV_Info_0x03[i] = true;
		Search_PV_Info_0x04[i] = true;
		buf[0]=HIBYTE(TCPClient_CNT[i]);
		buf[1]=LOBYTE(TCPClient_CNT[i]);
		buf[2]=0;
		buf[3]=0;
		buf[4]=HIBYTE(length-6);
		buf[5]=LOBYTE(length-6);

		return length;
	}
	else
	{
		buf[6] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address;
		buf[7] = 0x04;
		buf[8] = 0x13;
		buf[9] = 0x87;
		buf[10] = 0x00;
		buf[11] = 0x53;
		length = 12;
	}

	buf[0]=HIBYTE(TCPClient_CNT[i]);
	buf[1]=LOBYTE(TCPClient_CNT[i]);
	buf[2]=0;
	buf[3]=0;
	buf[4]=HIBYTE(length-6);
	buf[5]=LOBYTE(length-6);

	return length;
}

uint8_t EMS_Perpare_To_Send_Para_To_PV_By_Net(uint8_t *buf, uint16_t id)
{
	uint8_t length=0;
	uint16_t i=id;
	static uint16_t TCPClient_CNT[PV_NUM]={0};

	TCPClient_CNT[i]++; //tcp modbus包号

	if (Send_Data_To_PV_0x10_01[i] == true)
	{
		buf[6] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address;
		buf[7] = 0x10;
		buf[8] = 0x13;
		buf[9] = 0x87;   //地址10050  ccg 2013.1.16  状态切换随运行参数一起下发
		buf[10] = 0x00;
		buf[11] = 0x06;
		buf[12] = 0x0C;

		buf[13] = HIBYTE((showTime.tm_year)+2000);
		buf[14] = LOBYTE((showTime.tm_year)+2000);
		buf[15] = HIBYTE((showTime.tm_mon));
		buf[16] = LOBYTE((showTime.tm_mon));
		buf[17] = HIBYTE((showTime.tm_day));
		buf[18] = LOBYTE((showTime.tm_day));
		buf[19] = HIBYTE((showTime.tm_hour));
		buf[20] = LOBYTE((showTime.tm_hour));
		buf[21] = HIBYTE((showTime.tm_min));
		buf[22] = LOBYTE((showTime.tm_min));
		buf[23] = HIBYTE((showTime.tm_sec));
		buf[24] = LOBYTE((showTime.tm_sec));
		length = 25;
		buf[0]=HIBYTE(TCPClient_CNT[i]);
		buf[1]=LOBYTE(TCPClient_CNT[i]);
		buf[2]=0;
		buf[3]=0;
		buf[4]=HIBYTE(length-6);
		buf[5]=LOBYTE(length-6);

		return length;
	}
	else if (Send_Data_To_PV_0x10_02[i] == true)
	{
		buf[6] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address;
		buf[7] = 0x10;
		buf[8] = 0x13;
		buf[9] = 0x8D;   //地址10050  ccg 2013.1.16  状态切换随运行参数一起下发
		buf[10] = 0x00;
		buf[11] = 0x03;
		buf[12] = 0x06;

		buf[13] = HIBYTE(PV_Ctrl_Data[i].Power_ONOFF);
		buf[14] = LOBYTE(PV_Ctrl_Data[i].Power_ONOFF);
		buf[15] = 0;
		buf[16] = 0xAA;
		buf[18] = HIBYTE(PV_Ctrl_Data[i].PowerLimit_Set);
		buf[18] = LOBYTE(PV_Ctrl_Data[i].PowerLimit_Set);
		length = 19;
		Search_PV_Info_0x03[i] = true;
		Search_PV_Info_0x04[i] = true;
		buf[0]=HIBYTE(TCPClient_CNT[i]);
		buf[1]=LOBYTE(TCPClient_CNT[i]);
		buf[2]=0;
		buf[3]=0;
		buf[4]=HIBYTE(length-6);
		buf[5]=LOBYTE(length-6);

		return length;
	}
	return 0;
}
