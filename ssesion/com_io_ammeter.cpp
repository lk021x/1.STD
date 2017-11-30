/*
 * com_io_ammeter.cpp
 *
 *  Created on: 2017-2-24
 *      Author: root
 */

#include "includes.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------
//电表MODBUS通信协议解析
//-----------------------------------------------------------------------------------------------------------------------------------------------------

void EMS_Process_Data_From_Ammeter_0x04(uint8_t *data, uint8_t which)
{

    #if 1

	float temp = 1.0;
	uint8_t i = 0, j = 0;

	AMMETER_Info[which].AMMETER4x.UIpoint = GetU16(data + 3);
	i = (AMMETER_Info[which].AMMETER4x.UIpoint >> 8) & 0xff;
	for (j = 0; j < i; j++)
	{
		temp *= 10;
	}
	AMMETER_Info[which].AMMETER4x.Uab = (uint16_t) (temp * ((float) GetU16(data + 13)) / 1000.0);
	AMMETER_Info[which].AMMETER4x.Ubc = (uint16_t) (temp * ((float) GetU16(data + 15)) / 1000.0);
	AMMETER_Info[which].AMMETER4x.Uac = (uint16_t) (temp * ((float) GetU16(data + 17)) / 1000.0);

	temp = 1.0;
	AMMETER_Info[which].AMMETER4x.PSpoint = GetU16(data + 5);
	i = (AMMETER_Info[which].AMMETER4x.PSpoint >> 8) & 0xff;
	for (j = 0; j < i; j++)
	{
		temp *= 10;
	}
	if((AMMETER_Info[which].AMMETER4x.PSpoint) & 0x0008) // 与之前程序兼容，反向处理
	{
		AMMETER_Info[which].AMMETER4x.Pt = (int32_t) (temp * ((float) GetU16(data + 31)) / 1000000.0);  //0.1kw
	}
	else
	{
		AMMETER_Info[which].AMMETER4x.Pt = -(int32_t) (temp * ((float) GetU16(data + 31)) / 1000000.0);  //0.1kw
	}

	if((AMMETER_Info[which].AMMETER4x.PSpoint) & 0x0080)  //与之前程序兼容，反向处理
	{
		AMMETER_Info[which].AMMETER4x.Qt = (int32_t) (temp * ((float) GetU16(data + 39)) / 1000000.0);  //0.1kw
	}
	else
	{
		AMMETER_Info[which].AMMETER4x.Qt = -(int32_t) (temp * ((float) GetU16(data + 39)) / 1000000.0);  //0.1kw
	}


	AMMETER_Info[which].AMMETER4x.PFt = GetU16(data + 47);

	AMMETER_Info[which].AMMETER4x.hz = GetU16(data + 57);

	//printf("Uab %d Ubc %d Uac %d pt %d qt %d pft %d hz %d\n",
	//AMMETER_Info[which].AMMETER4x.Uab,AMMETER_Info[which].AMMETER4x.Ubc,AMMETER_Info[which].AMMETER4x.Uac,
	//AMMETER_Info[which].AMMETER4x.Pt,AMMETER_Info[which].AMMETER4x.Qt,AMMETER_Info[which].AMMETER4x.PFt,AMMETER_Info[which].AMMETER4x.hz);

	//caoxh
	Meter_Info[which].VAB = AMMETER_Info[which].AMMETER4x.Uab;
	Meter_Info[which].VBC = AMMETER_Info[which].AMMETER4x.Ubc;
	Meter_Info[which].VCA = AMMETER_Info[which].AMMETER4x.Uac;

	Meter_Info[which].Total_active_P = AMMETER_Info[which].AMMETER4x.Pt;
	Meter_Info[which].Total_Q        = AMMETER_Info[which].AMMETER4x.Qt;
	Meter_Info[which].Total_P_factor = AMMETER_Info[which].AMMETER4x.PFt;
	Meter_Info[which].Freq           = AMMETER_Info[which].AMMETER4x.hz;

	//printf("power:    %d\n",  Meter_Info[0].Total_active_P  );

	#else

	Meter_Info[which].VAB = GetU16(data + 3);
	Meter_Info[which].VBC = GetU16(data + 5);
	Meter_Info[which].VCA = GetU16(data + 7);

	Meter_Info[which].Total_active_P = -GetU16(data + 9);
	Meter_Info[which].Total_Q        = GetU16(data + 11);
	Meter_Info[which].Total_P_factor = GetU16(data + 13);
	Meter_Info[which].Freq           = GetU16(data + 15);



	#endif

}

void EMS_Process_Data_From_Ammeter_By_UART_For_Modbus(uint8_t *data, uint32_t len)
{
	uint16_t crcresult = embf_get_crc16(data, len - 2);
	if (data[len - 2] == LOBYTE(crcresult) && data[len - 1] == HIBYTE(crcresult) )
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number; i++)
		{
			if((data[0] == EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address)&&(System_Info.Config.Meter_Position_Enable[i]==ENABLE))
			{
				System_Info.EMS_Meter_Com_Count[i] = METER_SIGNAL_FULL;
				//printf("%d yes\n",i);
				Meter_Com_Err[i] = false;
				if (data[1] == 0x04)
				{
					EMS_Process_Data_From_Ammeter_0x04(data, i);
				}
			}
		}
	}
}

uint8_t EMS_Perpare_To_Send_Data_To_Ammeter_For_Modbus(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)
{
	uint8_t length = 0;
	uint16_t l_crc = 0;
	static uint8_t i[GENERAL_SERIAL_NUM] = {0};
	uint8_t times = 0;

	times = 0;
	for(;;)
	{
		if((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i[id]].Modubus_Address>=stID) &&
		   (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i[id]].Modubus_Address<=endID)&&
		   (System_Info.Config.Meter_Position_Enable[i[id]]==ENABLE))
		{
			buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i[id]].Modubus_Address;
			buf[1] = 0x04;
			buf[2] = 0x00;  //0x0023
			buf[3] = 0x23;
			buf[4] = 0x00;  //读28个寄存器
			buf[5] = 0x1C;
			l_crc = embf_get_crc16(buf, 6);
			buf[6] = LOBYTE(l_crc);
			buf[7] = HIBYTE(l_crc);
			length = 8;
			//printf("i %d id %d\n", i[id], id);
			i[id]++;
			i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;
			break;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number)
			{
				i[id]=0;
				//break;
			}

#if 1		//下面代码在查找条件中包含除起始和结束ID以外条件时使用
			times++;
			if(times >= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number+1)
			{
				//printf("i %d id %d times %d\n", i[id], id, times);
				break;
			}
#endif
		}
	}
	return length;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
//电表DL/T645通信协议解析
//-----------------------------------------------------------------------------------------------------------------------------------------------------

S_Meter_Info_T Meter_Info[Meter_NUM];

bool Get_Measure_Info_cmd[Meter_NUM];
bool Get_Power_Direct_cmd[Meter_NUM];
bool Get_Count_Info_P_cmd[Meter_NUM];
bool Get_Count_Info_Q_cmd[Meter_NUM];
bool Get_Set_Info_PT_cmd[Meter_NUM];
bool Get_Set_Info_CT_cmd[Meter_NUM];


uint8_t CCD(uint8_t *data,uint8_t len)
{
	uint8_t ccd = 0;

	for(uint8_t i=0;i<len;i++)
	{
		ccd += data[i];
	}

	return ccd;
}

//需要设置串口参数
void Meter_System_Init(uint8_t i)
{
	Meter_Info[i].Slave_addr[0] = 0x50;
	Meter_Info[i].Slave_addr[1] = 0x01+i;
	Meter_Info[i].Slave_addr[2] = 0x00;
	Meter_Info[i].Slave_addr[3] = 0x00;
	Meter_Info[i].Slave_addr[4] = 0x00;
	Meter_Info[i].Slave_addr[5] = 0x00;

	Get_Set_Info_PT_cmd[i]  = true;
	Get_Set_Info_CT_cmd[i]  = false;
	Get_Power_Direct_cmd[i] = false;
	Get_Measure_Info_cmd[i] = false;
	Get_Count_Info_P_cmd[i] = false;
	Get_Count_Info_Q_cmd[i] = false;
}

void EMS_Process_Data_From_Ammeter_By_UART_For_DLT645(uint8_t *data,uint8_t len)
{
	uint8_t ccd = CCD(data,len-2);
	static uint8_t active_power_direct[Meter_NUM] = {0};
	static uint8_t reactive_power_direct[Meter_NUM] = {0};

	if(ccd == data[len-2])//CCD校验判断
	{
		for(int i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number; i++)
		{
			if(System_Info.Config.Meter_Position_Enable[i]==ENABLE)
			{
				//地址判断
				if((data[1] != Meter_Info[i].Slave_addr[0])||(data[2] != Meter_Info[i].Slave_addr[1])||
					(data[3] != Meter_Info[i].Slave_addr[2])||(data[4] != Meter_Info[i].Slave_addr[3])||
					 (data[5] != Meter_Info[i].Slave_addr[4])||(data[6] != Meter_Info[i].Slave_addr[5]))
				{
					//return;
					continue;
				}

				//帧基本格式判断
				if((data[0] != FRAME_START)||(data[7] != FRAME_START)||(data[len-1] != FRAME_END))
				{
					//return;
					continue;
				}

				//数据标识判断
				if(data[8]&Slave_Send)
				{
					if(!(data[8]&Slave_ack_err))
					{
						//caoxh
						System_Info.EMS_Meter_Com_Count[i] = METER_SIGNAL_FULL;
						Meter_Com_Err[i] = false;
						//----------------------------------------------------------------------------------------------

						uint16_t R_addr = (((uint8_t)(data[11]-0x33))<<8)|((uint8_t)(data[10]-0x33));	//寄存器地址

						if(R_addr == PARA_PT_IDENTIFIER)//PT
						{
							Meter_Info[i].PT = BcdToBin(data[12]-0x33)+BcdToBin(data[13]-0x33)*100+BcdToBin(data[14]-0x33)*10000+BcdToBin(data[15]-0x33)*1000000;//单位为0.0001

							Get_Set_Info_PT_cmd[i] = false;
							Get_Set_Info_CT_cmd[i] = true;
						}
						else if(R_addr == PARA_CT_IDENTIFIER)//CT
						{
							Meter_Info[i].CT = BcdToBin(data[12]-0x33)+BcdToBin(data[13]-0x33)*100+BcdToBin(data[14]-0x33)*10000+BcdToBin(data[15]-0x33)*1000000;//单位为0.0001

							Get_Set_Info_CT_cmd[i]  = false;
							Get_Power_Direct_cmd[i] = true;
						}
						else if(R_addr == PWR_Direct_IDENTIFIER)// PWR DIRECT
						{
							active_power_direct[i]   = data[12]&0x10;
							reactive_power_direct[i] = data[12]&0x20;
							Get_Power_Direct_cmd[i] = false;
							Get_Measure_Info_cmd[i] = true;

						}
						else if(R_addr == MEASURE_IDENTIFIER)//测量量数据
						{
							Meter_Info[i].VA = (BcdToBin(data[12]-0x33)+BcdToBin(data[13]-0x33)*100)*(Meter_Info[i].PT/10000.0)*10;//单位0.1V
							Meter_Info[i].VB = (BcdToBin(data[14]-0x33)+BcdToBin(data[15]-0x33)*100)*(Meter_Info[i].PT/10000.0)*10;//单位0.1V
							Meter_Info[i].VC = (BcdToBin(data[16]-0x33)+BcdToBin(data[17]-0x33)*100)*(Meter_Info[i].PT/10000.0)*10;//单位0.1V

							//caoxh
							Meter_Info[i].VAB = (float)Meter_Info[i].VA *1.732;
							Meter_Info[i].VBC = (float)Meter_Info[i].VB *1.732;
							Meter_Info[i].VCA = (float)Meter_Info[i].VC *1.732;

							Meter_Info[i].IA = (BcdToBin(data[19]-0x33)+BcdToBin(data[20]-0x33)*100)*(Meter_Info[i].CT/10000.0)/10;//单位0.1A
							Meter_Info[i].IB = (BcdToBin(data[21]-0x33)+BcdToBin(data[22]-0x33)*100)*(Meter_Info[i].CT/10000.0)/10;//单位0.1A
							Meter_Info[i].IC = (BcdToBin(data[23]-0x33)+BcdToBin(data[24]-0x33)*100)*(Meter_Info[i].CT/10000.0)/10;//单位0.1A

							if(active_power_direct[i])
							{
								Meter_Info[i].Total_active_P 	= -(BcdToBin(data[28]-0x33)*10000+BcdToBin(data[27]-0x33)*100+BcdToBin(data[26]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
								Meter_Info[i].Active_PA 		= -(BcdToBin(data[31]-0x33)*10000+BcdToBin(data[30]-0x33)*100+BcdToBin(data[29]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
								Meter_Info[i].Active_PB 		= -(BcdToBin(data[34]-0x33)*10000+BcdToBin(data[33]-0x33)*100+BcdToBin(data[32]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
								Meter_Info[i].Active_PC 		= -(BcdToBin(data[37]-0x33)*10000+BcdToBin(data[36]-0x33)*100+BcdToBin(data[35]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
							}
							else
							{
								Meter_Info[i].Total_active_P 	= (BcdToBin(data[28]-0x33)*10000+BcdToBin(data[27]-0x33)*100+BcdToBin(data[26]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
								Meter_Info[i].Active_PA 		= (BcdToBin(data[31]-0x33)*10000+BcdToBin(data[30]-0x33)*100+BcdToBin(data[29]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
								Meter_Info[i].Active_PB 		= (BcdToBin(data[34]-0x33)*10000+BcdToBin(data[33]-0x33)*100+BcdToBin(data[32]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
								Meter_Info[i].Active_PC 		= (BcdToBin(data[37]-0x33)*10000+BcdToBin(data[36]-0x33)*100+BcdToBin(data[35]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kW
							}

							if(reactive_power_direct[i])
							{
								Meter_Info[i].Total_Q 	= -(BcdToBin(data[39]-0x33)+BcdToBin(data[40]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
								Meter_Info[i].QA		= -(BcdToBin(data[41]-0x33)+BcdToBin(data[42]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
								Meter_Info[i].QB		= -(BcdToBin(data[43]-0x33)+BcdToBin(data[44]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
								Meter_Info[i].QC		= -(BcdToBin(data[45]-0x33)+BcdToBin(data[46]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
							}
							else
							{
								Meter_Info[i].Total_Q 	= (BcdToBin(data[39]-0x33)+BcdToBin(data[40]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
								Meter_Info[i].QA		= (BcdToBin(data[41]-0x33)+BcdToBin(data[42]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
								Meter_Info[i].QB		= (BcdToBin(data[43]-0x33)+BcdToBin(data[44]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
								Meter_Info[i].QC		= (BcdToBin(data[45]-0x33)+BcdToBin(data[46]-0x33)*100)*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/10;//单位0.1kVar
							}

							Meter_Info[i].Total_P_factor	= BcdToBin(data[48]-0x33)+BcdToBin(data[49]-0x33)*100;//单位0.001
							Meter_Info[i].A_P_factor		= BcdToBin(data[50]-0x33)+BcdToBin(data[51]-0x33)*100;//单位0.001
							Meter_Info[i].B_P_factor		= BcdToBin(data[52]-0x33)+BcdToBin(data[53]-0x33)*100;//单位0.001
							Meter_Info[i].C_P_factor		= BcdToBin(data[54]-0x33)+BcdToBin(data[55]-0x33)*100;//单位0.001

							Meter_Info[i].Total_apparent_P	= (BcdToBin(data[59]-0x33)*10000+BcdToBin(data[58]-0x33)*100+BcdToBin(data[57]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kVA
							Meter_Info[i].Apparent_PA		= (BcdToBin(data[62]-0x33)*10000+BcdToBin(data[61]-0x33)*100+BcdToBin(data[60]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kVA
							Meter_Info[i].Apparent_PB		= (BcdToBin(data[65]-0x33)*10000+BcdToBin(data[64]-0x33)*100+BcdToBin(data[63]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kVA
							Meter_Info[i].Apparent_PC		= (BcdToBin(data[68]-0x33)*10000+BcdToBin(data[67]-0x33)*100+BcdToBin(data[66]-0x33))*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0)/1000;//单位0.1kVA

							Meter_Info[i].Freq 	= (BcdToBin(data[70]-0x33)+BcdToBin(data[71]-0x33)*100);//单位0.01Hz

							Get_Measure_Info_cmd[i] = false;
							Get_Power_Direct_cmd[i] = true;
							//Get_Count_Info_P_cmd[i] = true;

						}
						else if(R_addr == ACTIVE_P_IDENTIFIER)//正向总有功电能
						{
							Meter_Info[i].Positive_active_Pt = (BcdToBin(data[12]-0x33)+BcdToBin(data[13]-0x33)*100+BcdToBin(data[14]-0x33)*10000+BcdToBin(data[15]-0x33)*1000000)
																							*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0);//单位为0.01
							Get_Count_Info_P_cmd[i] = false;
							Get_Count_Info_Q_cmd[i] = true;
						}
						else if(R_addr == REACTIVE_Q_IDENTIFIER)//正向总无功电能
						{
							Meter_Info[i].Positive_reactive_Qt = (BcdToBin(data[12]-0x33)+BcdToBin(data[13]-0x33)*100+BcdToBin(data[14]-0x33)*10000+BcdToBin(data[15]-0x33)*1000000)
																							*(Meter_Info[i].PT/10000.0)*(Meter_Info[i].CT/10000.0);//单位为0.01
							Get_Count_Info_Q_cmd[i] = false;
							Get_Measure_Info_cmd[i] = true;
						}
					}
				}
			}
		}
	}
}

uint8_t EMS_Perpare_To_Send_Data_To_Ammeter_For_DLT645(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)
{
	static uint8_t i[GENERAL_SERIAL_NUM]={0};
	uint8_t times = 0;

	for(;;)
	{
		if((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i[id]].Modubus_Address>=stID)&&
		   (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i[id]].Modubus_Address<=endID)&&
		   (System_Info.Config.Meter_Position_Enable[i[id]]==ENABLE))
		{
			buf[0]=FRAME_START;
			buf[1]=Meter_Info[i[id]].Slave_addr[0];  //0x50
			buf[2]=EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i[id]].Modubus_Address;
			buf[3]=Meter_Info[i[id]].Slave_addr[2];  //0x00
			buf[4]=Meter_Info[i[id]].Slave_addr[3];  //0x00
			buf[5]=Meter_Info[i[id]].Slave_addr[4];  //0x00
			buf[6]=Meter_Info[i[id]].Slave_addr[5];  //0x00
			buf[7]=FRAME_START;
		    if(Get_Set_Info_PT_cmd[i[id]] == true)
			{
		        buf[8]=0x01;    //控制码 0x01 读数据 0x04写数据 0x0A写设备地址 0x0C更改通讯速率
		        buf[9]=0x02;    //长度
		        buf[10]=0x4A;
		        buf[11]=0xF8;
		        buf[12]=CCD(buf,12);
		        buf[13]=FRAME_END;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;
			}
		    else if(Get_Set_Info_CT_cmd[i[id]] == true)
			{
		        buf[8]=0x01;	//控制码 0x01 读数据 0x04写数据 0x0A写设备地址 0x0C更改通讯速率
		        buf[9]=0x02;	//长度
		        buf[10]=0x4B;
		        buf[11]=0xF8;
		        buf[12]=CCD(buf,12);
		        buf[13]=FRAME_END;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;
			}
		    else if(Get_Power_Direct_cmd[i[id]] == true)
		    {
		        buf[8]=0x01;	//控制码 0x01 读数据 0x04写数据 0x0A写设备地址 0x0C更改通讯速率
		        buf[9]=0x02;	//长度
		        buf[10]=0x53;
		        buf[11]=0xF3;
		        buf[12]=CCD(buf,12);
		        buf[13]=FRAME_END;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;
		    }
		    else if(Get_Measure_Info_cmd[i[id]] == true)
			{
		        buf[8]=0x01;	//控制码 0x01 读数据 0x04写数据 0x0A写设备地址 0x0C更改通讯速率
		        buf[9]=0x02;	//长度
		        buf[10]=0x32;	//MEASURE_IDENTIFIER : LBYTE+33H
		        buf[11]=0xE9;	//MEASURE_IDENTIFIER : HBYTE+33H
		        buf[12]=CCD(buf,12);
		        buf[13]=FRAME_END;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;
			}
		    else if(Get_Count_Info_P_cmd[i[id]] == true)
			{
		        buf[8]=0x01;	//控制码 0x01 读数据 0x04写数据 0x0A写设备地址 0x0C更改通讯速率
		        buf[9]=0x02;	//长度
		        buf[10]=0x43;
		        buf[11]=0xC3;
		        buf[12]=CCD(buf,12);
		        buf[13]=FRAME_END;
				//i[id]++;
				//i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;
			}
		    else if(Get_Count_Info_Q_cmd[i[id]]==true)
			{
		        buf[8]=0x01;	//控制码 0x01 读数据 0x04写数据 0x0A写设备地址 0x0C更改通讯速率
		        buf[9]=0x02;	//长度
		        buf[10]=0x43;
		        buf[11]=0xC4;
		        buf[12]=CCD(buf,12);
		        buf[13]=FRAME_END;
				i[id]++;
				i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;
			}
		    //break;
		    return 14;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number)
			{
				i[id]=0;
				//return 0;
			}

#if 1		//下面代码在查找条件中包含除起始和结束ID以外条件时使用
			times++;
			if(times >= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number+1)
			{
				//printf("i %d id %d times %d\n", i[id], id, times);
				break;
			}
#endif
		}
	}
	return 0;
}

