/*
 * com_io_pcs.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#include "includes.h"

void EMS_Process_Data_From_PCS_0x03(uint8_t *data,uint8_t which)                           //处理DSP返回数据   ccg 2012.12.21
{
 	//PCS_Ctrl_Data_Rec[which].Grid_Island							=
	PCS_Info[which].PCS4x.RControl.Grid_Island					    = GetU16(data + 15);

 	//PCS_Ctrl_Data_Rec[which].Grid_Mode_Set						=
	PCS_Info[which].PCS4x.RControl.Grid_Mode_Set					= GetU16(data + 17);

	PCS_Ctrl_Data_Rec[which].Island_Mode_Set						=
	PCS_Info[which].PCS4x.RControl.Island_Mode_Set				    = GetU16(data + 19);

	//PCS_Ctrl_Data_Rec[which].Island_Mode_ONOFF					=
	PCS_Info[which].PCS4x.RControl.Island_Mode_ONOFF			    = GetU16(data + 21);

	PCS_Ctrl_Data_Rec[which].Qvar_Adj_Mode							=
	PCS_Info[which].PCS4x.RControl.Qvar_Adj_Mode					= GetU16(data + 23);

	PCS_Ctrl_Data_Rec[which].Grid_V_Drop_Mode						=
	PCS_Info[which].PCS4x.RControl.Grid_V_Drop_Mode				    = GetU16(data + 25);

	PCS_Ctrl_Data_Rec[which].Const_I_Mode_I							=
	PCS_Info[which].PCS4x.RAdjust.Const_I_Mode_I					= (int16_t)GetU16(data + 35);

	PCS_Ctrl_Data_Rec[which].Const_V_Mode_I							=
	PCS_Info[which].PCS4x.RAdjust.Const_V_Mode_I					= GetU16(data + 37);

	PCS_Ctrl_Data_Rec[which].Const_V_Mode_V							=
	PCS_Info[which].PCS4x.RAdjust.Const_V_Mode_V					= GetU16(data + 39);

	PCS_Ctrl_Data_Rec[which].Const_P_DC_Mode_P						=
	PCS_Info[which].PCS4x.RAdjust.Const_P_DC_Mode_P				    = (int16_t)GetU16(data + 41);

	PCS_Ctrl_Data_Rec[which].Const_P_AC_Mode_P						=
	PCS_Info[which].PCS4x.RAdjust.Const_P_AC_Mode_P				    = (int16_t)GetU16(data + 49);

	PCS_Ctrl_Data_Rec[which].Ind_Inverter_V							=
	PCS_Info[which].PCS4x.RAdjust.Ind_Inverter_V					= GetU16(data + 51);
	PCS_Ctrl_Data_Rec[which].Ind_Inverter_F							=
	PCS_Info[which].PCS4x.RAdjust.Ind_Inverter_F					= GetU16(data + 53);

	if((System_Info.Config.PCS_Type==PCS_Power_SC500)||(System_Info.Config.PCS_Type==PCS_Power_SC500TL))
	{
		PCS_Ctrl_Data_Rec[which].Power_Factor                       =
        PCS_Info[which].PCS4x.RAdjust.Power_Factor                  =  ((int16_t)GetU16(data + 55)); // 功率因数    10076
		PCS_Ctrl_Data_Rec[which].Qvar_Power_Ratio                   =
        PCS_Info[which].PCS4x.RAdjust.Qvar_Power_Ratio              =  ((int16_t)GetU16(data + 57)); // 无功比例    10077
	}
	else
	{
		PCS_Ctrl_Data_Rec[which].Power_Factor                       =
        PCS_Info[which].PCS4x.RAdjust.Power_Factor                  =  (-(int16_t)GetU16(data + 55)); // 功率因数    10076
		PCS_Ctrl_Data_Rec[which].Qvar_Power_Ratio                   =
        PCS_Info[which].PCS4x.RAdjust.Qvar_Power_Ratio              =  (-(int16_t)GetU16(data + 57)); // 无功比例    10077
    }
}
void EMS_Process_Data_From_PCS_0x04(uint8_t *data,uint8_t num,uint8_t which)                           //处理DSP返回数据   ccg 2012.12.21
{
	if(num==0)
	{
		PCS_Info[which].PCS3x.Property.Protocol_Number				= GetU32(data +  3);

		PCS_Info[which].PCS3x.Property.Protocol_Version				= GetU32(data +  7);

		PCS_Info[which].PCS3x.Property.Mechine_Type[0]				= GetU16(data + 47);
		PCS_Info[which].PCS3x.Property.Mechine_Type[1]				= GetU16(data + 49);
		PCS_Info[which].PCS3x.Property.Mechine_Type[2]				= GetU16(data + 51);
		PCS_Info[which].PCS3x.Property.Mechine_Type[3]				= GetU16(data + 53);
		PCS_Info[which].PCS3x.Property.Mechine_Type[4]				= GetU16(data + 55);
		PCS_Info[which].PCS3x.Property.Mechine_Type[5]				= GetU16(data + 57);
		PCS_Info[which].PCS3x.Property.Mechine_Type[6]				= GetU16(data + 59);
		PCS_Info[which].PCS3x.Property.Mechine_Type[7]				= GetU16(data + 61);

		PCS_Info[which].PCS3x.Property.Mechine_Version[0]			= GetU16(data + 63);
		PCS_Info[which].PCS3x.Property.Mechine_Version[1]			= GetU16(data + 65);
		PCS_Info[which].PCS3x.Property.Mechine_Version[2]			= GetU16(data + 67);
		PCS_Info[which].PCS3x.Property.Mechine_Version[3]			= GetU16(data + 69);
		PCS_Info[which].PCS3x.Property.Mechine_Version[4]			= GetU16(data + 71);
		PCS_Info[which].PCS3x.Property.Mechine_Version[5]			= GetU16(data + 73);
		PCS_Info[which].PCS3x.Property.Mechine_Version[6]			= GetU16(data + 75);
		PCS_Info[which].PCS3x.Property.Mechine_Version[7]			= GetU16(data + 77);
		PCS_Info[which].PCS3x.Property.Mechine_Version[8]			= GetU16(data + 79);
		PCS_Info[which].PCS3x.Property.Mechine_Version[9]			= GetU16(data + 81);

		PCS_Info[which].PCS3x.Property.Serial_Number[0]				= GetU16(data + 83);
		PCS_Info[which].PCS3x.Property.Serial_Number[1]				= GetU16(data + 85);
		PCS_Info[which].PCS3x.Property.Serial_Number[2]				= GetU16(data + 87);
		PCS_Info[which].PCS3x.Property.Serial_Number[3]				= GetU16(data + 89);
		PCS_Info[which].PCS3x.Property.Serial_Number[4]				= GetU16(data + 91);
		PCS_Info[which].PCS3x.Property.Serial_Number[5]				= GetU16(data + 93);
		PCS_Info[which].PCS3x.Property.Serial_Number[6]				= GetU16(data + 95);
		PCS_Info[which].PCS3x.Property.Serial_Number[7]				= GetU16(data + 97);
		PCS_Info[which].PCS3x.Property.Serial_Number[8]				= GetU16(data + 99);
		PCS_Info[which].PCS3x.Property.Serial_Number[9]				= GetU16(data +101);
	}
	else if(num==1)
	{
		PCS_Info[which].PCS3x.Rmeasure.Type_Code					= GetU16(data +  3);
		PCS_Info[which].PCS3x.Rmeasure.Rated_Output_Power			= GetU16(data +  5);
		PCS_Info[which].PCS3x.Rmeasure.Output_Type					= GetU16(data +  7);
		PCS_Info[which].PCS3x.Rmeasure.Charge_Status				= GetU16(data +  9);//0-其它；1-充电；2-放电。
		PCS_Info[which].PCS3x.Rmeasure.Efficiency					= GetU16(data + 11);
		PCS_Info[which].PCS3x.Rmeasure.DC_Voltage					= GetU16(data + 13);
		PCS_Info[which].PCS3x.Rmeasure.DC_Current					= GetU16(data + 15);
		PCS_Info[which].PCS3x.Rmeasure.u32_DC_Power					= GetU32(data + 17);
		PCS_Info[which].PCS3x.Rmeasure.VAB							= GetU16(data + 21);
		PCS_Info[which].PCS3x.Rmeasure.VBC							= GetU16(data + 23);
		PCS_Info[which].PCS3x.Rmeasure.VCA							= GetU16(data + 25);
		PCS_Info[which].PCS3x.Rmeasure.IA							= GetU16(data + 27);
		PCS_Info[which].PCS3x.Rmeasure.IB							= GetU16(data + 29);
		PCS_Info[which].PCS3x.Rmeasure.IC							= GetU16(data + 31);
		PCS_Info[which].PCS3x.Rmeasure.Apparent_Power				= GetU32(data + 33);
		PCS_Info[which].PCS3x.Rmeasure.u32_Active_Power				= GetU32(data + 37);
		PCS_Info[which].PCS3x.Rmeasure.s32_Reactive_Power			= GetU32(data + 41);
		PCS_Info[which].PCS3x.Rmeasure.Power_Factor					= GetU16(data + 45);
		PCS_Info[which].PCS3x.Rmeasure.Grid_Frequency				= GetU16(data + 47);
		PCS_Info[which].PCS3x.Rmeasure.Inside_Temper				= GetU16(data + 49);
		PCS_Info[which].PCS3x.Rmeasure.ModuleA1_Temper				= GetU16(data + 51);
		PCS_Info[which].PCS3x.Rmeasure.ModuleB1_Temper				= GetU16(data + 53);
		PCS_Info[which].PCS3x.Rmeasure.ModuleC1_Temper				= GetU16(data + 55);
		PCS_Info[which].PCS3x.Rmeasure.ModuleA2_Temper				= GetU16(data + 57);
		PCS_Info[which].PCS3x.Rmeasure.ModuleB2_Temper				= GetU16(data + 59);
		PCS_Info[which].PCS3x.Rmeasure.ModuleC2_Temper				= GetU16(data + 61);
		PCS_Info[which].PCS3x.Rmeasure.Total_Run_Time				= GetU32(data + 63);
		PCS_Info[which].PCS3x.Rmeasure.Day_Charge_Energe			= GetU16(data + 67);
		PCS_Info[which].PCS3x.Rmeasure.Day_Discharge_Energe			= GetU16(data + 69);
		PCS_Info[which].PCS3x.Rmeasure.Day_Charge_Time				= GetU16(data + 71);
		PCS_Info[which].PCS3x.Rmeasure.Day_Discharge_Time			= GetU16(data + 73);
		PCS_Info[which].PCS3x.Rmeasure.Month_Charge_Energe			= GetU32(data + 75);
		PCS_Info[which].PCS3x.Rmeasure.Month_Discharge_Energe		= GetU32(data + 79);
		PCS_Info[which].PCS3x.Rmeasure.Month_Charge_Time			= GetU32(data + 83);
		PCS_Info[which].PCS3x.Rmeasure.Month_Discharge_Time			= GetU32(data + 87);
		PCS_Info[which].PCS3x.Rmeasure.Year_Charge_Energe			= GetU32(data + 91);
		PCS_Info[which].PCS3x.Rmeasure.Year_Discharge_Energe		= GetU32(data + 95);
		PCS_Info[which].PCS3x.Rmeasure.Year_Charge_Time				= GetU32(data + 99);
		PCS_Info[which].PCS3x.Rmeasure.Year_Discharge_Time			= GetU32(data +103);
		PCS_Info[which].PCS3x.Rmeasure.Total_Charge_Energe			= GetU32(data +107);
		PCS_Info[which].PCS3x.Rmeasure.Total_Discharge_Energe		= GetU32(data +111);
		PCS_Info[which].PCS3x.Rmeasure.Total_Charge_Time			= GetU32(data +115);
		PCS_Info[which].PCS3x.Rmeasure.Total_Discharge_Time			= GetU32(data +119);

		PCS_Info[which].PCS3x.Rmeasure.Const_I_Mode_I_Uplimit		= (int16_t)GetU16(data +123);
		PCS_Info[which].PCS3x.Rmeasure.Const_I_Mode_I_Lowlimit		= (int16_t)GetU16(data +125);
		PCS_Info[which].PCS3x.Rmeasure.Const_V_Mode_V_Uplimit		= GetU16(data +127);
		PCS_Info[which].PCS3x.Rmeasure.Const_V_Mode_V_Lowlimit		= GetU16(data +129);
		PCS_Info[which].PCS3x.Rmeasure.Const_V_Mode_I_Uplimit		= GetU16(data +131);
		PCS_Info[which].PCS3x.Rmeasure.Const_V_Mode_I_Lowlimit		= GetU16(data +133);
		PCS_Info[which].PCS3x.Rmeasure.Const_P_DC_Mode_P_Uplimit	= (int16_t)GetU16(data +135);
		PCS_Info[which].PCS3x.Rmeasure.Const_P_DC_Mode_P_Lowlimit	= (int16_t)GetU16(data +137);

		PCS_Info[which].PCS3x.Rmeasure.Const_P_AC_Mode_P_Uplimit	= GetU16(data +151);
		PCS_Info[which].PCS3x.Rmeasure.Const_P_AC_Mode_P_Lowlimit	= GetU16(data +153);

		PCS_Info[which].PCS3x.Rmeasure.Ind_Inverter_F_Uplimit		= GetU16(data +155);
		PCS_Info[which].PCS3x.Rmeasure.Ind_Inverter_F_Lowlimit		= GetU16(data +157);
		PCS_Info[which].PCS3x.Rmeasure.Ind_Inverter_V_Uplimit		= GetU16(data +159);
		PCS_Info[which].PCS3x.Rmeasure.Ind_Inverter_V_Lowlimit		= GetU16(data +161);

		PCS_Info[which].PCS3x.Rmeasure.Powerfactor_Uplimit			= GetU16(data +163);
		PCS_Info[which].PCS3x.Rmeasure.Powerfactor_Lowlimit			= GetU16(data +165);
		PCS_Info[which].PCS3x.Rmeasure.Reactive_Power_Uplimit		= GetU16(data +167);
		PCS_Info[which].PCS3x.Rmeasure.Reactive_Power_Lowlimit		= GetU16(data +169);

		PCS_Info[which].PCS3x.Rmeasure.Charge_Times					= GetU16(data +187);
		PCS_Info[which].PCS3x.Rmeasure.Discharge_Times				= GetU16(data +189);

		PCS_Info[which].PCS3x.Rmeasure.Island_Mode					= GetU16(data +191);
		PCS_Info[which].PCS3x.Rmeasure.Ractive_Mode					= GetU16(data +193);
		PCS_Info[which].PCS3x.Rmeasure.Master_Slave					= GetU16(data +195);


		PCS_Info[which].PCS3x.Rmeasure.DC_Power=(uint16_t)(PCS_Info[which].PCS3x.Rmeasure.u32_DC_Power/100);
		PCS_Info[which].PCS3x.Rmeasure.Active_Power=(int16_t)(PCS_Info[which].PCS3x.Rmeasure.u32_Active_Power/100)*
							(PCS_Info[which].PCS3x.Rmeasure.Charge_Status==1?(-1):(PCS_Info[which].PCS3x.Rmeasure.Charge_Status==2?1:0));
		PCS_Info[which].PCS3x.Rmeasure.Reactive_Power=PCS_Info[which].PCS3x.Rmeasure.s32_Reactive_Power/100;
	}
	else if(num==2)
	{
		PCS_Info[which].PCS3x.RCommunication.Run_Status				= GetU32(data +  3);
		PCS_Info[which].PCS3x.RCommunication.Fault_Status1			= GetU32(data +  7);
		PCS_Info[which].PCS3x.RCommunication.Fault_Status2			= GetU32(data + 11);
	}
}
int EMS_Process_Data_From_PCS_By_UART(uint8_t *data,uint32_t len)    //处理DSP发来的数据 ccg 2013.1.14
{
	uint16_t crcresult=embf_get_crc16(data,len-2);
	if(data[len-2]== LOBYTE(crcresult) && data[len-1]== HIBYTE(crcresult))
	{
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
		{
			if(data[0]==EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address)
			{
				System_Info.EMS_PCS_Com_Count[i]=SIGNAL_FULL;
				PCS_Com_Err[i]=false;


				if(data[1] == 0x03)
				{
					EMS_Process_Data_From_PCS_0x03(data,i);
					Search_PCS_Info_0x03[i]=false;
				}
				else if(data[1] == 0x04)
				{
					if(data[2]==0x64)
					{
						EMS_Process_Data_From_PCS_0x04(data,0,i);
						Search_PCS_Info_0x04_01[i]=false;

					}
					else if(data[2]==0xC8)
					{
						EMS_Process_Data_From_PCS_0x04(data,1,i);
						Search_PCS_Info_0x04_02[i]=false;
						PCS_Com_OK[i]=true;   // 上电报警

						//printf("04_02 %d\n", PCS_Info[i].PCS3x.Rmeasure.Active_Power);

					}
					else if(data[2]==0x0C)
					{
						EMS_Process_Data_From_PCS_0x04(data,2,i);
						Search_PCS_Info_0x04_03[i]=false;
						//printf("04_03\n");
					}
				}
				else if(data[1] == 0x10)
				{
					if((data[1]==0x10) && (data[2]==0x27) && (data[3]==0x47) && (data[4]==0x00) && (data[5]==0x16))
					{
						Send_Data_To_PCS_0x10_02[i] = false;//说明设置数据已经收到，不用再发
						//printf("10_02\n");
					}
					if((data[1]==0x10) && (data[2]==0x27) && (data[3]==0x41) && (data[4]==0x00) && (data[5]==0x06))
					{
						Send_Data_To_PCS_0x10_01[i] = false;//说明设置数据已经收到，不用再发
					}
				}
			}
		}
	}else{
		return -1;
	}
	return 0;
}

void EMS_Process_Data_From_PCS_By_Net(uint8_t *data,uint32_t len)    //处理DSP发来的数据
{
    uint16_t Modbus_type    = (uint16_t)(((uint16_t)data[2] << 8) | data[3]);
    uint16_t data_amount    = (uint16_t)(((uint16_t)data[4] << 8) | data[5]);
	if(Modbus_type==0)
	{
		if(data_amount==(len-6))
		{
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				if(data[6]==EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address)
				{
					System_Info.EMS_PCS_Com_Count[i]=SIGNAL_FULL;
					PCS_Com_Err[i]=false;


					if(data[7] == 0x03)
					{
						EMS_Process_Data_From_PCS_0x03(data+6,i);
						Search_PCS_Info_0x03[i]=false;
					}
					else if(data[7] == 0x04)
					{
						if(data[8]==0x64)
						{
							EMS_Process_Data_From_PCS_0x04(data+6,0,i);
							Search_PCS_Info_0x04_01[i]=false;

						}
						else if(data[8]==0xC8)
						{
							EMS_Process_Data_From_PCS_0x04(data+6,1,i);
							Search_PCS_Info_0x04_02[i]=false;
							PCS_Com_OK[i]=true;     //上电报警

						}
						else if(data[8]==0x0C)
						{
							EMS_Process_Data_From_PCS_0x04(data+6,2,i);
							Search_PCS_Info_0x04_03[i]=false;

						}
					}
					else if(data[7] == 0x10)
					{
						if((data[7]==0x10) && (data[8]==0x27) && (data[9]==0x47) && (data[10]==0x00) && (data[11]==0x16))
						{
							Send_Data_To_PCS_0x10_02[i] = false;//说明设置数据已经收到，不用再发
						}
						if((data[7]==0x10) && (data[8]==0x27) && (data[9]==0x41) && (data[10]==0x00) && (data[11]==0x06))
						{
							Send_Data_To_PCS_0x10_01[i] = false;//说明设置数据已经收到，不用再发
						}
					}
				}
			}
		}
	}
}

//注意：设备的最大台数与起始结束ID没有关系，起始结束ID与设备ID有对应关系，例如： 当  2 =< id <= 4, 表明理论3台设备，对应ID:2,3,4，这个ID与默认ID不同需要手动改
uint8_t EMS_Perpare_To_Send_Data_To_PCS_By_UART(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)  //每秒向PCS发送数据或者查询数据
{
	uint8_t  length=0;
	uint16_t l_crc=0;
	static uint8_t i[GENERAL_SERIAL_NUM]={0};

	for(;;)
	{
		if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address>=stID &&
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address<=endID)
		{
			if(Search_PCS_Info_0x04_01[i[id]] == true)
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address;
				buf[1]=0x04;
				buf[2]=0x27;
				buf[3]=0x0F;
				buf[4]=0x00;
				buf[5]=0x32;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;
			}
			else if(Send_Data_To_PCS_0x10_01[i[id]] == true)
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address;
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
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;
			}
			else if(Search_PCS_Info_0x04_02[i[id]] == true )
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address;
				buf[1]=0x04;
				buf[2]=0x27;
				buf[3]=0x41;
				buf[4]=0x00;
				buf[5]=0x64;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				//Search_PCS_Info_0x04_03[i[id]]=true;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;
			}
			else if(Search_PCS_Info_0x04_03[i[id]] == true)
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address;
				buf[1]=0x04;
				buf[2]=0x27;
				buf[3]=0xD7;
				buf[4]=0x00;
				buf[5]=0x06;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				//Search_PCS_Info_0x04_03[i[id]]=true;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;
			}
			else if(Search_PCS_Info_0x03[i[id]] == true)
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address;
				buf[1]=0x03;
				buf[2]=0x27;
				buf[3]=0x41;
				buf[4]=0x00;
				buf[5]=0x1C;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;
			}
			else if(Send_Data_To_PCS_0x10_02[i[id]] == true)
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address;
				buf[1]=0x10;
				buf[2]=0x27;
				buf[3]=0x47;   //地址10050
				buf[4]=0x00;
				buf[5]=0x16;
				buf[6]=0x2C;
				buf[7]=HIBYTE(PCS_Ctrl_Data[i[id]].Grid_Island);
				buf[8]=LOBYTE(PCS_Ctrl_Data[i[id]].Grid_Island);
				buf[9]=HIBYTE(PCS_Ctrl_Data[i[id]].Grid_Mode_Set);
				buf[10]=LOBYTE(PCS_Ctrl_Data[i[id]].Grid_Mode_Set);
				buf[11]=HIBYTE(PCS_Ctrl_Data[i[id]].Island_Mode_Set);
				buf[12]=LOBYTE(PCS_Ctrl_Data[i[id]].Island_Mode_Set);
				buf[13]=HIBYTE(PCS_Ctrl_Data[i[id]].Island_Mode_ONOFF);
				buf[14]=LOBYTE(PCS_Ctrl_Data[i[id]].Island_Mode_ONOFF);
				buf[15]=HIBYTE(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);
				buf[16]=LOBYTE(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);
				buf[17]=HIBYTE(PCS_Info[i[id]].PCS4x.RControl.Grid_V_Drop_Mode);
				buf[18]=LOBYTE(PCS_Info[i[id]].PCS4x.RControl.Grid_V_Drop_Mode);
				buf[19]=0;
				buf[20]=0;
				buf[21]=0;
				buf[22]=0;
				buf[23]=0;
				buf[24]=0;
				buf[25]=0;
				buf[26]=0;
				buf[27]=HIBYTE(PCS_Ctrl_Data[i[id]].Const_I_Mode_I);			//恒流模式电流
				buf[28]=LOBYTE(PCS_Ctrl_Data[i[id]].Const_I_Mode_I);
				buf[29]=HIBYTE(PCS_Ctrl_Data[i[id]].Const_V_Mode_I);   		    //恒压模式电流
				buf[30]=LOBYTE(PCS_Ctrl_Data[i[id]].Const_V_Mode_I);
				buf[31]=HIBYTE(PCS_Ctrl_Data[i[id]].Const_V_Mode_V);			//恒压模式电压
				buf[32]=LOBYTE(PCS_Ctrl_Data[i[id]].Const_V_Mode_V);
				buf[33]=HIBYTE(PCS_Ctrl_Data[i[id]].Const_P_DC_Mode_P); 		//恒功率模式功率DC
				buf[34]=LOBYTE(PCS_Ctrl_Data[i[id]].Const_P_DC_Mode_P);
				buf[35]=0;
				buf[36]=0;
				buf[37]=0;
				buf[38]=0;
				buf[39]=0;
				buf[40]=0;
				buf[41]=HIBYTE(PCS_Ctrl_Data[i[id]].Const_P_AC_Mode_P);  	//恒功率模式功率AC
				buf[42]=LOBYTE(PCS_Ctrl_Data[i[id]].Const_P_AC_Mode_P);
				buf[43]=HIBYTE(PCS_Ctrl_Data[i[id]].Ind_Inverter_V);       	//独立逆变电压
				buf[44]=LOBYTE(PCS_Ctrl_Data[i[id]].Ind_Inverter_V);
				buf[45]=HIBYTE(PCS_Ctrl_Data[i[id]].Ind_Inverter_F);    		//独立逆变频率
				buf[46]=LOBYTE(PCS_Ctrl_Data[i[id]].Ind_Inverter_F);

		        if((System_Info.Config.PCS_Type==PCS_Power_SC500)||(System_Info.Config.PCS_Type==PCS_Power_SC500TL))
		        {
		            buf[47]=HIBYTE(PCS_Ctrl_Data[i[id]].Power_Factor);         //功率因数
		            buf[48]=LOBYTE(PCS_Ctrl_Data[i[id]].Power_Factor);
		            buf[49]=HIBYTE(PCS_Ctrl_Data[i[id]].Qvar_Power_Ratio);     //无功比例
		            buf[50]=LOBYTE(PCS_Ctrl_Data[i[id]].Qvar_Power_Ratio);
		        }
		        else
		        {
		            buf[47]=HIBYTE(-PCS_Ctrl_Data[i[id]].Power_Factor);         //功率因数
		            buf[48]=LOBYTE(-PCS_Ctrl_Data[i[id]].Power_Factor);
		            buf[49]=HIBYTE(-PCS_Ctrl_Data[i[id]].Qvar_Power_Ratio);     //无功比例
		            buf[50]=LOBYTE(-PCS_Ctrl_Data[i[id]].Qvar_Power_Ratio);
		        }

				l_crc=embf_get_crc16(buf,51);
				buf[51]=LOBYTE(l_crc);
				buf[52]=HIBYTE(l_crc);
				length=53;
				Search_PCS_Info_0x03[i[id]]=true;
				Search_PCS_Info_0x04_02[i[id]]=true;
				Search_PCS_Info_0x04_03[i[id]]=true;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;

				//printf("10_02 %d\n",PCS_Ctrl_Data[i[id]].Const_P_AC_Mode_P);

				//printf("10_02 %d\n",PCS_Ctrl_Data[i[id]].Grid_Mode_Set);
			}
			else
			{
				buf[0]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i[id]].Modubus_Address;
				buf[1]=0x04;
				buf[2]=0x27;
				buf[3]=0x41;
				buf[4]=0x00;
				buf[5]=0x64;
				l_crc=embf_get_crc16(buf,6);
				buf[6]=LOBYTE(l_crc);
				buf[7]=HIBYTE(l_crc);
				length=8;
				Search_PCS_Info_0x04_03[i[id]]=true;
				i[id]++;
				i[id]%=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;
			}
			break;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
			{
				i[id]=0;
				break;
			}
		}
	}
	return length;
}

uint8_t EMS_Perpare_To_Send_Data_To_PCS_By_Net(uint8_t *buf, uint16_t id)  //PCS网口查询数据
{
	uint8_t length=0;
	uint16_t i=id;
	static uint16_t TCPClient_CNT[PCS_NUM]={0};

	TCPClient_CNT[i]++; //tcp modbus包号

	if(Search_PCS_Info_0x04_01[i] == true)
	{ //读版本协议号
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x0F;
		buf[10]=0x00;
		buf[11]=0x32;
		length=12;
	}
	else if(Search_PCS_Info_0x04_02[i] == true)
	{ //储能系统信息
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x64;
		length=12;
	}
	else if(Search_PCS_Info_0x04_03[i] == true)
	{ //储能系统状态
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0xD7;
		buf[10]=0x00;
		buf[11]=0x06;
		length=12;
		Search_PCS_Info_0x03[i] = true;
	}
	else if (Search_PCS_Info_0x03[i] == true)
	{ //回读设置值
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x03;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x1C;
		length=12;
	}
	else
	{ //储能系统信息
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x64;
		length=12;
		Search_PCS_Info_0x04_03[i] = true; //交替读系统状态
	}

	buf[0]=HIBYTE(TCPClient_CNT[i]);
	buf[1]=LOBYTE(TCPClient_CNT[i]);
	buf[2]=0;
	buf[3]=0;
	buf[4]=HIBYTE(length-6);
	buf[5]=LOBYTE(length-6);

	return length;
}

uint8_t EMS_Perpare_To_Send_Para_To_PCS_By_Net(uint8_t *buf, uint16_t id)
{
	uint8_t length=0;
	uint16_t i=id;
	static uint16_t TCPClient_CNT[PCS_NUM]={0};

	TCPClient_CNT[i]++; //tcp modbus包号

	if(Send_Data_To_PCS_0x10_02[i]==true)//设置参数
	{
		//drv_write_light_gpio(GPIO_LIGHT_RUN, 0);  // test

		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x10;
		buf[8]=0x27;
		buf[9]=0x47;   //地址10050
		buf[10]=0x00;
		buf[11]=0x16;
		buf[12]=0x2C;
		buf[13]=HIBYTE(PCS_Ctrl_Data[i].Grid_Island);
		buf[14]=LOBYTE(PCS_Ctrl_Data[i].Grid_Island);
		buf[15]=HIBYTE(PCS_Ctrl_Data[i].Grid_Mode_Set);
		buf[16]=LOBYTE(PCS_Ctrl_Data[i].Grid_Mode_Set);
		buf[17]=HIBYTE(PCS_Ctrl_Data[i].Island_Mode_Set);
		buf[18]=LOBYTE(PCS_Ctrl_Data[i].Island_Mode_Set);
		buf[19]=HIBYTE(PCS_Ctrl_Data[i].Island_Mode_ONOFF);
		buf[20]=LOBYTE(PCS_Ctrl_Data[i].Island_Mode_ONOFF);
		buf[21]=HIBYTE(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);
		buf[22]=LOBYTE(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);
		buf[23]=HIBYTE(PCS_Info[i].PCS4x.RControl.Grid_V_Drop_Mode);
		buf[24]=LOBYTE(PCS_Info[i].PCS4x.RControl.Grid_V_Drop_Mode);
		buf[25]=0;
		buf[26]=0;
		buf[27]=0;
		buf[28]=0;
		buf[29]=0;
		buf[30]=0;
		buf[31]=0;
		buf[32]=0;
		buf[33]=HIBYTE(PCS_Ctrl_Data[i].Const_I_Mode_I);	      //恒流模式电流
		buf[34]=LOBYTE(PCS_Ctrl_Data[i].Const_I_Mode_I);
		buf[35]=HIBYTE(PCS_Ctrl_Data[i].Const_V_Mode_I);   		  //恒压模式电流
		buf[36]=LOBYTE(PCS_Ctrl_Data[i].Const_V_Mode_I);
		buf[37]=HIBYTE(PCS_Ctrl_Data[i].Const_V_Mode_V);		  //恒压模式电压
		buf[38]=LOBYTE(PCS_Ctrl_Data[i].Const_V_Mode_V);
		buf[39]=HIBYTE(PCS_Ctrl_Data[i].Const_P_DC_Mode_P); 	   //恒功率模式功率DC
		buf[40]=LOBYTE(PCS_Ctrl_Data[i].Const_P_DC_Mode_P);
		buf[41]=0;
		buf[42]=0;
		buf[43]=0;
		buf[44]=0;
		buf[45]=0;
		buf[46]=0;
		buf[47]=HIBYTE(PCS_Ctrl_Data[i].Const_P_AC_Mode_P);  	//恒功率模式功率AC
		buf[48]=LOBYTE(PCS_Ctrl_Data[i].Const_P_AC_Mode_P);
		buf[49]=HIBYTE(PCS_Ctrl_Data[i].Ind_Inverter_V);       	//独立逆变电压
		buf[50]=LOBYTE(PCS_Ctrl_Data[i].Ind_Inverter_V);
		buf[51]=HIBYTE(PCS_Ctrl_Data[i].Ind_Inverter_F);    		//独立逆变频率
		buf[52]=LOBYTE(PCS_Ctrl_Data[i].Ind_Inverter_F);

        if((System_Info.Config.PCS_Type==PCS_Power_SC500)||(System_Info.Config.PCS_Type==PCS_Power_SC500TL))
        {
    		buf[53]=HIBYTE(PCS_Ctrl_Data[i].Power_Factor);
    		buf[54]=LOBYTE(PCS_Ctrl_Data[i].Power_Factor);
    		buf[55]=HIBYTE(PCS_Ctrl_Data[i].Qvar_Power_Ratio);
    		buf[56]=LOBYTE(PCS_Ctrl_Data[i].Qvar_Power_Ratio);
        }
        else
        {
    		buf[53]=HIBYTE(-PCS_Ctrl_Data[i].Power_Factor);
    		buf[54]=LOBYTE(-PCS_Ctrl_Data[i].Power_Factor);
    		buf[55]=HIBYTE(-PCS_Ctrl_Data[i].Qvar_Power_Ratio);
    		buf[56]=LOBYTE(-PCS_Ctrl_Data[i].Qvar_Power_Ratio);
		}

		length=57;
		//Search_PCS_Info_0x03[i]=true;
		//Search_PCS_Info_0x04_02[i]=true;
		//Search_PCS_Info_0x04_03[i]=true;
		//buf[0]=HIBYTE(TCPClient_CNT[i]);
		//buf[1]=LOBYTE(TCPClient_CNT[i]);
		//buf[2]=0;
		//buf[3]=0;
		//buf[4]=HIBYTE(length-6);
		//buf[5]=LOBYTE(length-6);

		//return length;

		//printf("10_02 %d\n",PCS_Ctrl_Data[i].Const_P_AC_Mode_P);
	}
	else if(Send_Data_To_PCS_0x10_01[i]==true)//给机器对时
	{
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x10;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x06;
		buf[12]=0x0C;

		buf[13]=HIBYTE((showTime.tm_year)+2000);
		buf[14]=LOBYTE((showTime.tm_year)+2000);
		buf[15]=HIBYTE((showTime.tm_mon));
		buf[16]=LOBYTE((showTime.tm_mon));
		buf[17]=HIBYTE((showTime.tm_day));
		buf[18]=LOBYTE((showTime.tm_day));
		buf[19]=HIBYTE((showTime.tm_hour));
		buf[20]=LOBYTE((showTime.tm_hour));
		buf[21]=HIBYTE((showTime.tm_min));
		buf[22]=LOBYTE((showTime.tm_min));
		buf[23]=HIBYTE((showTime.tm_sec));
		buf[24]=LOBYTE((showTime.tm_sec));
		length=25;
		buf[0]=HIBYTE(TCPClient_CNT[i]);
		buf[1]=LOBYTE(TCPClient_CNT[i]);
		buf[2]=0;
		buf[3]=0;
		buf[4]=HIBYTE(length-6);
		buf[5]=LOBYTE(length-6);
	}
	else if(Search_PCS_Info_0x04_01[i] == true)
	{ //读版本协议号
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x0F;
		buf[10]=0x00;
		buf[11]=0x32;
		length=12;
	}
	else if(Search_PCS_Info_0x04_02[i] == true)
	{ //储能系统信息
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x64;
		length=12;
	}
	else if(Search_PCS_Info_0x04_03[i] == true)
	{ //储能系统状态
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0xD7;
		buf[10]=0x00;
		buf[11]=0x06;
		length=12;
		Search_PCS_Info_0x03[i] = true;
	}
	else if (Search_PCS_Info_0x03[i] == true)
	{ //回读设置值
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x03;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x1C;
		length=12;
	}
	else
	{ //储能系统信息
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x64;
		length=12;
		Search_PCS_Info_0x04_03[i] = true; //交替读系统状态
	}

	buf[0]=HIBYTE(TCPClient_CNT[i]);
	buf[1]=LOBYTE(TCPClient_CNT[i]);
	buf[2]=0;
	buf[3]=0;
	buf[4]=HIBYTE(length-6);
	buf[5]=LOBYTE(length-6);

	return length;
}


uint8_t EMS_Perpare_To_Send_Data_To_PCS_By_Net_Not_FM(uint8_t *buf, uint16_t id)  //PCS网口查询数据
{
	uint8_t length=0;
	uint16_t i=id;
	static uint16_t TCPClient_CNT[PCS_NUM]={0};

	TCPClient_CNT[i]++; //tcp modbus包号

	if(Search_PCS_Info_0x04_01[i] == true)
	{ //读版本协议号
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x0F;
		buf[10]=0x00;
		buf[11]=0x32;
		length=12;
	}
	else if(Search_PCS_Info_0x04_02[i] == true)
	{ //储能系统信息
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x64;
		length=12;
	}
	else if(Search_PCS_Info_0x04_03[i] == true)
	{ //储能系统状态
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0xD7;
		buf[10]=0x00;
		buf[11]=0x06;
		length=12;
		//Search_PCS_Info_0x03[i] = true;
	}
	else if (Search_PCS_Info_0x03[i] == true)
	{ //回读设置值
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x03;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x1C;
		length=12;
	}
	else if(Send_Data_To_PCS_0x10_01[i]==true)//给机器对时
	{
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x10;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x06;
		buf[12]=0x0C;

		buf[13]=HIBYTE((showTime.tm_year)+2000);
		buf[14]=LOBYTE((showTime.tm_year)+2000);
		buf[15]=HIBYTE((showTime.tm_mon));
		buf[16]=LOBYTE((showTime.tm_mon));
		buf[17]=HIBYTE((showTime.tm_day));
		buf[18]=LOBYTE((showTime.tm_day));
		buf[19]=HIBYTE((showTime.tm_hour));
		buf[20]=LOBYTE((showTime.tm_hour));
		buf[21]=HIBYTE((showTime.tm_min));
		buf[22]=LOBYTE((showTime.tm_min));
		buf[23]=HIBYTE((showTime.tm_sec));
		buf[24]=LOBYTE((showTime.tm_sec));
		length=25;
		buf[0]=HIBYTE(TCPClient_CNT[i]);
		buf[1]=LOBYTE(TCPClient_CNT[i]);
		buf[2]=0;
		buf[3]=0;
		buf[4]=HIBYTE(length-6);
		buf[5]=LOBYTE(length-6);

		return length;
	}
	else if(Send_Data_To_PCS_0x10_02[i]==true)//设置参数
	{
		//drv_write_light_gpio(GPIO_LIGHT_RUN, 0);  // test

		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x10;
		buf[8]=0x27;
		buf[9]=0x47;   //地址10050
		buf[10]=0x00;
		buf[11]=0x16;
		buf[12]=0x2C;
		buf[13]=HIBYTE(PCS_Ctrl_Data[i].Grid_Island);
		buf[14]=LOBYTE(PCS_Ctrl_Data[i].Grid_Island);
		buf[15]=HIBYTE(PCS_Ctrl_Data[i].Grid_Mode_Set);
		buf[16]=LOBYTE(PCS_Ctrl_Data[i].Grid_Mode_Set);
		buf[17]=HIBYTE(PCS_Ctrl_Data[i].Island_Mode_Set);
		buf[18]=LOBYTE(PCS_Ctrl_Data[i].Island_Mode_Set);
		buf[19]=HIBYTE(PCS_Ctrl_Data[i].Island_Mode_ONOFF);
		buf[20]=LOBYTE(PCS_Ctrl_Data[i].Island_Mode_ONOFF);
		buf[21]=HIBYTE(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);
		buf[22]=LOBYTE(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);
		buf[23]=HIBYTE(PCS_Info[i].PCS4x.RControl.Grid_V_Drop_Mode);
		buf[24]=LOBYTE(PCS_Info[i].PCS4x.RControl.Grid_V_Drop_Mode);
		buf[25]=0;
		buf[26]=0;
		buf[27]=0;
		buf[28]=0;
		buf[29]=0;
		buf[30]=0;
		buf[31]=0;
		buf[32]=0;
		buf[33]=HIBYTE(PCS_Ctrl_Data[i].Const_I_Mode_I);	      //恒流模式电流
		buf[34]=LOBYTE(PCS_Ctrl_Data[i].Const_I_Mode_I);
		buf[35]=HIBYTE(PCS_Ctrl_Data[i].Const_V_Mode_I);   		  //恒压模式电流
		buf[36]=LOBYTE(PCS_Ctrl_Data[i].Const_V_Mode_I);
		buf[37]=HIBYTE(PCS_Ctrl_Data[i].Const_V_Mode_V);		  //恒压模式电压
		buf[38]=LOBYTE(PCS_Ctrl_Data[i].Const_V_Mode_V);
		buf[39]=HIBYTE(PCS_Ctrl_Data[i].Const_P_DC_Mode_P); 	   //恒功率模式功率DC
		buf[40]=LOBYTE(PCS_Ctrl_Data[i].Const_P_DC_Mode_P);
		buf[41]=0;
		buf[42]=0;
		buf[43]=0;
		buf[44]=0;
		buf[45]=0;
		buf[46]=0;
		buf[47]=HIBYTE(PCS_Ctrl_Data[i].Const_P_AC_Mode_P);  	//恒功率模式功率AC
		buf[48]=LOBYTE(PCS_Ctrl_Data[i].Const_P_AC_Mode_P);
		buf[49]=HIBYTE(PCS_Ctrl_Data[i].Ind_Inverter_V);       	//独立逆变电压
		buf[50]=LOBYTE(PCS_Ctrl_Data[i].Ind_Inverter_V);
		buf[51]=HIBYTE(PCS_Ctrl_Data[i].Ind_Inverter_F);    		//独立逆变频率
		buf[52]=LOBYTE(PCS_Ctrl_Data[i].Ind_Inverter_F);
		buf[53]=HIBYTE(-PCS_Ctrl_Data[i].Power_Factor);
		buf[54]=LOBYTE(-PCS_Ctrl_Data[i].Power_Factor);
		buf[55]=HIBYTE(-PCS_Ctrl_Data[i].Qvar_Power_Ratio);
		buf[56]=LOBYTE(-PCS_Ctrl_Data[i].Qvar_Power_Ratio);

		length=57;
		Search_PCS_Info_0x03[i]=true;
		Search_PCS_Info_0x04_02[i]=true;
		Search_PCS_Info_0x04_03[i]=true;
		buf[0]=HIBYTE(TCPClient_CNT[i]);
		buf[1]=LOBYTE(TCPClient_CNT[i]);
		buf[2]=0;
		buf[3]=0;
		buf[4]=HIBYTE(length-6);
		buf[5]=LOBYTE(length-6);

		//printf("10_02 %d\n",PCS_Ctrl_Data[i].Const_P_AC_Mode_P);

		return length;
	}
	else
	{ //储能系统信息
		buf[6]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address;
		buf[7]=0x04;
		buf[8]=0x27;
		buf[9]=0x41;
		buf[10]=0x00;
		buf[11]=0x64;
		length=12;
		Search_PCS_Info_0x04_03[i] = true; //交替读系统状态
	}

	buf[0]=HIBYTE(TCPClient_CNT[i]);
	buf[1]=LOBYTE(TCPClient_CNT[i]);
	buf[2]=0;
	buf[3]=0;
	buf[4]=HIBYTE(length-6);
	buf[5]=LOBYTE(length-6);

	return length;
}




