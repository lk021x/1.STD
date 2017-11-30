/*
 * com_io_env.cpp
 *
 *  Created on: 2017-7-19
 *      Author: root
 */

#include "includes.h"

void EMS_Process_Data_From_ENV_0x04(uint8_t *data,uint8_t which)
{
	ENV_Info[which].Sunshine_Radiation_Intensity1		= GetU16(data +  29);
	ENV_Info[which].Env_Temperature						= GetU16(data +  31);
	ENV_Info[which].Battery_Board_Temperature			= GetU16(data +  33);
	ENV_Info[which].Wind_Speed							= GetU16(data +  39);
	ENV_Info[which].Wind_Level							= GetU16(data +  41);
	ENV_Info[which].Wind_Direction_Degree				= GetU16(data +  43);
	ENV_Info[which].Wind_Position						= GetU16(data +  47);
	ENV_Info[which].Day_Radiation1						= GetU32(data +  49);
	ENV_Info[which].All_Radiation1						= GetU32(data +  57);
	ENV_Info[which].Atmospheric_Pressure				= GetU16(data +  79);
	ENV_Info[which].Env_Humidity						= GetU16(data +  81);
	ENV_Info[which].Sunshine_Radiation_Intensity2		= GetU16(data +  85);
	ENV_Info[which].Day_Radiation2						= GetU32(data +  87);
	ENV_Info[which].All_Radiation2						= GetU32(data +  95);
}
void EMS_Process_Data_From_ENV_By_UART(uint8_t *data,uint32_t len)
{
	uint16_t crcresult=embf_get_crc16(data,len-2);
	if(data[len-2]== LOBYTE(crcresult) && data[len-1]== HIBYTE(crcresult))
	{
		for(uint8_t i=0;i<ENV_NUM;i++)
		{
			if(data[0]==EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address)
			{
				System_Info.EMS_ENV_Com_Count[i]=ENV_SIGNAL_FULL;
				ENV_Com_Err[i]=false;
				
				if(data[1] == 0x04)
				{
					EMS_Process_Data_From_ENV_0x04(data,i);
				}
			}
		}
	}
}

uint8_t EMS_Perpare_To_Send_Data_To_ENV(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)
{
	uint8_t  length=0;
	uint16_t l_crc=0;
	static uint8_t i[GENERAL_SERIAL_NUM]={0};

	for(;;)
	{
		if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i[id]].Modubus_Address>=stID &&
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i[id]].Modubus_Address<=endID)
		{
		    buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i[id]].Modubus_Address;
		    buf[1]=0x04;
		    buf[2]=0x23;  //9000
		    buf[3]=0x27;
		    buf[4]=0x00;
		    buf[5]=0x32;  //50
		    l_crc=embf_get_crc16(buf,6);
		    buf[6]=LOBYTE(l_crc);
		    buf[7]=HIBYTE(l_crc);
		    length=8;
			i[id]++;
			i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_ENV_Number;
			break;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_ENV_Number)
			{
				i[id]=0;
				break;
			}
		}
	}
	return length;
}

