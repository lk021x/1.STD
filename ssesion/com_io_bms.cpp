/*
 * com_io_bms.cpp
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#include "includes.h"

void BMS_Set_Data_To_EMS_0x06(uint16_t addr, uint8_t *data)                                      	//处理BMS  0x06命令  ccg 2013.5.9
{
	uint16_t INT32_Val = 0;
	INT32_Val = GetU16(data);
	switch (addr)
	{
	case 9999:												//工作状态
		if (BMS_STATUS_RIGHT(INT32_Val))
		{
			BMS_Info[0].BMS_Work_Status = INT32_Val;
		}
		break;
	case 10000:
		BMS_Info[0].BMS_Fault = INT32_Val;
		break;  //电池故障信息
	case 10001:
		BMS_Info[0].BMS_Warn = INT32_Val;
		break;  //电池报警信息
	case 10002:												 //电池组SOC
		if (INT32_Val <= 1000)												 //100.0%
		{
			BMS_Info[0].BMS_SOC = INT32_Val;
		}
		break;
	case 10003:
		BMS_Info[0].BMS_Rest_Input = INT32_Val;
		break;  //电池组可充电量
	case 10004:
		BMS_Info[0].BMS_Rest_Output = INT32_Val;
		break;  //电池组可发电量
	case 10005:
		BMS_Info[0].BMS_Total_V = INT32_Val;
		break;  //电池组总电压
	case 10006:
		BMS_Info[0].BMS_Total_I = INT32_Val;
		break;  //电池组总电流
	case 10007:
		BMS_Info[0].BMS_Single_SOC_Max = INT32_Val;
		break;  //电池单体最高SOC
	case 10008:
		BMS_Info[0].BMS_Single_SOC_Min = INT32_Val;
		break;  //电池单体最低SOC
	case 10009:
		BMS_Info[0].BMS_Single_V_Max = INT32_Val;
		break;  //电池单体最高电压
	case 10010:
		BMS_Info[0].BMS_Single_V_Min = INT32_Val;
		break;  //电池单体最低电压
	case 10011:
		BMS_Info[0].BMS_Input_V_Max = INT32_Val;
		break;  //最高允许充电电压
	case 10012:
		BMS_Info[0].BMS_Input_I_Max = INT32_Val;
		break;  //最高充电允许电流
	case 10013:
		BMS_Info[0].BMS_Battery_Temp = INT32_Val;
		break;  //柜体温度
	default:
		break;
	}
}
uint16_t EMS_Receive_Data_From_BMS_By_CAN(uint8_t *buf, uint32_t size)                           	//接收BMS发来的数据  ccg 2013.1.14
{
	static uint8_t index = 0;
	uint16_t utmp = 0;
	int32_t ret = -1;
	t_embf_canmsgrx rxmsg;

	//ret = drv_read(gt_glp.fdcan2,&rxmsg,sizeof(rxmsg));
	while (ret > 0)
	{
		// 剔除非数据包
		if ((rxmsg.StdId != 0x501u) || (rxmsg.RTR != 0x00u))
		{
			return (uint16_t) 0;
		}

		if (index + rxmsg.DLC <= size)
		{
			// 拷贝数据
			if ((buf + index != NULL) && (rxmsg.Data != NULL) && (rxmsg.DLC != 0))
			{
				memcpy(buf + index, rxmsg.Data, (uint32_t) rxmsg.DLC);
			}
			index += rxmsg.DLC;
		}
		else
		{
			index = 0;
			return (uint16_t) 0;
		}
		if ((buf[1] == 0x10))
		{
			utmp = buf[6] + 9;
		}
		else if ((buf[1] == 0x06))
		{
			utmp = 8;
		}

		if (utmp > 255)
		{
			index = 0;
			return (uint16_t) 0;
		}

		if (index >= utmp)
		{
			index = (uint16_t) 0;
			return utmp;
		}
		//ret = drv_read(gt_glp.fdcan2,&rxmsg,sizeof(rxmsg));
	}

	return 0;
}
uint16_t EMS_Process_Data_From_BMS_By_CAN(uint8_t *pbuf, uint8_t buflen, uint8_t *sbuf) 	//处理BMS返回数据    ccg 2013.5.9
{
	uint16_t crcresult = 0; 	//crc值
	uint16_t modbus_address = (uint16_t) (((uint16_t) pbuf[2] << 8) | pbuf[3]); 	//modbus地址
	uint16_t slen = 0;
	uint8_t i = 0;
	crcresult = embf_get_crc16(pbuf, buflen - 2);

	if (LOBYTE(crcresult) == pbuf[buflen - 2] && HIBYTE(crcresult) == pbuf[buflen - 1])
	{
		//if(normal_or_time_divide_pwr_next==pbuf[0])
		if (1) // normal_or_time_divide_pwr_next
		{
			if (pbuf[1] == 0x06)
			{
				BMS_Set_Data_To_EMS_0x06(modbus_address, pbuf + 4);
				memcpy(sbuf, pbuf, 8);
				slen = 8;
			}
			else if (pbuf[1] == 0x10)
			{
				for (i = 0; i < GetU16(pbuf + 4); i++)
				{
					BMS_Set_Data_To_EMS_0x06(modbus_address + i, pbuf + 7 + i * 2);
				}
				memcpy(sbuf, pbuf, 6);
				crcresult = embf_get_crc16(sbuf, 6);
				sbuf[6] = LOBYTE(crcresult); //ccg 2013.5.28
				sbuf[7] = HIBYTE(crcresult);
				slen = 8;
			}
			else
			{
				slen = 0;
			}
		}
	}
	return slen;
}
void EMS_Send_Data_To_BMS_By_CAN(int32_t CAN, uint8_t *buf, uint32_t size)                    	//使用CAN2回复BMS    ccg 2013.1.14
{
	/*t_embf_canmsgrx txmsg =
	 {
	 .StdId = 0x501,//ccg 2013.3.11  //标准帧ID号
	 .ExtId = 0x00,                  //扩展帧ID号
	 .IDE   = 0x00,                  //标准帧00  扩展帧04
	 .RTR   = 0x00
	 };
	 uint8_t *data = buf;
	 uint8_t i = 0;
	 uint8_t count = 0;
	 count = size >> 3;
	 if((size % 8) == 0)
	 {
	 for(i = 0; i < count; i++)
	 {
	 txmsg.DLC = 8;
	 if((txmsg.Data!=NULL)&&(data + (uint8_t)(i << 3)!=NULL))
	 {
	 memcpy(txmsg.Data, data + (uint8_t)(i << 3), 8);
	 }
	 (void)drv_write(CAN,&txmsg,sizeof(t_embf_canmsgrx));
	 }
	 }
	 else
	 {
	 for(i = 0; i < count; i++)
	 {
	 txmsg.DLC = 8;
	 if((txmsg.Data!=NULL)&&(data + (uint8_t)(i << 3)!=NULL))
	 {
	 memcpy(txmsg.Data, data + (uint8_t)(i << 3), 8);
	 }
	 (void)drv_write(CAN,&txmsg,sizeof(t_embf_canmsgrx));
	 }

	 txmsg.DLC = (uint8_t)(size % 8);
	 if((txmsg.Data!=NULL)&&(data + (size - txmsg.DLC)!=NULL)&&(txmsg.DLC!=0))
	 {
	 memcpy(txmsg.Data, data + (size - txmsg.DLC), (uint32_t)txmsg.DLC);
	 }
	 (void)drv_write(CAN,&txmsg,sizeof(t_embf_canmsgrx));
	 }*/
}

/*******-------------- UART for samsung battery 68ah and 94ah ---------------*********/

void EMS_Process_Data_From_BMS_0x04(uint8_t *data, uint8_t which)
{
	if (System_Info.Config.BMS_Type == BMS_68AH)
	{
		BMS_Info[which].Sys_Voltage = GetU16(data + 5);
		BMS_Info[which].Sys_Current = GetU16(data + 7);
		BMS_Info[which].Sys_SOC = GetU16(data + 9);
		BMS_Info[which].Sys_SOH = GetU16(data + 11);
		BMS_Info[which].Single_V_Min = GetU16(data + 13);
		BMS_Info[which].Single_V_Max = GetU16(data + 15);
		BMS_Info[which].Single_T_Min = (int8_t) data[17];
		BMS_Info[which].Single_T_Max = (int8_t) data[18];
		BMS_Info[which].Room_Temp1 = (int8_t) data[19];
		BMS_Info[which].Room_Temp2 = (int8_t) data[20];

		BMS_Info[which].Rack_Fault_Word = GetU16(data + 21);  // 68ah
		BMS_Info[which].Rack_Alarm_Word = GetU16(data + 23);
		BMS_Info[which].Sys_Alarm_Word = GetU16(data + 25);  // 68ah
		BMS_Info[which].Sys_Mode = GetU16(data + 27);

		BMS_Info[which].Discharge_Limit = GetU16(data + 29);
		BMS_Info[which].Charge_Limit = GetU16(data + 31);

		BMS_Info[which].Num_Of_Rack_In_Service = data[37];
		BMS_Info[which].Num_Of_Rack_In_System = data[38];
		BMS_Info[which].Service_Voltage = GetU16(data + 39);
		BMS_Info[which].Service_SOC = GetU16(data + 41);

		BMS_Info[which].DC_Contactor_P_Status = GetU16(data + 57);

		BMS_Info[which].DC_Contactor_N_Status = GetU16(data + 73);

		BMS_Info[which].Fan_Operating_Status = GetU16(data + 89);

		BMS_Info[which].Total_G_Charge_Elec = GetU16(data + 91);
		BMS_Info[which].Total_M_Charge_Elec = GetU16(data + 93);
		BMS_Info[which].Total_K_Charge_Elec = GetU16(data + 95);
		BMS_Info[which].Total_G_Discharge_Elec = GetU16(data + 97);
		BMS_Info[which].Total_M_Discharge_Elec = GetU16(data + 99);
		BMS_Info[which].Total_K_Discharge_Elec = GetU16(data + 101);
	}
	else if (System_Info.Config.BMS_Type == BMS_94AH)
	{
		BMS_Info[which].Sys_Voltage = GetU16(data + 5);
		BMS_Info[which].Sys_Current = GetU16(data + 7);
		BMS_Info[which].Sys_SOC = GetU16(data + 9);
		BMS_Info[which].Sys_SOH = GetU16(data + 11);
		BMS_Info[which].Sys_Mode = GetU16(data + 13);
		BMS_Info[which].Single_V_Max = GetU16(data + 15);
		BMS_Info[which].Single_V_Min = GetU16(data + 17);
		BMS_Info[which].Single_T_Max = (int16_t) GetU16(data + 19);
		BMS_Info[which].Single_T_Min = (int16_t) GetU16(data + 21);

		BMS_Info[which].Protection4 = GetU16(data + 31);   // 94ah
		BMS_Info[which].Protection3 = GetU16(data + 33);
		BMS_Info[which].Protection2 = GetU16(data + 35);
		BMS_Info[which].Protection1 = GetU16(data + 37);

		BMS_Info[which].Alarm4 = GetU16(data + 39);   // 94ah
		BMS_Info[which].Alarm3 = GetU16(data + 41);
		BMS_Info[which].Alarm2 = GetU16(data + 43);
		BMS_Info[which].Alarm1 = GetU16(data + 45);

		BMS_Info[which].Discharge_Limit = GetU16(data + 47);
		BMS_Info[which].Charge_Limit = GetU16(data + 49);

		BMS_Info[which].Num_Of_Rack_In_Service = data[55];
		BMS_Info[which].Num_Of_Rack_In_System = data[56];

		BMS_Info[which].Service_Voltage = GetU16(data + 57);
		BMS_Info[which].Service_SOC = GetU16(data + 59);

		BMS_Info[which].Room_Temp1 = (int8_t) data[61];
		BMS_Info[which].Room_Temp2 = (int8_t) data[62];

		BMS_Info[which].Sys_Alarm_Status = GetU16(data + 63);
	}
}

void EMS_Process_Data_From_BMS_By_UART(uint8_t *data, uint32_t len)
{
	uint8_t i;
	uint16_t crcresult = embf_get_crc16(data, len - 2);
	if (data[len - 2] == LOBYTE(crcresult) && data[len - 1] == HIBYTE(crcresult) )
	{
		for (i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (data[0] == EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address)
			{
				System_Info.EMS_BMS_CAN_Com_Count[i] = BMS_SIGNAL_FULL;   //信号满格
				BMS_Com_Err[i] = false;
				if (data[1] == 0x04)
				{
					EMS_Process_Data_From_BMS_0x04(data, i);
					BMS_Com_OK[i] = true;
				}
			}
		}
	}
}
uint8_t EMS_Perpare_To_Send_Data_To_BMS(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID)
{
	uint8_t length = 0;
	uint16_t l_crc = 0;
	static uint8_t i[GENERAL_SERIAL_NUM] = {0};

	for(;;)
	{
		if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address>=stID &&
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address<=endID)
		{
			if (System_Info.Config.BMS_Type == BMS_68AH)
			{
				if (BMS_DC_Contactor_on_cmd[i[id]] == true)
				{
					BMS_DC_Contactor_on_cmd[i[id]] = false;
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x06;
					buf[2] = 0x00;
					buf[3] = 0x01;
					buf[4] = 0x00;
					buf[5] = 0x55;   // on
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
				else if (BMS_DC_Contactor_off_cmd[i[id]] == true)
				{
					BMS_DC_Contactor_off_cmd[i[id]] = false;
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x06;
					buf[2] = 0x00;
					buf[3] = 0x01;
					buf[4] = 0x00;
					buf[5] = 0x00;  // off
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
				else if (BMS_All_Fault_Reset_cmd[i[id]] == true)
				{
					BMS_All_Fault_Reset_cmd[i[id]] = false;
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x06;
					buf[2] = 0x00;
					buf[3] = 0x02;
					buf[4] = 0x00;
					buf[5] = 0x55;  // reset
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
				else
				{
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x04;
					buf[2] = 0x00;
					buf[3] = 0x00;
					buf[4] = 0x00;
					buf[5] = 0x32;
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
			}
			else if (System_Info.Config.BMS_Type == BMS_94AH)
			{
				if (BMS_DC_Contactor_on_cmd[i[id]] == true)
				{
					BMS_DC_Contactor_on_cmd[i[id]] = false;
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x06;
					buf[2] = 0x00;
					buf[3] = 0x00;  // on
					buf[4] = 0x00;
					buf[5] = 0x05;
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
				else if (BMS_DC_Contactor_off_cmd[i[id]] == true)
				{
					BMS_DC_Contactor_off_cmd[i[id]] = false;
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x06;
					buf[2] = 0x00;
					buf[3] = 0x00;
					buf[4] = 0x00;  // off
					buf[5] = 0x03;
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
				else if (BMS_All_Fault_Reset_cmd[i[id]] == true)
				{
					BMS_All_Fault_Reset_cmd[i[id]] = false;
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x06;
					buf[2] = 0x00;
					buf[3] = 0x01;  // reset
					buf[4] = 0x00;
					buf[5] = 0x05;
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
				else
				{
					buf[0] = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i[id]].Modubus_Address;
					buf[1] = 0x04;
					buf[2] = 0x00;
					buf[3] = 0x00;
					buf[4] = 0x00;
					buf[5] = 0x1f;
					l_crc = embf_get_crc16(buf, 6);
					buf[6] = LOBYTE(l_crc);
					buf[7] = HIBYTE(l_crc);
					i[id]++;
					i[id] %= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;
					length = 8;
				}
			}
			break;
		}
		else
		{
			if(i[id]++>=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number)
			{
				i[id]=0;
				break;
			}
		}
	}
	return length;
}

