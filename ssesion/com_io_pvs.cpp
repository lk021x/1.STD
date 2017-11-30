/*
 * com_io_pvs.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#include "includes.h"

void EMS_Process_Data_From_PVS_0x04(uint8_t *data,uint8_t which)
{
	PVS_Info[which].DC_Vol				= GetU16(data +  3);
	PVS_Info[which].Room_T				= (int16_t)GetU16(data +  5);
	PVS_Info[which].Max_I				= GetU16(data +  13);
	PVS_Info[which].Ave_I				= GetU16(data +  15);

	for(uint8_t i=0;i<16;i++)
	{
		PVS_Info[which].Path_I[i]		= GetU16(data+17+2*i);
	}
	
	PVS_Info[which].Total_I				= GetU32(data + 53);
	PVS_Info[which].Total_P_DC			= GetU32(data + 57);
	PVS_Info[which].Output_Pt_Day		= GetU32(data + 61);
	PVS_Info[which].Output_Pt_Total		= GetU32(data + 65);
	
	PVS_Info[which].Work_Status			= GetU32(data + 73);

	//PVS_Info[which].Short_Data		= GetU32(data + 77);
	//PVS_Info[which].Reverse_I			= GetU32(data + 81);
	//PVS_Info[which].OI_Data			= GetU32(data + 85);
	//PVS_Info[which].Fuse_Off			= GetU32(data + 89);
	//PVS_Info[which].UI_Data			= GetU32(data + 93);
	//
	//PVS_Info[which].Open_Data			= GetU32(data + 101);
	//
	//for(uint8_t i=0;i<16;i++)
	//{
	//	PVS_Info[which].Path_P[i]		= GetU16(data+109+2*i);
	//}
}
void EMS_Process_Data_From_PVS_By_UART(uint8_t *data,uint32_t len)
{
	uint16_t crcresult=embf_get_crc16(data,len-2);
	if(data[len-2]== LOBYTE(crcresult) && data[len-1]== HIBYTE(crcresult))
	{
		for(uint8_t i=0;i<PVS_NUM;i++)
		{
			if(data[0]==EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address)
			{
				System_Info.EMS_PVS_Com_Count[i]=PVS_SIGNAL_FULL;
				PVS_Com_Err[i]=false;
				
				if(data[1] == 0x04)
				{
					EMS_Process_Data_From_PVS_0x04(data,i);
				}
			}
		}
	}
}

uint8_t EMS_Perpare_To_Send_Data_To_PVS(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)
{
	uint8_t  length=0;
	uint16_t l_crc=0;
	static uint8_t i[GENERAL_SERIAL_NUM]={0};

	for(;;)
	{
		if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i[id]].Modubus_Address>=stID &&
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i[id]].Modubus_Address<=endID)
		{
		    buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i[id]].Modubus_Address;
		    buf[1]=0x04;
		    buf[2]=0x1B;
		    buf[3]=0x5D;
		    buf[4]=0x00;
		    buf[5]=0x25; //0x45;
		    l_crc=embf_get_crc16(buf,6);
		    buf[6]=LOBYTE(l_crc);
		    buf[7]=HIBYTE(l_crc);
		    length=8;
			i[id]++;
			i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number;
			break;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number)
			{
				i[id]=0;
				break;
			}
		}
	}
	return length;
}

