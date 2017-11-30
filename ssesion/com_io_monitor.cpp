/*
 * com_to_monitor.cpp
 *
 *  Created on: 2017-1-16
 *      Author: root
 */

#include "includes.h"

void Monitor_Get_Data_From_EMS_0x03(uint16_t addr, uint16_t len, uint8_t *data)                			//处理PC  0x03命令
{
	uint16_t Addr_r = addr;
	uint8_t *l_data = data;

	if(len > 255 || len == 0) return;
	for (uint8_t i = 0; i < len; i++)
	{
		if((i+addr)>=10049&&(i+addr)<10120)  //EMS
		{
			switch(i+addr)
			{
				case 10049:l_data = devices_savedata(l_data, (showTime.tm_year) + 2000);break;                       //系统时钟：年
				case 10050:l_data = devices_savedata(l_data, (showTime.tm_mon));break;                             //系统时钟：月
				case 10051:l_data = devices_savedata(l_data, (showTime.tm_day));break;                             //系统时钟：日
				case 10052:l_data = devices_savedata(l_data, (showTime.tm_hour));break;                            //系统时钟：时
				case 10053:l_data = devices_savedata(l_data, (showTime.tm_min));break;                             //系统时钟：分
				case 10054:l_data = devices_savedata(l_data, (showTime.tm_sec));break;                             //系统时钟：秒

				case 10057:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.EMS_Run_Priority);break;     //电网/微网优先
				case 10058:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.EMS_Start_Type);break; 		//主/被动选择
				case 10059:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);break;//无功调节方式

				case 10061:l_data=devices_savedata(l_data,EMS_RL_Mode);break; //本地远程控制模式
				case 10062:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable);break;   //电池电压控制使能
				case 10063:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable);break;    //SOC控制使能

				case 10073:l_data=devices_savedata(l_data,(SYS_RUNNING==1?1:0));break;								//起动状态
				case 10074:l_data=devices_savedata(l_data,(SYS_RUNNING==0?1:0));break;								//停机状态
				case 10075:l_data=devices_savedata(l_data,(System_Info.Config.EMS_Run_Priority==0?1:0));break;		//电网优先
				case 10076:l_data=devices_savedata(l_data,(System_Info.Config.EMS_Run_Priority==1?1:0));break;      //微网优先
				case 10077:l_data=devices_savedata(l_data,(System_Info.Config.EMS_Start_Type==0?1:0));break;		//主动模式
				case 10078:l_data=devices_savedata(l_data,(System_Info.Config.EMS_Start_Type==1?1:0));break;		//被动模式
				case 10079:l_data=devices_savedata(l_data,(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==0x55?1:0));break;		//关闭无功调节
				case 10080:l_data=devices_savedata(l_data,(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==0xA1?1:0));break;        //功率因数
				case 10081:l_data=devices_savedata(l_data,(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==0xA2?1:0));break;		//无功比例

				case 10083:l_data=devices_savedata(l_data,(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode==GRID_MODE?1:0));break;
				case 10084:l_data=devices_savedata(l_data,(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode==ISLAND_MODE?1:0));break;
				case 10085:l_data=devices_savedata(l_data,(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode==GRID_ISLAND_MODE?1:0));break;
				case 10086:l_data=devices_savedata(l_data,(EMS_RL_Mode==0?1:0));break;   //本地/远程
				case 10087:l_data=devices_savedata(l_data,(EMS_RL_Mode==2?1:0));break;   //本地
				case 10088:l_data=devices_savedata(l_data,(EMS_RL_Mode==1?1:0));break;   //远程

				case 10100:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage);break; 		//微网启动电压
				case 10101:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency);break; 	//微网启动频率
				case 10102:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor);break;     //功率因数
				case 10103:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set);break;    //无功功率

				case 10110:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min);break;    //恒压模式运行电压下限
				case 10111:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max);break;    //恒压模式运行电压上限
				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
		else if((i+addr)>=10120&&(i+addr)<=10219)//分时电价
		{
			uint8_t t_Index = 0;
			t_Index = (i+addr-10120)/10;
			Addr_r = 10120+(i+addr-10120)%10;
			if(t_Index>9)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
				continue;
			}
			switch(Addr_r)
            {
				case 10120:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].Start_time.hour);break;//起始时间(时)
				case 10121:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].Start_time.min);break;//起始时间(分)
				case 10122:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].End_time.hour);break;//结束时间(时)
				case 10123:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].End_time.min);break;//结束时间(分)
				case 10124:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].Run_Mode);break;//用电峰谷状态
				case 10125:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].Power_Ctrl_Mode);break;//电价
				case 10126:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].Run_Power);break;//并网运行功率设定
				case 10127:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[t_Index].Mgrid_Run_Power);break;//离网运行功率设定
				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
		else if((i+addr)>=10300&&(i+addr)<=10499)//保护参数 及通讯参数
		{
			switch (i+addr)
			{
				case 10300:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2);break;    //电池电压上限2级
				case 10301:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1);break;    //电池电压上限1级
				case 10302:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1);break;    //电池电压下限1级
				case 10303:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2);break;    //电池电压下限2级
				case 10304:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover);break;    //电池电压下限1级恢复
				case 10305:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover);break;    //电池电压下限2级恢复
				case 10306:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis);break;    //电池电压下限2级恢复
				case 10307:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay);break;    //电池电压控制延迟时间
				case 10308:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay);break;    //电池电压控制恢复时间

				case 10310:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2);break;          //SOC上限2级
				case 10311:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1);break;          //SOC上限1级
				case 10312:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1);break;         //SOC下限1级
				case 10313:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2);break;         //SOC下限2级
				case 10314:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover);break;    //SOC下限1级恢复
				case 10315:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover);break;    //SOC下限2级恢复
				case 10316:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis);break;    //SOC滞环控制
				case 10317:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay);break;    //SOC控制延迟时间
				case 10318:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay);break;    //SOC控制恢复时间

				//通讯参数
				case 10320:l_data=devices_savedata(l_data,System_Info.Modbus_Address);break;    				//EMS对外设备Modbus地址 ETH2
				case 10321:l_data=devices_savedata(l_data,System_Info.Modbus_Port);break;    					//EMS对外Modbus端口号   ETH2
				case 10322:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[0]);break;               //ETH2 IP地址
				case 10323:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[1]);break;
				case 10324:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[2]);break;
				case 10325:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[3]);break;
				case 10326:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[0]);break;          //ETH2 子网掩码
				case 10327:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[1]);break;
				case 10328:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[2]);break;
				case 10329:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[3]);break;
				case 10330:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[0]);break;        	//ETH2 网关
				case 10331:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[1]);break;
				case 10332:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[2]);break;
				case 10333:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[3]);break;
				case 10334:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.ip[0]);break;               //ETH1 IP地址 对PCS、PV
				case 10335:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.ip[1]);break;
				case 10336:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.ip[2]);break;
				case 10337:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.ip[3]);break;
				case 10338:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.submask[0]);break;          //ETH1 子网掩码
				case 10339:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.submask[1]);break;
				case 10340:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.submask[2]);break;
				case 10341:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.submask[3]);break;
				case 10342:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.gateway[0]);break;        	//ETH1 网关
				case 10343:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.gateway[1]);break;
				case 10344:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.gateway[2]);break;
				case 10345:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net2.gateway[3]);break;

	            case 10370:l_data=devices_savedata(l_data,System_Info.NTP.ip[0]);break;													//NTP服务器地址
	            case 10371:l_data=devices_savedata(l_data,System_Info.NTP.ip[1]);break;
	            case 10372:l_data=devices_savedata(l_data,System_Info.NTP.ip[2]);break;
	            case 10373:l_data=devices_savedata(l_data,System_Info.NTP.ip[3]);break;
	            case 10374:l_data=devices_savedata(l_data,System_Info.NTP.port);break;													//NTP端口

	            case 10380:l_data=devices_savedata(l_data,System_Info.IEC104_Address);break;                                       		//IEC104通讯地址
	            case 10381:l_data=devices_savedata(l_data,System_Info.IEC104_T1);break;                                            		//IEC104超时时间T1
	            case 10382:l_data=devices_savedata(l_data,System_Info.IEC104_T3);break;                                            		//IEC104超时时间T3
	            case 10383:l_data=devices_savedata(l_data,System_Info.IEC104_K);break;                                             		//IEC104参数k
	            case 10384:l_data=devices_savedata(l_data,System_Info.IEC104_W);break;                                             		//IEC104参数w

				case 10400:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number);break;    //PCS总台数
				case 10401:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number);break;     //PV总台数
				case 10402:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number);break;    //BMS总台数
				case 10403:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number);break;   //DCDC总台数
				case 10404:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number);break;  //Meter总台数
				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
		else if((i+addr)>=10500&&(i+addr)<=10599)//PC Modbus地址
		{
			uint8_t index = 0;
			index = i+addr-10500;
			if(index>PCS_NUM)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
			}
			else
			{
				l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[index].Modubus_Address);
			}
		}
		else if((i+addr)>=10600&&(i+addr)<=10699)//PV Modbus地址
		{
			uint8_t index = 0;
			index = i+addr-10600;
			if(index>PV_NUM)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
			}
			else
			{
				l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[index].Modubus_Address);
			}
		}
		else if((i+addr)>=10700&&(i+addr)<=10799)//BMS Modbus地址
		{
			uint8_t index = 0;
			index = i+addr-10700;
			if(index>BMS_NUM)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
			}
			else
			{
				l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[index].Modubus_Address);
			}
		}
		else if((i+addr)>=10800&&(i+addr)<=10899)//DCDC Modbus地址
		{
			uint8_t index = 0;
			index = i+addr-10800;
			if(index>DCDC_NUM)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
			}
			else
			{
				l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[index].Modubus_Address);
			}
		}
		else if((i+addr)>=10900&&(i+addr)<=10999)//电表 Modbus地址
		{
			uint8_t index = 0;
			index = i+addr-10900;
			if(index>Meter_NUM)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
			}
			else
			{
				l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[index].Modubus_Address);
			}
		}
		else if((i+addr)>=11000&&(i+addr)<=11499)//PCS 网口参数
		{
	    	uint8_t index = 0;
	    	index = (i+addr-11000)/5;
	    	Addr_r = 11000+(i+addr-11000)%5;
			if(index>PCS_NUM)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
				continue;
			}
			switch(Addr_r)
			{
				case 11000:l_data=devices_savedata(l_data,System_Info.Server[index].ip[0]);break;//IP地址0
				case 11001:l_data=devices_savedata(l_data,System_Info.Server[index].ip[1]);break;//IP地址1
				case 11002:l_data=devices_savedata(l_data,System_Info.Server[index].ip[2]);break;//IP地址2
				case 11003:l_data=devices_savedata(l_data,System_Info.Server[index].ip[3]);break;//IP地址3
				case 11004:l_data=devices_savedata(l_data,System_Info.Server_Modbus_Port[index]);break;//端口号
				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
		else if((i+addr)>=11500&&(i+addr)<=11999)//PV 网口参数
		{
	    	uint8_t index = 0;
	    	index = (i+addr-11500)/5;
	    	Addr_r = 11500+(i+addr-11500)%5;
			if(index>PV_NUM)
			{
				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
				continue;
			}
			switch(Addr_r)
			{
				case 11000:l_data=devices_savedata(l_data,System_Info.PVServer[index].ip[0]);break;//IP地址0
				case 11001:l_data=devices_savedata(l_data,System_Info.PVServer[index].ip[1]);break;//IP地址1
				case 11002:l_data=devices_savedata(l_data,System_Info.PVServer[index].ip[2]);break;//IP地址2
				case 11003:l_data=devices_savedata(l_data,System_Info.PVServer[index].ip[3]);break;//IP地址3
				case 11004:l_data=devices_savedata(l_data,System_Info.PVServer_Modbus_Port[index]);break;//端口号
				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
		else if((i+addr)>=30000&&(i+addr)<=30100)
		{
			switch (i+addr)
			{
				case 30100:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Protocol_Type);break;  //协议类型
				case 30101:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Bus_Type);break;           //总线类型
				case 30102:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.DC_Bus_Charge_Permit);break;      //直流母线电网充电使能/禁止
				case 30103:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.System_Power_Set);break;          //系统功率设置
				case 30104:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Type);break;                  //PCS型号确定
				case 30105:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Power_Distribution_Enable);break; //配电使能.禁止
				case 30106:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Time_Divide_Function);break;      //使能分时功率控制
				case 30107:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.BMS_Type);break;	                // BMS类型选择
				case 30108:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Comm_Type);break;            //PCS通讯方式选择
				case 30109:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Grid_Run_Mode);break;        //PCS并网工作模式
				case 30110:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.DCDC_Type);break;                //DCDC类型选择
				case 30111:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Meter_Comm_Enable);break;        //电表通讯使能.禁止

				case 30113:l_data=devices_savedata(l_data,(uint16_t)Grid_Connect_Type);break;    	                    //是否并网
				case 30114:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Balancing_Enable);break;	        // SOC均衡使能控制
				case 30115:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Island_Ctrl_Mode);break;     // PCS离网启动策略
				case 30116:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Baudrate.PCS_Baud);break;  //PCS波特率
				case 30117:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Baudrate.PV_Baud);break;   //PV波特率
				case 30118:l_data=devices_savedata(l_data,System_Info.Config.Ammeter_Protocol_Type);break;				//电表协议
				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
		else if((i+addr)>=36000&&(i+addr)<=36100)
		{
			switch (i+addr)
			{
				//IAT
				case 35999:l_data=devices_savedata(l_data,LOWORD(Get_Protocol_ID(IAT_PROTOCOL_NUM)));break;             //通讯协议号
				case 36000:l_data=devices_savedata(l_data,HIWORD(Get_Protocol_ID(IAT_PROTOCOL_NUM)));break;
				case 36001:l_data=devices_savedata(l_data,LOWORD(Get_Protocol_Version(IAT_PROTOCOL_VEB)));break;        //通讯协议版本
				case 36002:l_data=devices_savedata(l_data,HIWORD(Get_Protocol_Version(IAT_PROTOCOL_VEB)));break;

				case 36008:l_data=devices_savedata(l_data,IAT_Info.Set_Info.SN_Enable);break;                           //SN设备序列号使能
				case 36009:case 36010:case 36011:case 36012:case 36013:case 36014:case 36015:case 36016:case 36017:     //SN设备序列号
				case 36018:l_data=devices_savedata(l_data,Caculate_Value16(IAT_Info.Set_Info.SN_Number[(i+addr-36009)*2],IAT_Info.Set_Info.SN_Number[(i+addr-36009)*2+1]));break;
				case 36019:case 36020:                                                                                  //MAC地址
				case 36021:l_data=devices_savedata(l_data,IAT_Info.Set_Info.MAC[i+addr-36019]);break;
				case 36022:l_data=devices_savedata(l_data,IAT_Info.Set_Info.MAC_Lock);break;                       	    //MAC地址锁定
				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
		else
		{
			l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
		}
	}
}
void Monitor_Get_Data_From_EMS_0x04(uint16_t addr,uint16_t len,uint8_t *data)                			//处理PC  0x04命令
{
	uint16_t Addr_r = addr;
	uint8_t *l_data=data;

    if((addr>=25000) && (addr<=25099))     //历史事件
    {
        Touch_Load_History_Event_Record();
    
        for(uint8_t i=0;i<len*2;i++)
        {
            data[i]=Touch_Data.Touch_History_Event[(addr-25000)*2+i];
        }
    }
    else if((addr>=25100) && (addr<=25199))     //历史故障
    {
        Touch_Load_History_Fault_Record();
    
        for(uint8_t i=0;i<len*2;i++)
        {
            l_data[i]=Touch_Data.Touch_History_Fault[(addr-25100)*2+i];
        }
    } 
    else
    { 	
    	for (uint8_t i=0;i<len;++i)
    	{        
    		if((i+addr)>=9999&&(i+addr)<=10223)//总遥测信息
            {
        		switch(i+addr)
        		{
        			//----------------------------------------------------------------------------------------------------------
        			//EMS协议
        			case 9999:l_data=devices_savedata(l_data,LOWORD(Get_Protocol_ID(APP_PROTOCOL_NUM)));break;              //应用协议号
        			case 10000:l_data=devices_savedata(l_data,HIWORD(Get_Protocol_ID(APP_PROTOCOL_NUM)));break;
        			case 10001:l_data=devices_savedata(l_data,LOWORD(Get_Protocol_Version(APP_PROTOCOL_VEB)));break;        //应用协议版本
        			case 10002:l_data=devices_savedata(l_data,HIWORD(Get_Protocol_Version(APP_PROTOCOL_VEB)));break;
        			case 10021:case 10022:case 10023:case 10024:case 10025:case 10026:case 10027:                           //机器型号
        			case 10028:l_data=devices_savedata(l_data,Caculate_Value16(Device_Info[DEVICE_NAME][(i+addr-10021)*2],Device_Info[DEVICE_NAME][(i+addr-10021)*2+1]));break;
        			case 10029:case 10030:case 10031:case 10032:case 10033:case 10034:case 10035:case 10036:case 10037:     //机器版本号
        			case 10038:l_data=devices_savedata(l_data,Caculate_Value16(Device_Info[DEVICE_VERSION][(i+addr-10029)*2],Device_Info[DEVICE_VERSION][(i+addr-10029)*2+1]));break;
        			case 10039:case 10040:case 10041:case 10042:case 10043:case 10044:case 10045:case 10046:case 10047:     //SN
        			case 10048:l_data=devices_savedata(l_data,Caculate_Value16(IAT_Info.Set_Info.SN_Number[(i+addr-10039)*2],IAT_Info.Set_Info.SN_Number[(i+addr-10039)*2+1]));break;
        			case 10049:l_data=devices_savedata(l_data,Device_Type_Code[System_Info.Config.PCS_Type]);break;         //设备类型编码
        			case 10050:l_data=devices_savedata(l_data,System_Info.Config.System_Power_Set);break;                   //额定功率
        			case 10051:l_data=devices_savedata(l_data,(uint16_t)((Load_Power_Status<<1)+System_Info.Config.EMS_Run_Priority));break; //供电状态
        			case 10052:l_data=devices_savedata(l_data,(uint16_t)(Grid_Exist?1:0));break;                            //电网状态
                    case 10053:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max);break;   //微网电压
                    case 10054:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency);break;         //微网频率
                    case 10055:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max);break;      //电池最大电压
                    case 10056:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min);break;      //电池最小电压
                    case 10057:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver);break;     //电池平均电压              
                    case 10058:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Max);break;          //电池最大电压
                    case 10059:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Min);break;          //电池最小电压
                    case 10060:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Aver);break;         //电池平均电压                
                    case 10061:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Run_Time));break;                //总运行时间
                    case 10062:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Run_Time));break;
                    case 10063:l_data=devices_savedata(l_data, (uint16_t)(FAULT ? 5 :(EMGSTOP ? 8 :
                    		Run_Status_Monitor(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status))));break;
                    
        			case 10115:l_data=devices_savedata(l_data,DIN(Grid_Contacter));break;
        			case 10116:l_data=devices_savedata(l_data,DIN(MGrid_Contacter));break;

                    case 10117:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status));break; //系统状态
                    case 10118:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status));break; //
                    case 10119:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode));break; //当前并离网状态
                    case 10120:l_data=devices_savedata(l_data,(uint16_t)((System_Info.Config.EMS_Run_Priority==0)?1:((System_Info.Config.EMS_Run_Priority==1)?2:0)));break;                       //电网优先 微网优先
                    case 10121:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1));break; //故障字1
                    case 10122:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1));break; //
                    case 10123:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2));break; //故障字2
                    case 10124:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2));break; //
                    case 10125:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word));break;  //告警字
                    case 10126:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word));break;  //
                    case 10127:l_data=devices_savedata(l_data,EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.InputPin_Status);break;  //输入节点信息

                    case 10129:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min);break;  //微网电压下限
                    case 10130:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max);break;  //微网电压上限
                    case 10131:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min);break;//微网频率下限
                    case 10132:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max);break;//微网频率上限
                    case 10133:l_data=devices_savedata(l_data,-P_Range[System_Info.Config.PCS_Type].Power_Factor.Max);break;  //功率因数下限
                    case 10134:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Power_Factor.Max);break;   //功率因数上限
                    case 10135:l_data=devices_savedata(l_data,-(int16_t)((float)System_Info.Config.System_Power_Set*0.3));break; //无功功率下限
                    case 10136:l_data=devices_savedata(l_data, (int16_t)((float)System_Info.Config.System_Power_Set*0.3));break; //无功功率上限
         			case 10137:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min);break; //并网恒压模式电压下限
        			case 10138:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max);break; //并网恒压模式电压上限
                    case 10139:l_data=devices_savedata(l_data,-(int16_t)((float)System_Info.Config.System_Power_Set*1.1));break;                //并网运行功率下限
                    case 10140:l_data=devices_savedata(l_data, (int16_t)((float)System_Info.Config.System_Power_Set*1.1));break;                   //并网运行功率上限

        			//PCS
        			case 10143:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num);break;       //储能变流器在线台数
        			case 10144:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Rated_Output_Power));break; //储能变流器可用容量
        			case 10145:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Rated_Output_Power));break; //储能变流器可用容量
        			case 10146:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power));break;   //PCS总有功功率
        			case 10147:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power));break;   //PCS总有功功率
        			case 10148:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power));break; //PCS总无功功率
        			case 10149:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power));break; //PCS总无功功率
        			case 10150:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor);break;    //PCS功率因数
        			case 10151:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy));break; //PCS累计充电
        			case 10152:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy));break; //PCS累计充电
        			case 10153:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy));break; //PCS累计放电
        			case 10154:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy));break; //PCS累计放电
                    case 10155:l_data=devices_savedata(l_data,System_Run_Info.Day_Charge_Energy);break;    //PCS电池日充电量
                    case 10156:l_data=devices_savedata(l_data,System_Run_Info.Day_Discharge_Energy);break; //PCS电池日放电量
        			//PV
                    case 10161:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num);break;        //光伏逆变器在线台数
                    case 10162:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power));break;  //光伏逆变器可用容量
                    case 10163:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power));break;  //光伏逆变器可用容量
        			case 10164:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power));break;   //PV总有功功率
        			case 10165:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power));break;   //PV总有功功率
        			case 10166:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power));break; //PV总无功功率
        			case 10167:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power));break; //PV总无功功率
        			case 10168:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Powerfactor);break;    //PV功率因数
        			case 10169:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy));break; //PV累计发电
        			case 10170:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy));break; //PV累计发电
                    case 10171:l_data=devices_savedata(l_data,System_Run_Info.Day_PV_Energy);break;                      //PV日发电量
                    //BMS
                    case 10180:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.BMS_Online_Num);break;       //BMS在线台数
                    case 10181:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max);break; //最大电压
                    case 10182:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min);break; //最小电压
                    case 10183:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max);break;     //最大SOC
                    case 10184:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min);break;     //最小SOC
                    //DCDC
                    case 10195:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Online_Num);break;      //DCDC在线台数
        			case 10196:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Active_Power));break;   //DCDC总有功功率
        			case 10197:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Active_Power));break;   //DCDC总有功功率
        			case 10198:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Charge_Energy));break;   //DCDC总充
        			case 10199:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Charge_Energy));break;   
        			case 10200:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Discharge_Energy));break;//DCDC总放
        			case 10201:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Discharge_Energy));break;    			
                    case 10202:l_data=devices_savedata(l_data,System_Run_Info.Day_DCDC_Charge_Energy);break;      //DCDC日充
                    case 10203:l_data=devices_savedata(l_data,System_Run_Info.Day_DCDC_Discharge_Energy);break;   //DCDC日放
        			//LOAD
        			case 10210:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power));break;   //负荷总有功功率
        			case 10211:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power));break;   //负荷总有功功率
        			case 10212:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power));break; //负荷总无功功率
        			case 10213:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power));break; //负荷总无功功率
        			case 10214:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Powerfactor);break;    //负荷功率因数
        			case 10215:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Load_Energy));break;            //负荷累计用电
        			case 10216:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Load_Energy));break;            //负荷累计用电

        			default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
        		}
    		}
    		else if((i+addr)>=10224&&(i+addr)<=11999)//储能单台信息
    		{
    			uint8_t Pcs_Index = 0;
    			Pcs_Index = (i+addr-10224)/33;
    			Addr_r = 10224+(i+addr-10224)%33;
    			if(Pcs_Index>PCS_NUM-1)
    			{
    				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
    				continue;
    			}
    			switch(Addr_r)
                {
    				case 10224:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.DC_Voltage);break;//当前显示储能变流器直流电压
    				case 10225:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.DC_Current);break;//当前显示储能变流器直流电流	
    				case 10226:l_data=devices_savedata(l_data,(uint16_t)PCS_Info[Pcs_Index].PCS3x.Rmeasure.DC_Power);break;//当前显示储能变流器直流功率
    				case 10227:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.VAB);break;//当前显示储能变流器VAB
    				case 10228:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.VBC);break;//当前显示储能变流器VBC
    				case 10229:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.VCA);break;//当前显示储能变流器VCA
    				case 10230:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.Grid_Frequency);break;//当前显示储能变流器电网频率
    				case 10231:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.IA);break;//当前显示储能变流器IA
    				case 10232:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.IB);break;//当前显示储能变流器IB
    				case 10233:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.IC);break;//当前显示储能变流器IC
    				case 10234:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.Active_Power);break;//当前显示储能变流器有功功率
    				case 10235:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.Reactive_Power);break;//当前显示储能变流器无功功率
    				case 10236:l_data=devices_savedata(l_data,PCS_Info[Pcs_Index].PCS3x.Rmeasure.Power_Factor);break;//当前显示储能变流器功率因数
    				case 10237:l_data=devices_savedata(l_data,LOWORD(PCS_Info[Pcs_Index].PCS3x.Rmeasure.Total_Charge_Energe));break;//当前显示储能变流器累计充电
    				case 10238:l_data=devices_savedata(l_data,HIWORD(PCS_Info[Pcs_Index].PCS3x.Rmeasure.Total_Charge_Energe));break;//当前显示储能变流器累计充电
    				case 10239:l_data=devices_savedata(l_data,LOWORD(PCS_Info[Pcs_Index].PCS3x.Rmeasure.Total_Discharge_Energe));break;//当前显示储能变流器累计放电
    				case 10240:l_data=devices_savedata(l_data,HIWORD(PCS_Info[Pcs_Index].PCS3x.Rmeasure.Total_Discharge_Energe));break;//当前显示储能变流器累计放电
    				case 10241:l_data=devices_savedata(l_data,LOWORD(PCS_Info[Pcs_Index].PCS3x.RCommunication.Run_Status));break;//当前显示储能变流器工作状态
    				case 10242:l_data=devices_savedata(l_data,HIWORD(PCS_Info[Pcs_Index].PCS3x.RCommunication.Run_Status));break;
    				case 10243:l_data=devices_savedata(l_data,LOWORD(PCS_Info[Pcs_Index].PCS3x.RCommunication.Fault_Status1));break;//当前显示储能变流器故障状态1
    				case 10244:l_data=devices_savedata(l_data,HIWORD(PCS_Info[Pcs_Index].PCS3x.RCommunication.Fault_Status1));break;
    				case 10245:l_data=devices_savedata(l_data,LOWORD(PCS_Info[Pcs_Index].PCS3x.RCommunication.Fault_Status2));break;//当前显示储能变流器故障状态2
    				case 10246:l_data=devices_savedata(l_data,HIWORD(PCS_Info[Pcs_Index].PCS3x.RCommunication.Fault_Status2));break;
    				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
    			}
    		}
    		else if((i+addr)>=12000&&(i+addr)<=13999)//PV单台信息
    		{
    			uint8_t Pv_Index = 0;
    			Pv_Index = (i+addr-12000)/30;
    			Addr_r = 12000+(i+addr-12000)%30;
    			if(Pv_Index > PV_NUM-1)
    			{
    				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
    				continue;
    			}
    			switch(Addr_r)
                {
                    case 12000:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.DC_Voltage1);break;    //当前显示光伏逆变器直流电压1
                    case 12001:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.DC_Current1);break;    //当前显示光伏逆变器直流电流1
                    case 12002:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.DC_Voltage2);break;    //当前显示光伏逆变器直流电压2
                    case 12003:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.DC_Current2);break;    //当前显示光伏逆变器直流电流2
                    case 12004:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.VA);break;             //当前显示光伏逆变器电网电压VA
                    case 12005:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.VB);break;             //当前显示光伏逆变器电网电压VB
                    case 12006:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.VC);break;             //当前显示光伏逆变器电网电压VC
                    case 12007:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.Frequency);break;      //当前显示光伏逆变器电网频率
                    case 12008:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.Total_Active_Power);break; //当前显示光伏逆变器有功功率
                    case 12009:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.Reactive_Power);break;     //当前显示光伏逆变器无功功率
                    case 12010:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.Power_Factor);break;       //当前显示光伏逆变器功率因数
                    case 12011:l_data=devices_savedata(l_data,LOWORD(PV_Info[Pv_Index].PV3x.RunParameter.Total_Generation));break; //当前显示光伏逆变器累计发电
                    case 12012:l_data=devices_savedata(l_data,HIWORD(PV_Info[Pv_Index].PV3x.RunParameter.Total_Generation));break;
                    case 12013:l_data=devices_savedata(l_data,PV_Info[Pv_Index].PV3x.RunParameter.Inverter_Status);break;          //当前显示光伏逆变器工作状态
    				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
                }
    		}
    		else if((i+addr)>=14000&&(i+addr)<=15999)//BMS单台信息
    		{
    			uint8_t Bms_Index = 0;
    			Bms_Index = (i+addr-14000)/40;
    			Addr_r = 14000+(i+addr-14000)%40;
    			if(Bms_Index>BMS_NUM-1)
    			{
    				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
    				continue;
    			}			
    			switch(Addr_r)
                {
    				case 14000:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Sys_Voltage);break;
    				case 14001:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Sys_Current);break;
    				case 14002:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Sys_SOC);break;
    				case 14003:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Sys_SOH);break;
    				case 14004:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Single_V_Min);break;
    				case 14005:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Single_V_Max);break;
    				case 14006:l_data=devices_savedata(l_data,(int16_t)BMS_Info[Bms_Index].Single_T_Min);break;
    				case 14007:l_data=devices_savedata(l_data,(int16_t)BMS_Info[Bms_Index].Single_T_Max);break;
    				case 14008:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Charge_Limit);break;
    				case 14009:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Discharge_Limit);break;
    				case 14010:l_data=devices_savedata(l_data,(uint16_t)BMS_Info[Bms_Index].Num_Of_Rack_In_Service);break; //在线rack
    				case 14011:l_data=devices_savedata(l_data,(uint16_t)BMS_Info[Bms_Index].Num_Of_Rack_In_System);break;  //总rack
    				case 14012:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Rack_Fault_Word);break; //68AH
    				case 14013:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Rack_Alarm_Word);break; //68AH
    				case 14014:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Sys_Alarm_Word);break;  //68AH
    				case 14015:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Protection1);break;     //94AH
    				case 14016:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Protection2);break;     //94AH
    				case 14017:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Protection3);break;     //94AH
    				case 14018:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Protection4);break;     //94AH
    				case 14019:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Alarm1);break;          //94AH
    				case 14020:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Alarm2);break;          //94AH
    				case 14021:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Alarm3);break;          //94AH
    				case 14022:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Alarm4);break;          //94AH
    				case 14023:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Sys_Alarm_Status);break;//94AH
    				case 14024:l_data=devices_savedata(l_data,BMS_Info[Bms_Index].Sys_Mode);break;        //68ah&94ah
    				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;	
                }           
    		}
    		else if((i+addr)>=16000&&(i+addr)<=21999)//DCDC单台信息
    		{
    			uint8_t Dcdc_Index = 0;
    			Dcdc_Index = (i+addr-16000)/120;
    			Addr_r = 16000+(i+addr-16000)%120;
    			if(Dcdc_Index>DCDC_NUM-1)
    			{
    				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
    				continue;
    			}			
                if(System_Info.Config.DCDC_Type==1) //单向
                {
        			switch(Addr_r)
                    {
                        case 16000:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Array_V);break;//阵列电压1
                        case 16001:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Array_I);break;//阵列电流1
                        case 16002:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Bat_V);break;//电池电压1
                        case 16003:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Charge_I);break;//充电电流1
                        case 16004:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Charge_P));break;//充电功率1
                        case 16005:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Charge_P));break;//充电功率1
                        case 16006:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_1.Module_In_Temp);break;//机内温度1
                        case 16007:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Bat_Temp);break;//电池温度1
                        case 16008:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?6:
                                                                             DCDC_Info[Dcdc_Index].Module_1.Module_Dev_Status));break; //设备状态1
                        case 16009:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Dev_Fault_Status);break;//故障状态1
                        case 16010:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Day);break;//日发电量
                        case 16011:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Mon));break;//月发电量
                        case 16012:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Mon));break;//月发电量
                        case 16013:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Total));break;//发电总量
                        case 16014:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Total));break;//发电总量
                        case 16015:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_1.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16016:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_1.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16017:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Run_Min_Day);break;//日运行分钟
                        case 16018:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Module_Run_Hour_Mon);break;//月运行时数
                        case 16019:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Run_Hour_Total));break;//总运行时数
                        case 16020:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_1.Module_Run_Hour_Total));break;//总运行时数
                        
                        case 16030:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Array_V);break;//阵列电压2
                        case 16031:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Array_I);break;//阵列电流2
                        case 16032:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Bat_V);break;//电池电压2
                        case 16033:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Charge_I);break;//充电电流2
                        case 16034:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Charge_P));break;//充电功率2
                        case 16035:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Charge_P));break;//充电功率2
                        case 16036:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_2.Module_In_Temp);break;//机内温度2
                        case 16037:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Bat_Temp);break;//电池温度2
                        case 16038:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?6:
                                                                             DCDC_Info[Dcdc_Index].Module_2.Module_Dev_Status));break; //设备状态2                        
                        case 16039:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Dev_Fault_Status);break;//故障状态2
                        case 16040:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Day);break;//日发电量
                        case 16041:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Mon));break;//月发电量
                        case 16042:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Mon));break;//月发电量
                        case 16043:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Total));break;//发电总量
                        case 16044:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Total));break;//发电总量
                        case 16045:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_2.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16046:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_2.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16047:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Run_Min_Day);break;//日运行分钟
                        case 16048:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Module_Run_Hour_Mon);break;//月运行时数
                        case 16049:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Run_Hour_Total));break;//总运行时数
                        case 16050:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_2.Module_Run_Hour_Total));break;//总运行时数
                        
                        case 16060:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Array_V);break;//阵列电压3
                        case 16061:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Array_I);break;//阵列电流3
                        case 16062:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Bat_V);break;//电池电压3
                        case 16063:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Charge_I);break;//充电电流3
                        case 16064:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Charge_P));break;//充电功率3
                        case 16065:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Charge_P));break;//充电功率3
                        case 16066:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_3.Module_In_Temp);break;//机内温度3
                        case 16067:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Bat_Temp);break;//电池温度3
                        case 16068:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?6:
                                                                             DCDC_Info[Dcdc_Index].Module_3.Module_Dev_Status));break; //设备状态3
                        case 16069:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Dev_Fault_Status);break;//故障状态3
                        case 16070:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Day);break;//日发电量
                        case 16071:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Mon));break;//月发电量
                        case 16072:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Mon));break;//月发电量
                        case 16073:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Total));break;//发电总量
                        case 16074:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Total));break;//发电总量
                        case 16075:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_3.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16076:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_3.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16077:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Run_Min_Day);break;//日运行分钟
                        case 16078:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Module_Run_Hour_Mon);break;//月运行时数
                        case 16079:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Run_Hour_Total));break;//总运行时数
                        case 16080:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_3.Module_Run_Hour_Total));break;//总运行时数

                        case 16090:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Array_V);break;//阵列电压4
                        case 16091:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Array_I);break;//阵列电流4
                        case 16092:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Bat_V);break;//电池电压4
                        case 16093:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Charge_I);break;//充电电流4
                        case 16094:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Charge_P));break;//充电功率4
                        case 16095:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Charge_P));break;//充电功率4
                        case 16096:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_4.Module_In_Temp);break;//机内温度4
                        case 16097:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Bat_Temp);break;//电池温度4
                        case 16098:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?6:
                                                                            DCDC_Info[Dcdc_Index].Module_4.Module_Dev_Status));break; //设备状态4                        
                        case 16099:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Dev_Fault_Status);break;//故障状态4
                        case 16100:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Day);break;//日发电量
                        case 16101:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Mon));break;//月发电量
                        case 16102:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Mon));break;//月发电量
                        case 16103:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Total));break;//发电总量
                        case 16104:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Total));break;//发电总量
                        case 16105:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_4.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16106:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_4.Module_CO2_Reduced));break;//二氧化碳减排
                        case 16107:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Run_Min_Day);break;//日运行分钟
                        case 16108:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Module_Run_Hour_Mon);break;//月运行时数
                        case 16109:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Run_Hour_Total));break;//总运行时数
                        case 16110:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_4.Module_Run_Hour_Total));break;//总运行时数
                        default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
                    }
                }            
                else if (System_Info.Config.DCDC_Type==2) //双向
                {
                    switch(Addr_r)
                    {
                        case 16000:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Bus_Bar_V);break;//母线电压
                        case 16001:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Bus_Bar_I);break;//母线电流
                        case 16002:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Battery_V);break;//蓄电池电压
                        case 16003:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_1.Battery_I);break;//蓄电池电流
                        case 16004:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_1.Module1_T);break;//模组1温度
                        case 16005:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_1.Module2_T);break;//模组2温度
                        case 16006:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?0:
                                                                             DCDC_Info[Dcdc_Index].Module_1.Device_Status));break; //设备状态1
                        case 16007:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Info[Dcdc_Index].Module_1.Fault_Status));break; //故障1

                        case 16030:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Bus_Bar_V);break;//母线电压
                        case 16031:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Bus_Bar_I);break;//母线电流
                        case 16032:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Battery_V);break;//蓄电池电压
                        case 16033:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_2.Battery_I);break;//蓄电池电流
                        case 16034:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_2.Module1_T);break;//模组1温度
                        case 16035:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_2.Module2_T);break;//模组2温度
                        case 16036:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?0:
                                                                             DCDC_Info[Dcdc_Index].Module_2.Device_Status));break; //设备状态2
                        case 16037:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Info[Dcdc_Index].Module_2.Fault_Status));break; //故障2

                        case 16060:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Bus_Bar_V);break;//母线电压
                        case 16061:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Bus_Bar_I);break;//母线电流
                        case 16062:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Battery_V);break;//蓄电池电压
                        case 16063:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_3.Battery_I);break;//蓄电池电流
                        case 16064:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_3.Module1_T);break;//模组1温度
                        case 16065:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_3.Module2_T);break;//模组2温度
                        case 16066:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?0:
                                                                             DCDC_Info[Dcdc_Index].Module_3.Device_Status));break; //设备状态3 
                        case 16067:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Info[Dcdc_Index].Module_3.Fault_Status));break; //故障3

                        case 16090:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Bus_Bar_V);break;//母线电压
                        case 16091:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Bus_Bar_I);break;//母线电流
                        case 16092:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Battery_V);break;//蓄电池电压
                        case 16093:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_4.Battery_I);break;//蓄电池电流
                        case 16094:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_4.Module1_T);break;//模组1温度
                        case 16095:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Dcdc_Index].Module_4.Module2_T);break;//模组2温度
                        case 16096:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Dcdc_Index]?0:
                                                                             DCDC_Info[Dcdc_Index].Module_4.Device_Status));break; //设备状态4            
                        case 16097:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Info[Dcdc_Index].Module_4.Fault_Status));break;  //故障4

                        case 16105:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_T.Charge_Discharge_Flag);break; //充放电标识
                        case 16106:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_T.Total_Active_Power));break;      //当前总运行功率
                        case 16107:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_T.Total_Active_Power));break;
                        case 16108:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_T.Day_Charge_Energy);break;    //日充放电量
                        case 16109:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Dcdc_Index].Module_T.Day_Discharge_Energy);break; //日放电量
                        case 16110:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_T.Total_Charge_Energy));break;     //总充电量
                        case 16111:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_T.Total_Charge_Energy));break;
                        case 16112:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Dcdc_Index].Module_T.Total_Discharge_Energy));break;  //总放电量
                        case 16113:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Dcdc_Index].Module_T.Total_Discharge_Energy));break;
                        default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
                    }
                }            
    		}
    		else if((i+addr)>=22000&&(i+addr)<=23999)//光伏汇流箱单台信息
			{
				uint8_t Pvs_Index = 0;
				Pvs_Index = (i+addr-22000)/30;
				Addr_r = 22000+(i+addr-22000)%30;
				if(Pvs_Index>PVS_NUM-1)
				{
					l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
					continue;
				}
				switch(Addr_r)
				{
					case 22000:l_data=devices_savedata(l_data,PVS_Info[Pvs_Index].DC_Vol);break;
					case 22001:l_data=devices_savedata(l_data,LOWORD(PVS_Info[Pvs_Index].Total_I));break;
					case 22002:l_data=devices_savedata(l_data,HIWORD(PVS_Info[Pvs_Index].Total_I));break;
					case 22003:l_data=devices_savedata(l_data,LOWORD(PVS_Info[Pvs_Index].Total_P_DC));break;
					case 22004:l_data=devices_savedata(l_data,HIWORD(PVS_Info[Pvs_Index].Total_P_DC));break;
					case 22005:l_data=devices_savedata(l_data,LOWORD(PVS_Info[Pvs_Index].Output_Pt_Day));break;
					case 22006:l_data=devices_savedata(l_data,HIWORD(PVS_Info[Pvs_Index].Output_Pt_Day));break;
					case 22007:l_data=devices_savedata(l_data,LOWORD(PVS_Info[Pvs_Index].Output_Pt_Total));break;
					case 22008:l_data=devices_savedata(l_data,HIWORD(PVS_Info[Pvs_Index].Output_Pt_Total));break;
					case 22009:l_data=devices_savedata(l_data,PVS_Info[Pvs_Index].Room_T);break;
					case 22010:l_data=devices_savedata(l_data,LOWORD(PVS_Info[Pvs_Index].Work_Status));break;
					case 22011:l_data=devices_savedata(l_data,HIWORD(PVS_Info[Pvs_Index].Work_Status));break;
					default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
				}
			}
    		else if((i+addr)>=24000&&(i+addr)<=24999)//电表单台信息
    		{
    			uint8_t Ammeter_Index = 0;
    			Ammeter_Index = (i+addr-24000)/30;
    			Addr_r = 24000+(i+addr-24000)%30;
    			if(Ammeter_Index>Meter_NUM-1)
    			{
    				l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
    				continue;
    			}
    			switch(Addr_r)
    			{
					case 24000:l_data[0] = (uint16_t)Meter_Info[Ammeter_Index].VAB;break;
					case 24001:l_data[0] = (uint16_t)Meter_Info[Ammeter_Index].VBC;break;
					case 24002:l_data[0] = (uint16_t)Meter_Info[Ammeter_Index].VCA;break;
					case 24003:l_data[0] = (int16_t)Meter_Info[Ammeter_Index].Total_active_P;break;
					case 24004:l_data[0] = (int16_t)Meter_Info[Ammeter_Index].Total_Q;break;
					case 24005:l_data[0] = (int16_t)Meter_Info[Ammeter_Index].Total_P_factor;break;
    				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
    			}
    		}
    		else
    		{
    			l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;
    		}
    	}
	}
}

void Monitor_Set_Data_To_EMS_0x06(uint16_t addr,uint8_t *data)    										//处理PC  0x06命令
{
	uint16_t Addr_r = addr;
	uint16_t INT32_Val=0;
	static char sn_temp[20] = {0};
	//static uint16_t mac_temp[3] = {0};
	//static uint16_t temp1=0;	
	uint8_t Address_used=0;
	
	INT32_Val=GetU16(data);
	
	if(addr >= 10049 && addr <= 10119)
	{
		switch(addr)
		{
			case 10049: //系统时钟：年
				if(2000<=INT32_Val&&INT32_Val<=2099)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[0]=((uint8_t)(INT32_Val-2000)) ;
				}
			break;
			case 10050: //系统时钟：月
				if(INT32_Val<=12)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[1]=((uint8_t)INT32_Val);
				}
			break;
			case 10051: //系统时钟：日
				if(INT32_Val>=1 || INT32_Val<=31)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[2]=((uint8_t)INT32_Val);
				}
			break;
			case 10052: //系统时钟：时
				if(INT32_Val<=23)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[3]=((uint8_t)INT32_Val);
				}
			break;
			case 10053: //系统时钟：分
				if(INT32_Val<=59)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[4]=((uint8_t)INT32_Val);
				}
			break;
			case 10054: //系统时钟：秒
				if(INT32_Val<=59)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[5]=((uint8_t)INT32_Val);
				}
			break;

			case 10055: //启停
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val==START_COMMAND)
					{
						Sys_Ctrl.Sys_Start=true;
					}
					if(INT32_Val==STOP_COMMAND)
					{
						Sys_Ctrl.Manual_Stop=true;
					}
				}
			break;

			case 10056://BMS总控制
				if(PC_CAN_SET_DATA)
				{
					if(1==INT32_Val)
					{
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
						{
							BMS_DC_Contactor_off_cmd[i]=true;
							BMS_Switch_Control[i]=INT32_Val;
						}
					}
					else if(2==INT32_Val)
					{
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
						{
							BMS_DC_Contactor_on_cmd[i]=true;
							BMS_Switch_Control[i]=INT32_Val;
						}
					}
					else if(3==INT32_Val)
					{
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
						{
							BMS_All_Fault_Reset_cmd[i]=true;
							BMS_Switch_Control[i]=INT32_Val;
						}
					}
				}
			break;

			case 10057: //电网优先，微网优先
				if(PC_CAN_SET_DATA)
				{
					if((INT32_Val==0) || (INT32_Val==1))
					{
						System_Info.Config.EMS_Run_Priority=(uint8_t)INT32_Val;
						write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
					}
				}
			break;

			case 10058: //主被动选择
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val==0 || INT32_Val==1)
					{
						System_Info.Config.EMS_Start_Type=(uint8_t)INT32_Val;
						write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
					}
				}
			break;

			case 10059://无功调节方式
				if(PC_CAN_SET_DATA)
				{
					if((INT32_Val==QP)||(INT32_Val==PF)||(INT32_Val==SHUT_OFF))
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode=(int16_t)INT32_Val;
						write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);

						if(SHUT_OFF==EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
							EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
							write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
							write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
						}
						else if(PF==EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
						{
							 EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
							 write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
						}
						else if(QP==EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
							write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
						}
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
						{
							PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
							PCS_Ctrl_Data[i].Power_Factor=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
							Send_Data_To_PCS_0x10_02[i]=true;
						}
					}
				}
			break;
			case 10060:
			break;

			case 10061: //远程本地模式
				if(INT32_Val==RE_LO_MODE || INT32_Val==REMOTE_MODE || INT32_Val==LOCAL_MODE)
				{
					EMS_RL_Mode=INT32_Val;
					write_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 2);
				}
			break;

			case 10062: //电池电压控制使能
				if(PC_CAN_SET_DATA)
				{
					if((INT32_Val==0xAA) || (INT32_Val==0x55))
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=INT32_Val;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
						if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable==ENABLE)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable=DISABLE;
							write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
						}
						else if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable==DISABLE)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable=ENABLE;
							write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
						}
					}
				}
			break;

			case 10063: //SOC控制使能
				if(PC_CAN_SET_DATA)
				{
				if((INT32_Val==0xAA )||( INT32_Val==0x55))
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable=INT32_Val;
					write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
					if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable==ENABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=DISABLE;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					}
					else if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable==DISABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=ENABLE;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					}
				}
			}
			break;

			case 10073:  //起动
				if(INT32_Val==1)
				{
					Sys_Ctrl.Sys_Start=true;
				}
			break;
			case 10074:  //停止
				if(INT32_Val==1)
				{
					Sys_Ctrl.Manual_Stop=true;
				}
			break;
			case 10075:	//电网优先
				if(INT32_Val==1)
				{
					System_Info.Config.EMS_Run_Priority=0;
					write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
				}
			break;
			case 10076:	//微网优先
				if(INT32_Val==1)
				{
					System_Info.Config.EMS_Run_Priority=1;
					write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
				}
			break;
			case 10077: //主动
				if(INT32_Val==1)
				{
					System_Info.Config.EMS_Start_Type=0;
					write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
				}
			break;
			case 10078: //被动
				if(INT32_Val==1)
				{
					System_Info.Config.EMS_Start_Type=1;
					write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
				}
			break;
			case 10079: //关闭无功调节
				if(INT32_Val==1)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode=SHUT_OFF;
					write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
					EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
					EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
					write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
					write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
						PCS_Ctrl_Data[i].Power_Factor=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
				}
			break;
			case 10080: //无功功率调节方式
				if(INT32_Val==1)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode=PF;
					write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
					EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
					write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
						PCS_Ctrl_Data[i].Power_Factor=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
				}
			break;
			case 10081: //功率因数调节方式
				if(INT32_Val==1)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode=QP;
					write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
					EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
					write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
						PCS_Ctrl_Data[i].Power_Factor=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
				}
			break;
			case 10083:  //并网模式
				if(INT32_Val==1)
				{
					Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode = (uint8_t)GRID_MODE;
					write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+Time_Div_Info.Period*7,&Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode,1);
				}
			break;
			case 10084:  //离网模式
				if(INT32_Val==1)
				{
					Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode = (uint8_t)ISLAND_MODE;
					write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+Time_Div_Info.Period*7,&Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode,1);
				}
			break;
			case 10085:  //并离网切换模式
				if(INT32_Val==1)
				{
					Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode = (uint8_t)GRID_ISLAND_MODE;
					write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+Time_Div_Info.Period*7,&Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode,1);
				}
			break;
			case 10100: //微网启动电压
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage=INT32_Val;
						write_eeprom(MG_VOLTAGE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage, 2);
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
						{
							PCS_Ctrl_Data[i].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
							Send_Data_To_PCS_0x10_02[i]=true;
						}
					}
				}
			break;
			case 10101: //微网启动频率
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency=INT32_Val;
						write_eeprom(MG_FREQUENCY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency, 2);
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
						{
							PCS_Ctrl_Data[i].Ind_Inverter_F=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
							Send_Data_To_PCS_0x10_02[i]=true;
						}
					}
				}
			break;

			case 10102://功率因数
				if(PC_CAN_SET_DATA)
				{
					if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==PF)
					{
						if(((abs((int16_t)INT32_Val))>=P_Range[System_Info.Config.PCS_Type].Power_Factor.Min) && ((abs((int16_t)INT32_Val))<=P_Range[System_Info.Config.PCS_Type].Power_Factor.Max))
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=(int16_t)INT32_Val;
							write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);

							EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
							write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);

							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								PCS_Ctrl_Data[i].Power_Factor=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
								Send_Data_To_PCS_0x10_02[i]=true;
							}
						}
					}
				}
			break;

			case 10103://无功功率
				if(PC_CAN_SET_DATA)
				{
					if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==QP)
					{
						if(((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*0.3)&&((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*0.3))
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=(int16_t)INT32_Val;
							write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);

							EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
							write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
						}
					}
				}
			break;

			case 10110: //恒压模式放电电压
				if(PC_CAN_SET_DATA)
				{
					if((INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min)&&(INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max)&&
					   (INT32_Val<EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max))
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min=INT32_Val;
						write_eeprom(CONST_V_MODE_V_MIN_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min, 2);
					}
				}
			break;
			case 10111: //恒压模式充电电压
				if(PC_CAN_SET_DATA)
				{
					if((INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min)&&(INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max)&&
					   (INT32_Val>EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min))
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max=INT32_Val;
						write_eeprom(CONST_V_MODE_V_MAX_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max, 2);
					}
				}
			break;
			default: break;
		}
	}
	else if(addr >= 10120 && addr <= 10299)
	{
    	uint8_t index = 0;
    	index = (addr-10120)/10;
    	Addr_r = 10120+(addr-10120)%10;
    	switch(Addr_r)
    	{
			case 10120://时间段1起始时间:时
				if(INT32_Val<=23)
				{
					Time_Div_Info.Time_Div_Set_Info[index].Start_time.hour = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_START_TIME_HOUR_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Start_time.hour,1);
				}
			break;
			case 10121://时间段1起始时间:分
				if(INT32_Val<=59)
				{
					Time_Div_Info.Time_Div_Set_Info[index].Start_time.min = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_START_TIME_MIN_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Start_time.min,1);
				}
			break;
			case 10122://时间段1结束时间:时
				if(INT32_Val<=23)
				{
					Time_Div_Info.Time_Div_Set_Info[index].End_time.hour = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_END_TIME_HOUR_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].End_time.hour,1);
				}
			break;
			case 10123://时间段1结束时间:分
				if(INT32_Val<=59)
				{
					Time_Div_Info.Time_Div_Set_Info[index].End_time.min = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_END_TIME_MIN_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].End_time.min,1);
				}
			break;
			case 10124://时间段1运行模式  0-并网 1-离网 2-并离网切换
				if((INT32_Val==GRID_MODE)|| (INT32_Val==ISLAND_MODE)|| (INT32_Val==GRID_ISLAND_MODE))
				{
					Time_Div_Info.Time_Div_Set_Info[index].Run_Mode = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Run_Mode,1);
				}
			break;
			case 10125://时间段1控制模式
				//if((INT32_Val==0)|| (INT32_Val==1)|| (INT32_Val==2)||(INT32_Val==3))
				if(INT32_Val<=20)
				{
					Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode= (uint8_t)INT32_Val;
					if(Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode>=10)
					{
						System_Info.Config.Time_Divide_Function = 0;  //高级功能关闭分时电价
					}
					write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode,1);
				}
			break;
			case 10126://时间段1运行功率
				if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
				{
					Time_Div_Info.Time_Div_Set_Info[index].Run_Power = (int16_t)INT32_Val;
					write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+index*2,&Time_Div_Info.Time_Div_Set_Info[index].Run_Power,2);
				}
			break;
			case 10127://时间段1运行功率
				if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
				{
					Time_Div_Info.Time_Div_Set_Info[index].Mgrid_Run_Power = (int16_t)INT32_Val;
					write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+index*2,&Time_Div_Info.Time_Div_Set_Info[index].Mgrid_Run_Power,2);
				}
			break;
			default:
			break;
    	}
	}
	else if(addr >= 10300 && addr <= 10499)
	{
		switch(addr)
		{
			//保护参数
			case 10300: //电池电压上限2级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2=INT32_Val;
							write_eeprom(VBAT_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2, 2);
						}
					}
				}
			break;
			case 10301: //电池电压上限1级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1=INT32_Val;
							write_eeprom(VBAT_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1, 2);
						}
					}
				}
			break;
			case 10302: //电池电压下限1级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1=INT32_Val;
							write_eeprom(VBAT_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1, 2);
						}
					}
				}
			break;
			case 10303: //电池电压下限2级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Max)
					{
						if(INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 &&
						   INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2=INT32_Val;
							write_eeprom(VBAT_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2, 2);
						}
					}
				}
			break;
			case 10304: //电池电压下限1级恢复
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 &&
							INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover=INT32_Val;
							write_eeprom(VBAT_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover, 2);
						}
					}
				}
			break;
			case 10305: //电池电压下限2级恢复
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover=INT32_Val;
							write_eeprom(VBAT_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover, 2);
						}
					}
				}
			break;

			case 10306: //VBat滞环控制
				if(PC_CAN_SET_DATA)
				{
					//if((INT32_Val>=0) && (INT32_Val<=500))
					if(INT32_Val<=500)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis= INT32_Val;
						write_eeprom(VBAT_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis, 2);
					}
				}
			break;

			case 10307: //电池电压控制延迟时间
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Max)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay=INT32_Val;
						write_eeprom(VBAT_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay, 2);
					}
				}
			break;
			case 10308: //电池电压恢复延迟时间
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Max)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay=INT32_Val;
						write_eeprom(VBAT_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay, 2);
					}
				}
			break;

			case 10310: //SOC上限2级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2=INT32_Val;
							write_eeprom(SOC_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2, 2);
						}
					}
				}
			break;
			case 10311: //SOC上限1级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1=INT32_Val;
							write_eeprom(SOC_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1, 2);
						}
					}
				}
			break;
			case 10312: //SOC下限1级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1=INT32_Val;
							write_eeprom(SOC_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1, 2);
						}
					}
				}
			break;
			case 10313: //SOC下限2级
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Max)
					{
						if(INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1 &&
							INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2=INT32_Val;
							write_eeprom(SOC_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2, 2);
						}
					}
				}
			break;
			case 10314: //SOC下限1级恢复
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover=INT32_Val;
							write_eeprom(SOC_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover, 2);
						}
					}
				}
			break;
			case 10315: //SOC下限2级恢复
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Max)
					{
						if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover=INT32_Val;
							write_eeprom(SOC_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover, 2);
						}
					}
				}
			break;

			case 10316: //SOC滞环控制
				if(PC_CAN_SET_DATA)
				{
					//if((INT32_Val>=0) && (INT32_Val<=100))
					if(INT32_Val<=100)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis = INT32_Val;
						write_eeprom(SOC_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis, 2);
					}
				}
			break;

			case 10317: //SOC控制延迟时间
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Max)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay=INT32_Val;
						write_eeprom(SOC_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay, 2);
					}
				}
			break;
			case 10318: //SOC恢复延迟时间
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Max)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay=INT32_Val;
						write_eeprom(SOC_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay, 2);
					}
				}
			break;

			case 10320: //设备地址
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=1 && INT32_Val<=247)
					{
						System_Info.Modbus_Address=INT32_Val;
						write_eeprom(MODBUS_ADDRESS_ADDRESS, &System_Info.Modbus_Address, 1);
					}
				}
			break;
			case 10321: //端口号
				if(PC_CAN_SET_DATA)
				{
					if((1<=INT32_Val)&&(INT32_Val<= UINT16_MAX)&&(INT32_Val!=WEBSERVER_PORT)&&(INT32_Val!=IEC104_PORT))
					{
						System_Info.Modbus_Port=INT32_Val;
						(void)write_eeprom(NET_PORT_ADDRESS, &System_Info.Modbus_Port, 2);
						Com_Data.Touch_Set_Net_Port_Flag = true;
					}
				}
			break;

			case 10322: //IP地址0
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.ip[0]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10323: //IP地址1
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.ip[1]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10324: //IP地址2
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.ip[2]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10325: //IP地址3
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.ip[3]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10326: //子网掩码0
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.submask[0]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10327: //子网掩码1
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.submask[1]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10328: //子网掩码2
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.submask[2]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10329: //子网掩码3
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.submask[3]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10330: //网关0
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.gateway[0]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10331: //网关1
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.gateway[1]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10332: //网关2
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.gateway[2]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10333: //网关3
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val<=255)
					{
						System_Info.Net.gateway[3]=INT32_Val;
						Com_Data.Touch_Set_Net_Flag = true;
					}
				}
			break;
			case 10334://IP地址[0]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.ip[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_0_ADDRESS, &System_Info.Net2.ip[0], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10335://IP地址[1]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.ip[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_1_ADDRESS, &System_Info.Net2.ip[1], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10336://IP地址[2]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.ip[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_2_ADDRESS, &System_Info.Net2.ip[2], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10337://IP地址[3]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.ip[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_3_ADDRESS, &System_Info.Net2.ip[3], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10338://子网掩码[0]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.submask[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_0_ADDRESS, &System_Info.Net2.submask[0], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10339://子网掩码[1]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.submask[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_1_ADDRESS, &System_Info.Net2.submask[1], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10340://子网掩码[2]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.submask[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_2_ADDRESS, &System_Info.Net2.submask[2], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10341://子网掩码[3]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.submask[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_3_ADDRESS, &System_Info.Net2.submask[3], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10342://网关[0]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.gateway[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_0_ADDRESS, &System_Info.Net2.gateway[0], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10343://网关[1]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.gateway[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_1_ADDRESS, &System_Info.Net2.gateway[1], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10344://网关[2]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.gateway[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_2_ADDRESS, &System_Info.Net2.gateway[2], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10345://网关[3]
				if(INT32_Val <= 255)
				{
					System_Info.Net2.gateway[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_3_ADDRESS, &System_Info.Net2.gateway[3], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 10370://IP地址[0]
				if(INT32_Val <= 255)
				{
					System_Info.NTP.ip[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_0_ADDRESS, &System_Info.NTP.ip[0], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 10371://IP地址[1]
				if(INT32_Val <= 255)
				{
					System_Info.NTP.ip[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_1_ADDRESS, &System_Info.NTP.ip[1], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 10372://IP地址[2]
				if(INT32_Val <= 255)
				{
					System_Info.NTP.ip[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_2_ADDRESS, &System_Info.NTP.ip[2], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 10373://IP地址[3]
				if(INT32_Val <= 255)
				{
					System_Info.NTP.ip[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_3_ADDRESS, &System_Info.NTP.ip[3], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 10374:
				if(INT32_Val > 0 && INT32_Val <= 65535)
				{
					System_Info.NTP.port = INT32_Val;
					(void)write_eeprom(NTP_PORT_ADDRESS, &System_Info.NTP.port, 2);
					//Com_Data.Touch_Set_NTP_Flag = true;
					char ipStr[16]={0};
					memset(ipStr, 0x00, 16);
					sprintf(ipStr,"%d.%d.%d.%d", System_Info.NTP.ip[0],System_Info.NTP.ip[1],System_Info.NTP.ip[2],System_Info.NTP.ip[3]);
				    ctrl.ntp.SetServer(ipStr, System_Info.NTP.port);
				}
			break;
			case 10380://IEC104通讯地址
				if((1 <= INT32_Val) && (INT32_Val <= 65535))
				{
					System_Info.IEC104_Address=INT32_Val;
					(void)write_eeprom(IEC104_ADDRESS_ADDRESS, &System_Info.IEC104_Address, 2);
				}
			break;
			case 10381://IEC104超时时间T1
				if((1 <= INT32_Val) && (INT32_Val < System_Info.IEC104_T3))
				{
					System_Info.IEC104_T1=INT32_Val;
					(void)write_eeprom(IEC104_T1_ADDRESS, &System_Info.IEC104_T1, 2);
				}
			break;
			case 10382://IEC104超时时间T3
				if((System_Info.IEC104_T1 < INT32_Val) && (INT32_Val <= 255))
				{
					System_Info.IEC104_T3=INT32_Val;
					(void)write_eeprom(IEC104_T3_ADDRESS, &System_Info.IEC104_T3, 2);
				}
			break;
			case 10383://IEC104参数k
				if((System_Info.IEC104_W<=((uint16_t)INT32_Val*2/3))&&(INT32_Val<=32767))
				{
					System_Info.IEC104_K = INT32_Val;
					(void)write_eeprom(IEC104_K_ADDRESS,&System_Info.IEC104_K,2);
				}
			break;
			case 10384://IEC104参数w
				if((1<=INT32_Val)&&(INT32_Val<=((uint16_t)System_Info.IEC104_K*2/3)))
				{
					System_Info.IEC104_W = INT32_Val;
					(void)write_eeprom(IEC104_W_ADDRESS,&System_Info.IEC104_W,2);
				}
			break;
			case 10400: //储能变流器总台数
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number=INT32_Val;
						write_eeprom(TOTAL_PCS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number, 2);
					}
				}
			break;
			case 10401: //光伏变流器总台数
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number=INT32_Val;
						write_eeprom(TOTAL_PV_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number, 2);
					}
				}
			break;
			case 10402: //BMS台数
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number=INT32_Val;
						write_eeprom(TOTAL_BMS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number, 2);
					}
				}
			break;
			case 10403: //DCDC台数
				if(PC_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number=INT32_Val;
						write_eeprom(TOTAL_DCDC_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number, 2);
					}
				}
			break;
			default: break;
		}
	}
	else if(addr >= 10500 && addr <= 10599)  //PCS modbus
	{
    	uint8_t index = 0;
    	index = addr-10500;
    	if(index < PCS_NUM)
    	{
			if(INT32_Val>=1 && INT32_Val<=247)
			{
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
				{
					if(i!=index)
					{
						if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address==INT32_Val)
						{
							Address_used=1;
						}
					}
				}
				if(Address_used==0)
				{
					EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[index].Modubus_Address=INT32_Val;
					write_eeprom(PCS0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[index].Modubus_Address, 1);
				}
			}
			if(PCS_Svr_Param_Change[index]==true)
			{
				PCS_Svr_Param_Change[index]=false;
				char ipStr[16]={0};
				sprintf(ipStr,"%d.%d.%d.%d", System_Info.Server[index].ip[0],System_Info.Server[index].ip[1],
						System_Info.Server[index].ip[2],System_Info.Server[index].ip[3]);
				ctrl.Client[index].SetServer(ipStr, System_Info.Server_Modbus_Port[index]);
			}
    	}
	}
	else if(addr >= 10600 && addr <= 10699)   //PV modbus
	{
    	uint8_t index = 0;
    	index = addr-10600;
    	if(index < PV_NUM)
    	{
			if(INT32_Val>=1 && INT32_Val<=247)
			{
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					if(i!=index)
					{
						if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address==INT32_Val)
						{
							Address_used=1;
						}
					}
				}
				if(Address_used==0)
				{
					EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[index].Modubus_Address=INT32_Val;
					write_eeprom(PV0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[index].Modubus_Address, 1);
				}
			}
			if(PV_Svr_Param_Change[index]==true)
			{
				PV_Svr_Param_Change[index]=false;
				char ipStr[16]={0};
				sprintf(ipStr,"%d.%d.%d.%d", System_Info.PVServer[index].ip[0],System_Info.PVServer[index].ip[1],
						System_Info.PVServer[index].ip[2],System_Info.PVServer[index].ip[3]);
				ctrl.PvClient[index].SetServer(ipStr, System_Info.PVServer_Modbus_Port[index]);
			}
    	}
	}
	else if(addr >= 10700 && addr <= 10799)   //BMS modbus
	{
    	uint8_t index = 0;
    	index = addr-10700;
    	if(index < BMS_NUM)
    	{
    		if(INT32_Val>=1 && INT32_Val<=247)
    		{
    			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
    			{
    				if(i!=index)
    				{
    					if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address==INT32_Val)
    					{
    						Address_used=1;
    					}
    				}
    			}
    			if(Address_used==0)
    			{
    				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[index].Modubus_Address=INT32_Val;
    				write_eeprom(BMS0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[index].Modubus_Address, 1);
    			}
    		}
    	}
	}
	else if(addr >= 10800 && addr <= 10899)   //DCDC modbus
	{
    	uint8_t index = 0;
    	index = addr-10800;
    	if(index < DCDC_NUM)
    	{
    		if(INT32_Val>=1 && INT32_Val<=247)
    		{
    			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
    			{
    				if(i!=index)
    				{
    					if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address==INT32_Val)
    					{
    						Address_used=1;
    					}
    				}
    			}
    			if(Address_used==0)
    			{
    				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[index].Modubus_Address=INT32_Val;
    				write_eeprom(DCDC0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[index].Modubus_Address, 1);
    			}
    		}
    	}
	}
	else if(addr >= 10900 && addr <= 10999)   //Ammeter modbus
	{
    	uint8_t index = 0;
    	index = addr-10900;
    	if(index < Meter_NUM)
    	{
    		if(INT32_Val>=1 && INT32_Val<=247)
    		{
    			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;i++)
    			{
    				if(i!=index)
    				{
    					if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address==INT32_Val)
    					{
    						Address_used=1;
    					}
    				}
    			}
    			if(Address_used==0)
    			{
    				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[index].Modubus_Address=INT32_Val;
    				write_eeprom(AMMETER0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[index].Modubus_Address, 1);
    			}
    		}
    	}
	}
	else if(addr >= 11000 && addr <= 11499)   //PCS server param
	{
    	uint8_t index = 0;
    	index = (addr-11000)/5;
    	Addr_r = 11000+(addr-11000)%5;
    	if(index<PCS_NUM)
    	{
			switch(Addr_r)
			{
				case 11000: //IP地址0
					if(INT32_Val<=255)
					{
						PCS_Svr_Param_Change[index]=true;
						if(System_Info.Server[index].ip[0]!=INT32_Val)
						{
							System_Info.Server[index].ip[0]=INT32_Val;
							write_eeprom(SERVER_0_IP_0_ADDRESS+index*4, &System_Info.Server[index].ip[0], 1);
						}
					}
				break;
				case 11001: //IP地址1
					if(INT32_Val<=255)
					{
						PCS_Svr_Param_Change[index]=true;
						if(System_Info.Server[index].ip[1]!=INT32_Val)
						{
							System_Info.Server[index].ip[1]=INT32_Val;
							write_eeprom(SERVER_0_IP_1_ADDRESS+index*4, &System_Info.Server[index].ip[1], 1);
						}
					}
				break;
				case 11002: //IP地址2
					if(INT32_Val<=255)
					{
						PCS_Svr_Param_Change[index]=true;
						if(System_Info.Server[index].ip[2]!=INT32_Val)
						{
							System_Info.Server[index].ip[2]=INT32_Val;
							write_eeprom(SERVER_0_IP_2_ADDRESS+index*4, &System_Info.Server[index].ip[2], 1);
						}
					}
				break;
				case 11003: //IP地址3
					if(INT32_Val<=255)
					{
						PCS_Svr_Param_Change[index]=true;
						if(System_Info.Server[index].ip[3]!=INT32_Val)
						{
							System_Info.Server[index].ip[3]=INT32_Val;
							write_eeprom(SERVER_0_IP_3_ADDRESS+index*4, &System_Info.Server[index].ip[3], 1);
						}
					}
				break;
				case 11004: //端口号
					if((1<=INT32_Val)&&(INT32_Val<= UINT16_MAX))
					{
						PCS_Svr_Param_Change[index]=true;
						if(System_Info.Server_Modbus_Port[index]!=INT32_Val)
						{
							System_Info.Server_Modbus_Port[index]=INT32_Val;
							write_eeprom(SERVER_0_PORT_ADDRESS+index*2, &System_Info.Server_Modbus_Port[index], 2);
						}
					}
				break;
				default:
				break;
			}
    	}
	}
	else if(addr >= 11500 && addr <= 11999)
	{
    	uint8_t index = 0;
    	index = (addr-11500)/5;
    	Addr_r = 11500+(addr-11500)%5;
    	if(index<PV_NUM)
    	{
    		switch(Addr_r)
			{
				case 11500: //IP地址0
					if(INT32_Val<=255)
					{
						PV_Svr_Param_Change[index]=true;
						if(System_Info.PVServer[index].ip[0]!=INT32_Val)
						{
							System_Info.PVServer[index].ip[0]=INT32_Val;
							write_eeprom(SERVER_PV_0_IP_0_ADDRESS+index*4, &System_Info.PVServer[index].ip[0], 1);
						}
					}
				break;
				case 11501: //IP地址1
					if(INT32_Val<=255)
					{
						PV_Svr_Param_Change[index]=true;
						if(System_Info.PVServer[index].ip[1]!=INT32_Val)
						{
							System_Info.PVServer[index].ip[1]=INT32_Val;
							write_eeprom(SERVER_PV_0_IP_1_ADDRESS+index*4, &System_Info.PVServer[index].ip[1], 1);
						}
					}
				break;
				case 11502: //IP地址2
					if(INT32_Val<=255)
					{
						PV_Svr_Param_Change[index]=true;
						if(System_Info.PVServer[index].ip[2]!=INT32_Val)
						{
							System_Info.PVServer[index].ip[2]=INT32_Val;
							write_eeprom(SERVER_PV_0_IP_2_ADDRESS+index*4, &System_Info.PVServer[index].ip[2], 1);
						}
					}
				break;
				case 11503: //IP地址3
					if(INT32_Val<=255)
					{
						PV_Svr_Param_Change[index]=true;
						if(System_Info.PVServer[index].ip[3]!=INT32_Val)
						{
							System_Info.PVServer[index].ip[3]=INT32_Val;
							write_eeprom(SERVER_PV_0_IP_3_ADDRESS+index*4, &System_Info.PVServer[index].ip[3], 1);
						}
					}
				break;
				case 11504: //端口号
					if((1<=INT32_Val)&&(INT32_Val<= UINT16_MAX))
					{
						PV_Svr_Param_Change[index]=true;
						if(System_Info.PVServer_Modbus_Port[index]!=INT32_Val)
						{
							System_Info.PVServer_Modbus_Port[index]=INT32_Val;
							write_eeprom(SERVER_PV_0_PORT_ADDRESS+index*2, &System_Info.PVServer_Modbus_Port[index], 2);
						}
					}
				break;
				default:
				break;
			}
    	}
	}
	else if(addr >= 30000 && addr <= 30020)
	{
		switch(addr)
	    {
	        case 30000: //新旧协议
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==NEW_PROTOCOL) || (INT32_Val==OLD_PROTOCOL))
					{
						System_Info.Config.PCS_Protocol_Type=(uint8_t)INT32_Val;
						write_eeprom(PCS_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.PCS_Protocol_Type, 1);
					}
            	}
	        break;
	        case 30001://母线类型
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0 )||( INT32_Val==1))
					{
						System_Info.Config.Bus_Type=(uint8_t)INT32_Val;
						write_eeprom(BUS_TYPE_ADDRESS, &System_Info.Config.Bus_Type, 1);
					}
            	}
	        break;
	        case 30002://直流母线PCS从电网充电使能开关
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val==0 || INT32_Val==1)
					{
						System_Info.Config.DC_Bus_Charge_Permit=(uint8_t)INT32_Val;
						write_eeprom(DC_BUS_CHARGE_PERMIT_ADDRESS, &System_Info.Config.DC_Bus_Charge_Permit, 1);
					}
            	}
	        break;
	        case 30003://系统功率设置
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					//if(INT32_Val>=SYS_POWER_SET_MIN && INT32_Val<=SYS_POWER_SET_MAX)
					if(INT32_Val<=SYS_POWER_SET_MAX)
					{
						System_Info.Config.System_Power_Set=INT32_Val;
						write_eeprom(SYS_POWER_SET_ADDRESS, &System_Info.Config.System_Power_Set, 2);
					}
            	}
	        break;
	        case 30004://PCS型号确定
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==PCS_Power_SC30_50)||(INT32_Val==PCS_Power_SC100)||(INT32_Val==PCS_Power_SC250)
					|| (INT32_Val==PCS_Power_SC500)||(INT32_Val==PCS_Power_SC500TL)||(INT32_Val==PCS_Power_SC1000))
					{
						System_Info.Config.PCS_Type=INT32_Val;
						Com_Data.Load_Default_Flag = 0xAA;//设置完需回复出厂设置
						write_eeprom(SYS_PCS_POWER_TYPE_ADDRESS, &System_Info.Config.PCS_Type, 1);
					}
            	}
	        break;
			case 30005: //配电功能使能/禁止
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0xAA )||( INT32_Val==0x55))
					{
						System_Info.Config.Power_Distribution_Enable=INT32_Val;
						write_eeprom(POWER_DISTRIBUTION_ENABLE_ADDRESS, &System_Info.Config.Power_Distribution_Enable, 1);
					}
            	}
			break;
	        case 30006: //分时电价控制使能
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0) || (INT32_Val==1) ||(INT32_Val==2))
					{
						System_Info.Config.Time_Divide_Function = INT32_Val;
						write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
					}
            	}
	        break;
	        case 30007: //BMS类型
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val== BMS_None)||(INT32_Val==BMS_68AH)||(INT32_Val==BMS_94AH))
					{
						System_Info.Config.BMS_Type=(uint8_t)INT32_Val;
						write_eeprom(BMS_TYPE_ADDRESS, & System_Info.Config.BMS_Type, 1);
					}
            	}
	        break;
	        case 30008: //PCS通讯模式选择
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val== PCS_COMM_SERIAL)||(INT32_Val==PCS_COMM_LAN))
					{
						System_Info.Config.PCS_Comm_Type=(uint8_t)INT32_Val;
						write_eeprom(PCS_COMM_ADDRESS, & System_Info.Config.PCS_Comm_Type, 1);
					}
            	}
	        break;
	        case 30009: //PCS并网工作模式选择
				if((INT32_Val==GRID_CONST_I_MODE)||(INT32_Val==GRID_CONST_V_MODE)||(INT32_Val==GRID_CONST_P_DC_MODE)
				|| (INT32_Val==GRID_CONST_P_AC_MODE)||(INT32_Val==GRID_STANDBY_MODE))
				{
					System_Info.Config.PCS_Grid_Run_Mode=(uint8_t)INT32_Val;
					write_eeprom(PCS_GRID_RUN_MODE_ADDRESS, & System_Info.Config.PCS_Grid_Run_Mode, 1);
				}
	        break;
	        case 30010: //DCDC通讯使能/禁止
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0 )||( INT32_Val==1)||( INT32_Val==2))
					{
						System_Info.Config.DCDC_Type=INT32_Val;
						write_eeprom(DCDC_TYPE_ADDRESS, &System_Info.Config.DCDC_Type, 1);
					}
            	}
	        break;
	        case 30011: //Meter通讯使能/禁止
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0xAA )||( INT32_Val==0x55))
					{
						System_Info.Config.Meter_Comm_Enable=INT32_Val;
						write_eeprom(METER_COMM_ENABLE_ADDRESS, &System_Info.Config.Meter_Comm_Enable, 1);
					}
            	}
	        break;
	        case 30013://并网类型选择
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val==0 || INT32_Val==1)
					{
						Grid_Connect_Type=(uint8_t)INT32_Val;
						write_eeprom(GRID_CONNECT_TYPE_ADDRESS, &Grid_Connect_Type, 1);
					}
            	}
	        break;
	        case 30014: //soc均衡使能控制
	            if((INT32_Val==0xAA )||( INT32_Val==0x55))
	            {
	                System_Info.Config.Balancing_Enable = INT32_Val;
	                write_eeprom(BALANCING_ENABLE_ADDRESS, &System_Info.Config.Balancing_Enable, 1);
	                if(System_Info.Config.Balancing_Enable == ENABLE) //SOC均衡使能，同时使能SOC控制
	                {
	                    EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable  = ENABLE;
	                    write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
	                    EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable = DISABLE;
	                    write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
	                }
	            }
	        break;
	        case 30015: //PCS离网启动策略
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val== 0)||(INT32_Val==1))
					{
						System_Info.Config.PCS_Island_Ctrl_Mode=(uint8_t)INT32_Val;
						write_eeprom(PCS_ISLAND_CTRL_MODE_ADDRESS, &System_Info.Config.PCS_Island_Ctrl_Mode, 1);
					}
            	}
			break;
            case 30016://最大逆流功率
                if((INT32_Val<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1))&&(INT32_Val>=0))
                {
                    Max_anti_reflux_power= (int16_t)INT32_Val;
                    write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
                }
                break;
            case 30017://网口下发周期
            	if((INT32_Val >= 0) && (INT32_Val <= 2000))
            	{
            		EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay = INT32_Val;
            		write_eeprom(PCS_NET_SETPARA_DELAY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay, 2);
            	}
            	break;
			case 30018: //电表协议选择
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val == 0 || INT32_Val == 1)
					{
						System_Info.Config.Ammeter_Protocol_Type = INT32_Val;
						write_eeprom(AMMETER_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.Ammeter_Protocol_Type, 1);
					}
            	}
			break;
	        default: break;
	    }
	}
	else
	{
		switch(addr)
		{
			//------------------------------------IAT--------------------------------------------//
			case 36008: //SN序列号使能
				if (INT32_Val == SN_ENABLE || INT32_Val == SN_DISABLE)
				{
					IAT_Info.Set_Info.SN_Enable = INT32_Val;
					(void) write_eeprom(IAT_SN_ENABLE_ADDRESS, &IAT_Info.Set_Info.SN_Enable, 2);
				}
				break;
			case 36009: //SN序列号0  ccg 2013.5.15
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[0] = HIBYTE(INT32_Val);
					sn_temp[1] = LOBYTE(INT32_Val);
				}
				break;
			case 36010: //SN序列号1
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[2] = HIBYTE(INT32_Val);
					sn_temp[3] = LOBYTE(INT32_Val);
				}
				break;
			case 36011: //SN序列号2
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[4] = HIBYTE(INT32_Val);
					sn_temp[5] = LOBYTE(INT32_Val);
				}
				break;
			case 36012: //SN序列号3
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[6] = HIBYTE(INT32_Val);
					sn_temp[7] = LOBYTE(INT32_Val);
				}
				break;
			case 36013: //SN序列号4
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[8] = HIBYTE(INT32_Val);
					sn_temp[9] = LOBYTE(INT32_Val);
				}
				break;
			case 36014: //SN序列号5
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[10] = HIBYTE(INT32_Val);
					sn_temp[11] = LOBYTE(INT32_Val);
				}
				break;
			case 36015: //SN序列号6
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[12] = HIBYTE(INT32_Val);
					sn_temp[13] = LOBYTE(INT32_Val);
				}
				break;
			case 36016: //SN序列号7
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[14] = HIBYTE(INT32_Val);
					sn_temp[15] = LOBYTE(INT32_Val);
				}
				break;
			case 36017: //SN序列号8
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[16] = HIBYTE(INT32_Val);
					sn_temp[17] = LOBYTE(INT32_Val);
				}
				break;
			case 36018: //SN序列号9
				if (IAT_Info.Set_Info.SN_Enable == SN_ENABLE)
				{
					sn_temp[18] = HIBYTE(INT32_Val);
					sn_temp[19] = LOBYTE(INT32_Val);
					if (Check_Sn(sn_temp, Sn_Num(sn_temp, sizeof(sn_temp))) == true)
					{
						memcpy(IAT_Info.Set_Info.SN_Number, sn_temp, 20);
						(void) write_eeprom(IAT_SN_NUMBER_ADDRESS, &IAT_Info.Set_Info.SN_Number, 20);
						IAT_Info.Set_Info.SN_Enable = SN_DISABLE;
						(void) write_eeprom(IAT_SN_ENABLE_ADDRESS, &IAT_Info.Set_Info.SN_Enable, 2);
					}
					memset(sn_temp, 0, 20);
				}
				break;
			/*case 36019: //MAC地址  ccg 2013.5.15
				if (IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[0] = INT32_Val;
				}
				break;
			case 36020: //MAC地址
				if (IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[1] = INT32_Val;
				}
				break;
			case 36021: //MAC地址
				if (IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[2] = INT32_Val;
				}
				if (HIBYTE(mac_temp[0]) == 0xAC && LOBYTE(mac_temp[0]) == 0x19 && HIBYTE(mac_temp[1]) == 0x9F)
				{
					memcpy(IAT_Info.Set_Info.MAC, mac_temp, 6);
					for (uint8_t i = 0; i < 3; i++)
					{
						(void) write_eeprom(IAT_MAC_ADDRESS + i * 2, &IAT_Info.Set_Info.MAC[i], 2);
					}
				}
				memset(mac_temp, 0, 6);
				break;
			case 36022: //MAC地址锁定  ccg 2013.5.15
				if (IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					IAT_Info.Set_Info.MAC_Lock = INT32_Val;
					(void) write_eeprom(IAT_MAC_LOCK_ADDRESS, &IAT_Info.Set_Info.MAC_Lock, 2);
				}
				break;
			case 36023: //MAC地址  ccg 2013.5.15
				if (IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[0] = INT32_Val;
				}
				break;
			case 36024: //MAC地址
				if (IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[1] = INT32_Val;
				}
				break;
			case 36025: //MAC地址
				if (IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[2] = INT32_Val;
				}
				if (HIBYTE(mac_temp[0]) == 0xAC && LOBYTE(mac_temp[0]) == 0x19 && HIBYTE(mac_temp[1]) == 0x9F)
				{
					memcpy(IAT_Info.Set_Info.MAC2, mac_temp, 6);
					for (uint8_t i = 0; i < 3; i++)
					{
						(void) write_eeprom(IAT_MAC2_ADDRESS + i * 2, &IAT_Info.Set_Info.MAC2[i], 2);
					}
				}
				memset(mac_temp, 0, 6);
				break;*/
			case 40000://事件上一页
				if(INT32_Val==0)
				{
					if(read_event_record.current_page_nub>1)
					{
						read_event_record.current_page_nub--;
						read_event_record.read_event_record_flash_flag=true;
					}
				}
			break;
			case 40001://事件下一页
				if(INT32_Val==1)
				{
					if(read_event_record.current_page_nub<read_event_record.total_page_nub)
					{
					   read_event_record.current_page_nub++;
					   read_event_record.read_event_record_flash_flag=true;
					}
				}
			break;
			case 40002://故障上一页
				if(INT32_Val==0)
				{
					if(read_fault_record.current_page_nub>1)
					{
						read_fault_record.current_page_nub--;
						read_fault_record.read_fault_record_flash_flag=true;
					}
				}
			break;
			case 40003://故障下一页
				if(INT32_Val==1)
				{
					if(read_fault_record.current_page_nub<read_fault_record.total_page_nub)
					{
					   read_fault_record.current_page_nub++;
					   read_fault_record.read_fault_record_flash_flag=true;
					}
				}
			break;
			default:
			break;
		}
	}
}

uint8_t EMS_Receive_Data_From_Monitor_By_UART_Modbus(uint8_t *pbuf, uint32_t buflen, uint8_t *sbuf) 	//处理来自PC串口的数据
{
	uint16_t crcresult = 0; 			//crc值
	uint16_t modbus_address = (uint16_t) (((uint16_t) pbuf[2] << 8) | pbuf[3]); 			//modbus参数起始地址
	uint16_t modbus_len = (uint16_t) (((uint16_t) pbuf[4] << 8) | pbuf[5]);
	uint16_t len = 0;
	uint16_t slen = 0;
	uint8_t fault_code = 0;
	if (pbuf[1] == 0x06)
	{
		modbus_len = 0;
	}
	crcresult = embf_get_crc16(pbuf, buflen - 2);
	if (LOBYTE(crcresult) == pbuf[buflen - 2] && HIBYTE(crcresult) == pbuf[buflen - 1]) 			//CRC校验要正确
	{
		if (comParam[5].sId == pbuf[0]) 			//modbus地址正确
		{
			if((iapv2_range_chk(modbus_address, (uint8_t)modbus_len, pbuf[1]) == 1) ||
			(property_range_chk(modbus_address, (uint8_t)modbus_len, pbuf[1]) == 1) //|| (baudadj_range_chk(modbus_address, (uint8_t)modbus_len, pbuf[1]) == 1)
			 )
			{
				slen = iapv2_comdata_proc(pbuf, (uint16_t)buflen, sbuf);
				/*if(iapv2_updating() == true)
				{
					task_prio_prompt();
				}
				else
				{
					task_prio_recover();
				}*/
			}
			else
			{
				if ((pbuf[1] != 0x01) && (pbuf[1] != 0x03) && (pbuf[1] != 0x04) && (pbuf[1] != 0x06) && (pbuf[1] != 0x10) && (pbuf[1] != 0x42) && (pbuf[1] != 0x43) && (pbuf[1] != 0x44))
				{
					fault_code = 1;				//功能码异常
				}
				else if (((modbus_len < 1) || (modbus_len > 0x7d)) && (pbuf[1] != 0x06))
				{
					fault_code = 3;				//寄存器数目异常
				}
				else if ((modbus_address < 9999) || (modbus_address + modbus_len > 39999))
				{
					fault_code = 2;				//寄存器地址异常
				}
				else
				{
					if (pbuf[1] == 0x03)
					{
						g_adjust_flag = 0xA1;				//不处于校表状态//lixia;2012.12.20;
						Monitor_Get_Data_From_EMS_0x03(modbus_address, modbus_len, sbuf + 3);
						memcpy(sbuf, pbuf, 2);
						len = modbus_len * 2;				//返回字节长度
						sbuf[2] = (uint8_t) len;
						crcresult = embf_get_crc16(sbuf, 3 + len);
						sbuf[3 + len] = LOBYTE(crcresult);				//ccg 2013.5.28
						sbuf[4 + len] = HIBYTE(crcresult);
						slen = len + 5;				//返回读取的字节长度
					}
					else if (pbuf[1] == 0x04)
					{
						g_adjust_flag = 0xA1;				//不处于校表状态//lixia;2012.12.20;
						Monitor_Get_Data_From_EMS_0x04(modbus_address, modbus_len, sbuf + 3);
						memcpy(sbuf, pbuf, 2);
						len = modbus_len * 2;				//返回字节长度
						sbuf[2] = (uint8_t) len;
						crcresult = embf_get_crc16(sbuf, 3 + len);
						sbuf[3 + len] = LOBYTE(crcresult);				//ccg 2013.5.28
						sbuf[4 + len] = HIBYTE(crcresult);
						slen = len + 5;				//返回读取的字节长度
					}
					else if (pbuf[1] == 0x06)
					{
						g_adjust_flag = 0xA1;				//不处于校表状态//lixia;2012.12.20;

						Touch_Write_Time[0] = showTime.tm_year;       //lixia;2013.5.13;
						Touch_Write_Time[1] = showTime.tm_mon;
						Touch_Write_Time[2] = showTime.tm_day;
						Touch_Write_Time[3] = showTime.tm_hour;
						Touch_Write_Time[4] = showTime.tm_min;
						Touch_Write_Time[5] = showTime.tm_sec;

						Monitor_Set_Data_To_EMS_0x06(modbus_address, pbuf + 4);
						memcpy(sbuf, pbuf, 8);
						slen = 8;
					}
					else if (pbuf[1] == 0x10)
					{
						g_adjust_flag = 0xA1;       //不处于校表状态//lixia;2012.12.20;

						Touch_Write_Time[0] = showTime.tm_year;       //lixia;2013.5.13;
						Touch_Write_Time[1] = showTime.tm_mon;
						Touch_Write_Time[2] = showTime.tm_day;
						Touch_Write_Time[3] = showTime.tm_hour;
						Touch_Write_Time[4] = showTime.tm_min;
						Touch_Write_Time[5] = showTime.tm_sec;

						for (uint8_t i = 0; i < GetU16(pbuf + 4); i++)
						{
							Monitor_Set_Data_To_EMS_0x06(modbus_address + i, pbuf + 7 + i * 2);
							memcpy(sbuf, pbuf, 6);
							crcresult = embf_get_crc16(sbuf, 6);
							sbuf[6] = LOBYTE(crcresult);       //ccg 2013.5.28
							sbuf[7] = HIBYTE(crcresult);
							slen = 8;
						}
					}
					else
					{
						slen = 0;
					}
					if (Touch_Write_Time_Flag == true)
					{
						Touch_Write_Time_Flag = false;
						//(void) drv_write(gt_glp.frtc, Touch_Write_Time, 6);       //设置时间函数
						set_sys_time();
						get_sys_time();
						for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
						{
							Send_Data_To_PCS_0x10_01[i] = true;
						}
						for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
						{
							Send_Data_To_PV_0x10_01[i]=true;
						}
						//DCDC
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
						{
							Send_Data_To_DCDC_0x10_01[i]=true;
						}
						delay10s = true;
						Clear_Data = true;								//判断是否是新的一天，是否要清除前一天的数据
					}
				}
				if (fault_code != 0)
				{
					sbuf[0] = pbuf[0];
					sbuf[1] = pbuf[1] | 0x80;
					sbuf[2] = fault_code;
					crcresult = embf_get_crc16(sbuf, 2);
					sbuf[3] = LOBYTE(crcresult);								//ccg 2013.5.28
					sbuf[4] = HIBYTE(crcresult);
					slen = 5;
				}
			}
		}
	}
	return (uint8_t) slen;
}
uint8_t EMS_Receive_Data_From_Monitor_By_NET_Modbus(uint8_t *pbuf, uint16_t buflen, uint8_t *sbuf)		//处理来自PC网络接口的数据
{
	uint16_t Modbus_type = (uint16_t) (((uint16_t) pbuf[2] << 8) | pbuf[3]);
	uint16_t data_amount = (uint16_t) (((uint16_t) pbuf[4] << 8) | pbuf[5]);
	uint16_t modbus_address = (uint16_t) (((uint16_t) pbuf[8] << 8) | pbuf[9]);	//modbus地址
	uint16_t modbus_len = (uint16_t) (((uint16_t) pbuf[10] << 8) | pbuf[11]);
	uint16_t len = 0;
	uint16_t slen = 0;
	uint8_t fault_code = 0;
	if (pbuf[7] == 0x06)
	{
		modbus_len = 0;
	}
	if (Modbus_type == 0)
	{
		if (data_amount == (buflen - 6))
		{
			if (System_Info.Modbus_Address == pbuf[6])	//modbus地址为本机地址
			{
				if((iapv2_range_chk(modbus_address, (uint8_t)modbus_len, pbuf[7]) == 1) ||
				(property_range_chk(modbus_address, (uint8_t)modbus_len, pbuf[7]) == 1))
				{
					slen = iapv2_netdata_proc(pbuf, buflen, sbuf);
					/*if(iapv2_updating() == true)
					{
						(void)task_prio_prompt();
					}
					else
					{
						(void)task_prio_recover();
					}*/
				}
				else
				{
					if ((pbuf[7] != 0x03) && (pbuf[7] != 0x04) && (pbuf[7] != 0x06) && (pbuf[7] != 0x10))
					{
						fault_code = 1;
					}
					else if (((modbus_len < 1) || (modbus_len > 0x7d)) && (pbuf[7] != 0x06))
					{
						fault_code = 3;	//寄存器数目异常
					}
					else if ((modbus_address < 9999) || (modbus_address + modbus_len > 39999))
					{
						fault_code = 2;
					}
					else
					{
						if (pbuf[7] == 0x03)
						{
							Monitor_Get_Data_From_EMS_0x03(modbus_address, modbus_len, sbuf + 9);
							memcpy(sbuf, pbuf, 4);
							len = modbus_len * 2;	//返回字节长度
							sbuf[4] = HIBYTE(len+3);
							sbuf[5] = LOBYTE(len+3);
							sbuf[6] = pbuf[6];
							sbuf[7] = pbuf[7];
							sbuf[8] = (uint8_t) len;
							slen = len + 9;	//返回读取的字节长度
						}
						else if (pbuf[7] == 0x04)
						{
							Monitor_Get_Data_From_EMS_0x04(modbus_address, modbus_len, sbuf + 9);
							memcpy(sbuf, pbuf, 4);
							len = modbus_len * 2;	//返回字节长度
							sbuf[4] = HIBYTE(len+3);
							sbuf[5] = LOBYTE(len+3);
							sbuf[6] = pbuf[6];
							sbuf[7] = pbuf[7];
							sbuf[8] = (uint8_t) len;
							slen = len + 9;	//返回读取的字节长度
						}
						else if (pbuf[7] == 0x06)
						{
							Touch_Write_Time[0] = showTime.tm_year;       //lixia;2013.5.13;
							Touch_Write_Time[1] = showTime.tm_mon;
							Touch_Write_Time[2] = showTime.tm_day;
							Touch_Write_Time[3] = showTime.tm_hour;
							Touch_Write_Time[4] = showTime.tm_min;
							Touch_Write_Time[5] = showTime.tm_sec;

							Monitor_Set_Data_To_EMS_0x06(modbus_address, pbuf + 10);
							memcpy(sbuf, pbuf, 12);
							slen = 12;       //网络不需要CRC校验
						}
						else if (pbuf[7] == 0x10)
						{
							Touch_Write_Time[0] = showTime.tm_year;       //lixia;2013.5.13;
							Touch_Write_Time[1] = showTime.tm_mon;
							Touch_Write_Time[2] = showTime.tm_day;
							Touch_Write_Time[3] = showTime.tm_hour;
							Touch_Write_Time[4] = showTime.tm_min;
							Touch_Write_Time[5] = showTime.tm_sec;

							for (uint8_t i = 0; i < GetU16(pbuf + 10); i++)
							{
								Monitor_Set_Data_To_EMS_0x06(modbus_address + i, pbuf + 13 + i * 2);
								memcpy(sbuf, pbuf, 12);
								sbuf[5] = 6;
								slen = 12;       //网络不需要CRC校验
							}
						}
						else
						{
							slen = 0;
						}
						if (Touch_Write_Time_Flag == true)
						{
							Touch_Write_Time_Flag = false;
							//(void) drv_write(gt_glp.frtc, Touch_Write_Time, 6);       //设置时间函数
							set_sys_time();
							get_sys_time();
							for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
							{
								Send_Data_To_PCS_0x10_01[i] = true;
							}
							for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
							{
								Send_Data_To_PV_0x10_01[i]=true;
							}
							//DCDC
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
							{
								Send_Data_To_DCDC_0x10_01[i]=true;
							}						
							delay10s=true;
							Clear_Data = true;								//判断是否是新的一天，是否要清除前一天的数据
						}
					}
					if (fault_code != 0)
					{
						memcpy(sbuf, pbuf, 4);
						sbuf[4] = 0;
						sbuf[5] = 2;
						sbuf[6] = pbuf[6] | 0x80;
						sbuf[7] = fault_code;
						slen = 8;								//返回读取的字节长度
					}
				}
			}
		}
	}
	return (uint8_t) slen;
}

uint16_t iapv2_comdata_proc(uint8_t *data, uint16_t len, uint8_t *sbuf)                			//IAPV2升级(UART)
{
    uint8_t  ret      = 0;
    uint16_t slen     = 0;
    uint8_t  cmd_type = data[1];
    uint16_t bgn_reg  = GetU16(data + 2);
    uint8_t  num      = data[5];
    uint8_t  rlt;
    uint16_t crc;

    //ret = baudadj_range_chk(bgn_reg, num, cmd_type);
    if(ret == 1)//调节波特率
    {
        if(data[0] == System_Info.Modbus_Address)
        {
            if(cmd_type == 0x04)
            {
                //slen = baudadj_read_data(bgn_reg, sbuf + 3, num);
                sbuf[0] = data[0];
                sbuf[1] = data[1];
                sbuf[2] = slen;
                slen += 3;

                crc = embf_get_crc16(sbuf, slen);
                sbuf[slen] = LOBYTE(crc);
                sbuf[slen + 1] = HIBYTE(crc);
                slen += 2;
            }
            else if(cmd_type == 0x10)
            {
                //rlt = baudadj_write_data(bgn_reg, data + 7, num);
                sbuf[0] = data[0];
                sbuf[1] = data[1];
                //if(rlt == 0)
                //{
                //    sbuf[1] |= 0x80;
                //}
                sbuf[2] = data[2];
                sbuf[3] = data[3];
                sbuf[4] = data[4];
                sbuf[5] = data[5];
                slen = 6;

                crc = embf_get_crc16(sbuf, slen); //校验
                sbuf[slen] = LOBYTE(crc);
                sbuf[slen + 1] = HIBYTE(crc);
                slen += 2;
            }
        }
    }
    else		//IAPV2升级
    {
        ret = iapv2_range_chk(bgn_reg, num, cmd_type);
        if(ret == 1)
        {
            if((data[0] == System_Info.Modbus_Address) || (data[0] == 0x00))
            {
                /* IAP协议 */
                if(cmd_type == 0x03)
                {
                    slen = iapv2_read_data(bgn_reg, sbuf + 3, num);
                    sbuf[0] = data[0];
                    sbuf[1] = data[1];
                    sbuf[2] = (uint8_t)slen;
                    slen += 3;

                    crc = embf_get_crc16(sbuf, slen);
                    sbuf[slen] = LOBYTE(crc);
                    sbuf[slen + 1] = HIBYTE(crc);
                    slen += 2;
                }
                else if(cmd_type == 0x10)
                {
                    rlt = iapv2_write_data(bgn_reg, data + 7, num);
                    if(data[0] != 0)
                    {
                        sbuf[0] = data[0];
                        sbuf[1] = data[1];
                        if(rlt == 0)
                        {
                            sbuf[1] |= 0x80;
                        }
                        sbuf[2] = data[2];
                        sbuf[3] = data[3];
                        sbuf[4] = data[4];
                        sbuf[5] = data[5];
                        slen = 6;

                        crc = embf_get_crc16(sbuf, slen); //??
                        sbuf[slen] = LOBYTE(crc);
                        sbuf[slen + 1] = HIBYTE(crc);
                        slen += 2;
                    }
                }
            }
        }
        else
        {
            /* 属性协议*/
            ret = property_range_chk(bgn_reg, num, cmd_type);
            if(ret == 1)
            {
                if(data[0] == System_Info.Modbus_Address)
                {
                    if(cmd_type == 0x04)
                    {
                        slen=property_return_pc_cmd_04(bgn_reg, sbuf + 3, num);
                        if(slen != 0)
                        {
                            sbuf[0] = data[0];
                            sbuf[1] = data[1];
                            sbuf[2] = (uint8_t)slen;
                            slen += 3;

                            crc = embf_get_crc16(sbuf, slen);
                            sbuf[slen] = LOBYTE(crc);
                            sbuf[slen + 1] = HIBYTE(crc);
                            slen += 2;
                        }
                    }
                }
            }
        }
    }
    return slen;
}

uint16_t iapv2_netdata_proc(uint8_t *data, uint16_t len, uint8_t *sbuf)                			//IAPV2升级(NET)
{
    uint8_t  ret = 0;  /* IAP升级数据包标志 */
    uint16_t  slen = 0;
    uint8_t  cmd_type = data[7];
    uint16_t bgn_reg = GetU16(data + 8);
    uint16_t num = GetU16(data + 10);
    uint8_t rlt;

    //printf("iapv:");for(int j=0; j<len; j++) printf(" %02x", data[j]);printf("\r\n");
    ret = iapv2_range_chk(bgn_reg, (uint8_t)num, cmd_type);
    if(ret == 1)
    {
        if(data[6] == System_Info.Modbus_Address)
        {
            /* IAP通讯协议 */
            if(cmd_type == 0x03)
            {
                slen = iapv2_read_data(bgn_reg, sbuf + 9, (uint8_t)num);
                sbuf[0] = data[0];
                sbuf[1] = data[1];
                sbuf[2] = 0;
                sbuf[3] = 0;
                sbuf[4] = ((slen + 3) >> 8) & UINT8_MAX;
                sbuf[5] = (slen + 3) & UINT8_MAX;
                sbuf[6] = data[6];
                sbuf[7] = data[7];
                sbuf[8] = (uint8_t)slen;
                slen += 9;
            }
            else if(cmd_type == 0x10)
            {
                rlt = iapv2_write_data(bgn_reg, data + 13, (uint8_t)num);
                if(data[6] != 0)
                {
                    sbuf[0] = data[0];
                    sbuf[1] = data[1];
                    sbuf[2] = 0;
                    sbuf[3] = 0;
                    sbuf[4] = 0;
                    sbuf[5] = 6;
                    sbuf[6] = data[6];
                    sbuf[7] = data[7];
                    if(rlt == 0)
                    {
                        sbuf[7] |= 0x80;
                    }
                    sbuf[8] = data[8];
                    sbuf[9] = data[9];
                    sbuf[10] = data[10];
                    sbuf[11] = data[11];
                    slen = 12;
                }
            }
        }
    }
    else
    {
        /* 属性协议 */
        ret = property_range_chk(bgn_reg, (uint8_t)num, cmd_type);
        if(ret == 1)
        {
            if(data[6] == System_Info.Modbus_Address)
            {
                if(cmd_type == 0x04)
                {
                    slen = property_return_pc_cmd_04(bgn_reg, sbuf + 9, (uint8_t)num);
                    sbuf[0] = data[0];
                    sbuf[1] = data[1];
                    sbuf[2] = 0;
                    sbuf[3] = 0;
                    sbuf[4] = ((slen + 3) >> 8) & UINT8_MAX;
                    sbuf[5] = (slen + 3) & UINT8_MAX;
                    sbuf[6] = data[6];
                    sbuf[7] = data[7];
                    sbuf[8] = (uint8_t)slen;
                    slen += 9;
                }
            }
        }
    }
    return slen;
}

