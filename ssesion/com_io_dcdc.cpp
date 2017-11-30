/*
 * com_io_dcdc.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#include "includes.h"

void EMS_Process_Data_From_DCDC_0x04(uint8_t *data,uint8_t num,uint8_t which)//处理DCDC返回数据   ccg 2012.12.21
{
    if(num==0) //模组1、2
	{
	    if(System_Info.Config.DCDC_Type==1) //单向DCDC
		{
			DCDC_Info[which].Module_1.Module_Array_V					= GetU16(data +  3);
			DCDC_Info[which].Module_1.Module_Array_I					= GetU16(data +  5);
			DCDC_Info[which].Module_1.Module_Bat_V						= GetU16(data +  7);
			DCDC_Info[which].Module_1.Module_Charge_I					= GetU16(data +  9);
			DCDC_Info[which].Module_1.Module_Charge_P					= GetU32(data + 11);
			DCDC_Info[which].Module_1.Module_In_Temp					= GetU16(data + 15);
			DCDC_Info[which].Module_1.Module_Bat_Temp					= GetU16(data + 17);
			DCDC_Info[which].Module_1.Module_Dev_Status					= GetU16(data + 19);
			DCDC_Info[which].Module_1.Module_Dev_Fault_Status			= GetU16(data + 21);
			DCDC_Info[which].Module_1.Module_Output_E_Day				= GetU16(data + 23);
			DCDC_Info[which].Module_1.Module_Output_E_Mon				= GetU32(data + 25);
			DCDC_Info[which].Module_1.Module_Output_E_Total				= GetU32(data + 29);
			DCDC_Info[which].Module_1.Module_CO2_Reduced				= GetU32(data + 33);
			DCDC_Info[which].Module_1.Module_Run_Min_Day				= GetU16(data + 37);
			DCDC_Info[which].Module_1.Module_Run_Hour_Mon				= GetU16(data + 39);
			DCDC_Info[which].Module_1.Module_Run_Hour_Total				= GetU32(data + 41);
			
			DCDC_Info[which].Module_2.Module_Array_V					= GetU16(data + 63);
			DCDC_Info[which].Module_2.Module_Array_I					= GetU16(data + 65);
			DCDC_Info[which].Module_2.Module_Bat_V						= GetU16(data + 67);
			DCDC_Info[which].Module_2.Module_Charge_I					= GetU16(data + 69);
			DCDC_Info[which].Module_2.Module_Charge_P					= GetU32(data + 71);
			DCDC_Info[which].Module_2.Module_In_Temp					= GetU16(data + 75);
			DCDC_Info[which].Module_2.Module_Bat_Temp					= GetU16(data + 77);
			DCDC_Info[which].Module_2.Module_Dev_Status					= GetU16(data + 79);
			DCDC_Info[which].Module_2.Module_Dev_Fault_Status			= GetU16(data + 81);
			DCDC_Info[which].Module_2.Module_Output_E_Day				= GetU16(data + 83);
			DCDC_Info[which].Module_2.Module_Output_E_Mon				= GetU32(data + 85);
			DCDC_Info[which].Module_2.Module_Output_E_Total				= GetU32(data + 89);
			DCDC_Info[which].Module_2.Module_CO2_Reduced				= GetU32(data + 93);
			DCDC_Info[which].Module_2.Module_Run_Min_Day				= GetU16(data + 97);
			DCDC_Info[which].Module_2.Module_Run_Hour_Mon				= GetU16(data + 99);
			DCDC_Info[which].Module_2.Module_Run_Hour_Total				= GetU32(data + 101);
		}
        else if(System_Info.Config.DCDC_Type==2) //双向DCDC
		{
			DCDC_Info[which].Module_1.Bus_Bar_I							= GetU16(data +  3);
			DCDC_Info[which].Module_1.Battery_I							= GetU16(data +  5);
			DCDC_Info[which].Module_1.Module1_T							= GetU16(data +  7);
			DCDC_Info[which].Module_1.Module2_T							= GetU16(data +  9);
			DCDC_Info[which].Module_1.Device_Status						= GetU16(data + 11);
			DCDC_Info[which].Module_1.Fault_Status						= GetU16(data + 13);

			DCDC_Info[which].Module_2.Bus_Bar_I							= GetU16(data + 43);
			DCDC_Info[which].Module_2.Battery_I							= GetU16(data + 45);
			DCDC_Info[which].Module_2.Module1_T							= GetU16(data + 47);
			DCDC_Info[which].Module_2.Module2_T							= GetU16(data + 49);
			DCDC_Info[which].Module_2.Device_Status						= GetU16(data + 51);
			DCDC_Info[which].Module_2.Fault_Status						= GetU16(data + 53);

			DCDC_Info[which].Module_3.Bus_Bar_I							= GetU16(data + 83);
			DCDC_Info[which].Module_3.Battery_I							= GetU16(data + 85);
			DCDC_Info[which].Module_3.Module1_T							= GetU16(data + 87);
			DCDC_Info[which].Module_3.Module2_T							= GetU16(data + 89);
			DCDC_Info[which].Module_3.Device_Status						= GetU16(data + 91);
			DCDC_Info[which].Module_3.Fault_Status						= GetU16(data + 93);

			//DCDC_Info[which].Module_4.Bus_Bar_I							= GetU16(data + 123);
			//DCDC_Info[which].Module_4.Battery_I							= GetU16(data + 125);
			//DCDC_Info[which].Module_4.Module1_T							= GetU16(data + 127);
			//DCDC_Info[which].Module_4.Module2_T							= GetU16(data + 129);
			//DCDC_Info[which].Module_4.Device_Status						= GetU32(data + 131);
			//DCDC_Info[which].Module_4.Fault_Status						= GetU16(data + 133);
		}
	}
    if(num==1) //模组3、4
    {
		if(System_Info.Config.DCDC_Type==1)//单向DCDC
		{
			DCDC_Info[which].Module_3.Module_Array_V					= GetU16(data +  3);
			DCDC_Info[which].Module_3.Module_Array_I					= GetU16(data +  5);
			DCDC_Info[which].Module_3.Module_Bat_V						= GetU16(data +  7);
			DCDC_Info[which].Module_3.Module_Charge_I					= GetU16(data +  9);
			DCDC_Info[which].Module_3.Module_Charge_P					= GetU32(data + 11);
			DCDC_Info[which].Module_3.Module_In_Temp					= GetU16(data + 15);
			DCDC_Info[which].Module_3.Module_Bat_Temp					= GetU16(data + 17);
			DCDC_Info[which].Module_3.Module_Dev_Status 				= GetU16(data + 19);
			DCDC_Info[which].Module_3.Module_Dev_Fault_Status			= GetU16(data + 21);
			DCDC_Info[which].Module_3.Module_Output_E_Day				= GetU16(data + 23);
			DCDC_Info[which].Module_3.Module_Output_E_Mon				= GetU32(data + 25);
			DCDC_Info[which].Module_3.Module_Output_E_Total 			= GetU32(data + 29);
			DCDC_Info[which].Module_3.Module_CO2_Reduced				= GetU32(data + 33);
			DCDC_Info[which].Module_3.Module_Run_Min_Day				= GetU16(data + 37);
			DCDC_Info[which].Module_3.Module_Run_Hour_Mon				= GetU16(data + 39);
			DCDC_Info[which].Module_3.Module_Run_Hour_Total 			= GetU32(data + 41);

			DCDC_Info[which].Module_4.Module_Array_V					= GetU16(data + 63);
			DCDC_Info[which].Module_4.Module_Array_I					= GetU16(data + 65);
			DCDC_Info[which].Module_4.Module_Bat_V						= GetU16(data + 67);
			DCDC_Info[which].Module_4.Module_Charge_I					= GetU16(data + 69);
			DCDC_Info[which].Module_4.Module_Charge_P					= GetU32(data + 71);
			DCDC_Info[which].Module_4.Module_In_Temp					= GetU16(data + 75);
			DCDC_Info[which].Module_4.Module_Bat_Temp					= GetU16(data + 77);
			DCDC_Info[which].Module_4.Module_Dev_Status 				= GetU16(data + 79);
			DCDC_Info[which].Module_4.Module_Dev_Fault_Status			= GetU16(data + 81);
			DCDC_Info[which].Module_4.Module_Output_E_Day				= GetU16(data + 83);
			DCDC_Info[which].Module_4.Module_Output_E_Mon				= GetU32(data + 85);
			DCDC_Info[which].Module_4.Module_Output_E_Total 			= GetU32(data + 89);
			DCDC_Info[which].Module_4.Module_CO2_Reduced				= GetU32(data + 93);
			DCDC_Info[which].Module_4.Module_Run_Min_Day				= GetU16(data + 97);
			DCDC_Info[which].Module_4.Module_Run_Hour_Mon				= GetU16(data + 99);
			DCDC_Info[which].Module_4.Module_Run_Hour_Total 			= GetU32(data + 101);
		}		
        else if(System_Info.Config.DCDC_Type==2)//双向DCDC
        {
			DCDC_Info[which].Module_4.Bus_Bar_I							= GetU16(data + 3);  //10110
			DCDC_Info[which].Module_4.Battery_I							= GetU16(data + 5);  //10111
			DCDC_Info[which].Module_4.Module1_T							= GetU16(data + 7);
			DCDC_Info[which].Module_4.Module2_T							= GetU16(data + 9);
			DCDC_Info[which].Module_4.Device_Status						= GetU32(data + 11);
			DCDC_Info[which].Module_4.Fault_Status						= GetU16(data + 13);

			DCDC_Info[which].Module_1.Bus_Bar_V							=
			DCDC_Info[which].Module_2.Bus_Bar_V							=
			DCDC_Info[which].Module_3.Bus_Bar_V 						=
			DCDC_Info[which].Module_4.Bus_Bar_V 						= GetU16(data +  43);
			
			DCDC_Info[which].Module_1.Battery_V							=
			DCDC_Info[which].Module_2.Battery_V							=
			DCDC_Info[which].Module_3.Battery_V 						=
			DCDC_Info[which].Module_4.Battery_V							= GetU16(data + 45);

            DCDC_Info[which].Module_T.Charge_Discharge_Flag             = GetU16(data + 53);
            DCDC_Info[which].Module_T.Total_Active_Power                = GetU32(data + 55);
            DCDC_Info[which].Module_T.Day_Charge_Energy                 = GetU32(data + 59);
            DCDC_Info[which].Module_T.Day_Discharge_Energy              = GetU32(data + 61);
            DCDC_Info[which].Module_T.Total_Charge_Energy               = GetU32(data + 71);
            DCDC_Info[which].Module_T.Total_Discharge_Energy            = GetU32(data + 75);

			
		}
	}
}

void EMS_Process_Data_From_DCDC_By_UART(uint8_t *data,uint32_t len)    //处理DSP发来的数据 ccg 2013.1.14
{
	uint16_t crcresult=embf_get_crc16(data,len-2);
	if(data[len-2]== LOBYTE(crcresult) && data[len-1]== HIBYTE(crcresult))
	{
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
		{
			if(data[0]==EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address)
			{
				System_Info.EMS_DCDC_Com_Count[i]=SIGNAL_FULL;
				DCDC_Com_Err[i]=false;				
				if(data[1] == 0x04)
				{
                    if(data[2]== 0x78)
                    {
    					EMS_Process_Data_From_DCDC_0x04(data,0,i);
    					Search_DCDC_Info_0x04_01[i]=false;
					}
					else if(data[2]==0x7A)
					{
    					EMS_Process_Data_From_DCDC_0x04(data,1,i);
    					Search_DCDC_Info_0x04_02[i]=false;
					}
				}
				
				else if(data[1] == 0x10)
				{
					if((data[1]==0x10) && (data[2]==0x27) && (data[3]==0x47) && (data[4]==0x00) && (data[5]==0x04))
					{
						Send_Data_To_DCDC_0x10_02[i] = false;//说明设置数据已经收到，不用再发
					}
                    if((data[1]==0x10) && (data[2]==0x27) && (data[3]==0x41) && (data[4]==0x00) && (data[5]==0x06))
                    {
                        Send_Data_To_DCDC_0x10_01[i] = false;//说明设置数据已经收到，不用再发
                    }					
				}
			}
		}
	}
}
uint8_t EMS_Perpare_To_Send_Data_To_DCDC(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)                        //每秒向DCDC发送数据或者查询数据
{
	uint8_t  length=0;
	uint16_t l_crc=0;
	static uint8_t i[GENERAL_SERIAL_NUM]={0};

	for(;;)
	{
		if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i[id]].Modubus_Address>=stID &&
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i[id]].Modubus_Address<=endID)
		{
			if(Search_DCDC_Info_0x04_01[i[id]] == true) //模组1、2
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i[id]].Modubus_Address;
				buf[1]=0x04;
				buf[2]=0x27;
				buf[3]=0x41;
				buf[4]=0x00;
				buf[5]=0x3C;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				Search_DCDC_Info_0x04_02[i[id]]=true;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;
			}
			else if(Search_DCDC_Info_0x04_02[i[id]] == true) //模组3、4
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i[id]].Modubus_Address;
				buf[1]=0x04;
				buf[2]=0x27;
				buf[3]=0x7D;
				buf[4]=0x00;
				buf[5]=0x3D;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;
			}
			else if(Send_Data_To_DCDC_0x10_01[i[id]] == true)
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i[id]].Modubus_Address;
				buf[1]=0x10;
				buf[2]=0x27;
				buf[3]=0x41;
				buf[4]=0x00;
				buf[5]=0x06;
				buf[6]=0x0C;
				buf[7]=HIBYTE((showTime.tm_year)+2000);
				buf[8]=LOBYTE((showTime.tm_year)+2000);
				buf[9]=HIBYTE((showTime.tm_mon));
				buf[10]=LOBYTE((showTime.tm_mon));
				buf[11]=HIBYTE((showTime.tm_day));
				buf[12]=LOBYTE((showTime.tm_day));
				buf[13]=HIBYTE((showTime.tm_hour));
				buf[14]=LOBYTE((showTime.tm_hour));
				buf[15]=HIBYTE((showTime.tm_min));
				buf[16]=LOBYTE((showTime.tm_min));
				buf[17]=HIBYTE((showTime.tm_sec));
				buf[18]=LOBYTE((showTime.tm_sec));
				l_crc=embf_get_crc16(buf,19);
				buf[19]=LOBYTE(l_crc);
				buf[20]=HIBYTE(l_crc);
				length=21;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;
			}
			else if(Send_Data_To_DCDC_0x10_02[i[id]] == true)
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i[id]].Modubus_Address;
				buf[1]=0x10;
				buf[2]=0x27;
				buf[3]=0x47;
				buf[4]=0x00;
				buf[5]=0x04;
				buf[6]=0x08;
				buf[7]=HIBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				buf[8]=LOBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				buf[9]=HIBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				buf[10]=LOBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				buf[11]=HIBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				buf[12]=LOBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				buf[13]=HIBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				buf[14]=LOBYTE(DCDC_Ctrl_Data[i[id]].Power_ONOFF);
				l_crc=embf_get_crc16(buf,15);
				buf[15]=LOBYTE(l_crc);
				buf[16]=HIBYTE(l_crc);
				length=17;
				Search_DCDC_Info_0x04_01[i[id]]=true;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;
			}
			else
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i[id]].Modubus_Address;
				buf[1]=0x04;
				buf[2]=0x27;
				buf[3]=0x41;
				buf[4]=0x00;
				buf[5]=0x3C;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				Search_DCDC_Info_0x04_02[i[id]]=true;
				//i[id]++;
				//i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;
			}
			break;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number)
			{
				i[id]=0;
				break;
			}
		}
	}
	return length;
}

