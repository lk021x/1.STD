/*
 * com_to_touch.cpp
 *
 *  Created on: 2017-1-16
 *      Author: root
 */

#include "includes.h"

static int super_system(const char * cmd)
{
	FILE * fp;

	if (cmd == NULL) return -1;

	if ((fp = popen(cmd, "w") ) == NULL){   // "w" "r"
		//printf("POPEN error: %s\n", strerror(errno));
		return -1;
	}else{
		pclose(fp);
	}
	return 0;
}

void Touch_Get_Data_From_EMS_0x01(uint16_t addr, uint16_t len, uint8_t *data)          //触摸屏处理0x01命令
{
	for (uint8_t i = 0; i < len; i++)
	{
		if ((i + addr <= 63) && (i + addr >= 32))
		{
			if ((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t) 1 << i)) != 0)
			{
				data[i / 8] |= 1 << (i % 8);
			}
			else
			{
				data[i / 8] &= ~(1 << (i % 8));
			}
		}
		else if ((i + addr <= 95) && (i + addr >= 64))
		{
			if ((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2 & ((uint32_t) 1 << (i % 32))) != 0)
			{
				data[i / 8] |= 1 << (i % 8);
			}
			else
			{
				data[i / 8] &= ~(1 << (i % 8));
			}
		}
		else if ((i + addr >= 112) && (i + addr <= 127))
		{
			if ((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PCS_Comm_Fault & ((uint32_t) 1 << (i % 16))) != 0)
			{
				data[i / 8] |= 1 << (i % 8);
			}
			else
			{
				data[i / 8] &= ~(1 << (i % 8));
			}
		}
		else if ((i + addr >= 160) && (i + addr <= 175))
		{
			if (((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PV_Comm_Fault & 0xFFFF) & ((uint32_t) 1 << (i % 16))) != 0)
			{
				data[i / 8] |= 1 << (i % 8);
			}
			else
			{
				data[i / 8] &= ~(1 << (i % 8));
			}
		}
		else if ((i + addr >= 176) && (i + addr <= 191))
		{
			if (((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PV_Comm_Fault >> 16) & ((uint32_t) 1 << (i % 16))) != 0)
			{
				data[i / 8] |= 1 << (i % 8);
			}
			else
			{
				data[i / 8] &= ~(1 << (i % 8));
			}
		}
		else if ((i + addr >= 192) && (i + addr <= 207))
		{
			if (((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & 0xFFFF) & ((uint32_t) 1 << (i % 16))) != 0)
			{
				data[i / 8] |= 1 << (i % 8);
			}
			else
			{
				data[i / 8] &= ~(1 << (i % 8));
			}
		}
	}
}
void Touch_Get_Data_From_EMS_0x03(uint16_t addr, uint16_t len, uint8_t *data)          //触摸屏处理0x03命令
{
	uint8_t *l_data = data;

	for (uint8_t i = 0; i < len; i++)
	{
		switch (i + addr)
		{
			//时间设置
			case 1999:l_data = devices_savedata(l_data, (showTime.tm_sec));break;           //秒
			case 2000:l_data = devices_savedata(l_data, (showTime.tm_min));break;           //分
			case 2001:l_data = devices_savedata(l_data, (showTime.tm_hour));break;          //时
			case 2002:l_data = devices_savedata(l_data, (showTime.tm_day));break;           //日
			case 2003:l_data = devices_savedata(l_data, (showTime.tm_mon));break;           //月
			case 2004:l_data = devices_savedata(l_data, (showTime.tm_year) + 2000);break;     //年

			//系统参数配置
			case 2005:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.EMS_Run_Priority);break;          //运行模式 电网/微网优先
            //2006启动停机
			case 2007:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.EMS_Start_Type);break;            //启动类型  主被动
			case 2008:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Bus_Type);break;                  //总线类型
			case 2009:l_data=devices_savedata(l_data,(uint16_t)Grid_Connect_Type);break;    	                    //是否并网
			case 2010:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.DC_Bus_Charge_Permit);break;      //直流母线电网充电使能/禁止 
 			case 2011:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.System_Power_Set);break;          //系统功率设置
			case 2012:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Type);break;                  //PCS型号确定
			case 2013:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Time_Divide_Function);break;      //使能分时功率控制
			case 2014:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Comm_Type);break;               //PCS通讯方式选择
			case 2015:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Grid_Run_Mode);break;           //PCS并网工作模式 
			case 2016:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Power_Distribution_Enable);break;   //配电使能.禁止
			case 2017:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.DCDC_Type);break;                   //DCDC类型选择
			case 2018:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Meter_Comm_Enable);break;           //电表通讯使能.禁止 
			case 2019:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Protocol_Type);break;  //协议类型
			
			case 2020:l_data=devices_savedata(l_data,(uint16_t)Com_Data.Touch_Hardware_Edition);break;    	 //硬件版本号
			case 2021:l_data=devices_savedata(l_data,(uint16_t)Com_Data.Touch_Software_Edition);break;    	 //软件版本号
			case 2022:l_data=devices_savedata(l_data,(uint16_t)Com_Data.Load_Default_Flag);break;    	     //恢复出厂标识
			case 2023:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.BMS_Type);break;	         // BMS类型选择
			case 2024:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.Balancing_Enable);break;	 // SOC均衡使能控制
			case 2025:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.PCS_Island_Ctrl_Mode);break; // PCS离网启动策略

			case 2049:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Active_Power_Giving);break; 	   //PCC有功控制
			case 2050:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Reactive_Power_Giving);break;   //PCC无功控制

			case 2051:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode);break;//无功调节方式
			case 2052:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor);break;//功率因数
			case 2053:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set);break;//无功功率
			
			case 2069:l_data=devices_savedata(l_data,System_Run_Info.E_Load_Adjust);break;//负载电量校正

			case 2074:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[0].Modubus_Address);break;	//电表0地址
			case 2075:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[1].Modubus_Address);break;	//电表1地址
			case 2076:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[2].Modubus_Address);break;	//电表2地址

			case 2099:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number);break;    //储能变流器总台数
			case 2100:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[0].Modubus_Address);break;    //1#储能变流器地址
			case 2101:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[1].Modubus_Address);break;    //2#储能变流器地址
			case 2102:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[2].Modubus_Address);break;    //3#储能变流器地址
			case 2103:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[3].Modubus_Address);break;    //4#储能变流器地址
			case 2104:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[4].Modubus_Address);break;    //5#储能变流器地址
			case 2105:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[5].Modubus_Address);break;    //6#储能变流器地址

			case 2110:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number);break;    //光伏逆变器总台数
			case 2111:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[0].Modubus_Address);break;    //1#光伏逆变器地址
			case 2112:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[1].Modubus_Address);break;    //2#光伏逆变器地址
			case 2113:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[2].Modubus_Address);break;    //3#光伏逆变器地址
			case 2114:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[3].Modubus_Address);break;    //4#光伏逆变器地址
			case 2115:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[4].Modubus_Address);break;    //5#光伏逆变器地址
			case 2116:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[5].Modubus_Address);break;    //6#光伏逆变器地址
			case 2117:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[6].Modubus_Address);break;    //7#光伏逆变器地址
			case 2118:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[7].Modubus_Address);break;    //8#光伏逆变器地址
			case 2119:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[8].Modubus_Address);break;    //9#光伏逆变器地址
			case 2120:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[9].Modubus_Address);break;    //10#光伏逆变器地址
			case 2121:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[10].Modubus_Address);break;    //11#光伏逆变器地址
			case 2122:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[11].Modubus_Address);break;    //12#光伏逆变器地址
			case 2123:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[12].Modubus_Address);break;    //13#光伏逆变器地址
			case 2124:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[13].Modubus_Address);break;    //14#光伏逆变器地址
			case 2125:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[14].Modubus_Address);break;    //15#光伏逆变器地址
			case 2126:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[15].Modubus_Address);break;    //16#光伏逆变器地址
			case 2127:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[16].Modubus_Address);break;    //17#光伏逆变器地址
			case 2128:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[17].Modubus_Address);break;    //18#光伏逆变器地址
			case 2129:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[18].Modubus_Address);break;    //19#光伏逆变器地址
			case 2130:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[19].Modubus_Address);break;    //20#光伏逆变器地址

			case 2131:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number);break;    //DCDC总台数
			case 2132:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[0].Modubus_Address);break;    //1#DCDC地址
			case 2133:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[1].Modubus_Address);break;    //2#DCDC地址
			case 2134:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[2].Modubus_Address);break;    //3#DCDC地址
			case 2135:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[3].Modubus_Address);break;    //4#DCDC地址
			case 2136:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[4].Modubus_Address);break;    //5#DCDC地址
			case 2137:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[5].Modubus_Address);break;    //6#DCDC地址
			case 2138:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[6].Modubus_Address);break;    //7#DCDC地址
			case 2139:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[7].Modubus_Address);break;    //8#DCDC地址
			case 2140:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[8].Modubus_Address);break;    //9#DCDC地址
			case 2141:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[9].Modubus_Address);break;    //10#DCDC地址
			case 2142:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[10].Modubus_Address);break;    //11#DCDC地址
			case 2143:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[11].Modubus_Address);break;    //12#DCDC地址

			case 2145:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number);break;    // BMS总台数
			case 2146:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[0].Modubus_Address);break;//BMS1地址
			case 2147:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[1].Modubus_Address);break;//BMS2地址
			case 2148:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[2].Modubus_Address);break;//BMS3地址
			case 2149:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[3].Modubus_Address);break;//BMS4地址
			case 2150:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[4].Modubus_Address);break;//BMS5地址
			case 2151:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[5].Modubus_Address);break;//BMS6地址
			case 2152:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[6].Modubus_Address);break;//BMS7地址
			case 2153:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[7].Modubus_Address);break;//BMS8地址

			case 2154:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage);break;        //微网设定电压
			case 2155:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency);break;      //微网设定频率
			case 2156:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V);break;    //恒压模式电压
			case 2157:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I);break;    //恒压模式电流		           

			case 2158:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min);break;    //恒压模式运行电压下限
			case 2159:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max);break;    //恒压模式运行电压上限
			case 2160:l_data=devices_savedata(l_data,EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set);break;		//EMS运行功率设置

			case 2199:l_data=devices_savedata(l_data,EMS_RL_Mode);break;    //控制模式
			
			case 2200:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode);break; //电池电量低节点1输出模式
			case 2201:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode);break; //电池电量低节点2输出模式
			case 2202:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable);break;    //SOC控制使能
			case 2203:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2);break;          //SOC上限2级
			case 2204:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1);break;          //SOC上限1级
			case 2205:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1);break;         //SOC下限1级
			case 2206:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2);break;         //SOC下限2级
			case 2207:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover);break;    //SOC下限1级恢复
			case 2208:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover);break;    //SOC下限2级恢复
			case 2209:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis);break;    //SOC滞环控制
			case 2210:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay);break;    //SOC控制延迟时间
			case 2211:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay);break;    //SOC控制恢复时间

			case 2212:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable);break;    //电池电压控制使能
			case 2213:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2);break;    //电池电压上限2级
			case 2214:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1);break;    //电池电压上限1级
			case 2215:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1);break;    //电池电压下限1级
			case 2216:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2);break;    //电池电压下限2级
			case 2217:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover);break;    //电池电压下限1级恢复
			case 2218:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover);break;    //电池电压下限2级恢复
			case 2219:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis);break;    //电池电压下限2级恢复
			case 2220:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay);break;    //电池电压控制延迟时间
			case 2221:l_data=devices_savedata(l_data,EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay);break;    //电池电压控制恢复时间

			//分时电价设置点表
			//设置0
			case 2225:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Start_time.hour);break;//起始时间0(时)
			case 2226:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Start_time.min);break;//起始时间0(分)
			case 2227:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].End_time.hour);break;//结束时间0(时)
			case 2228:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].End_time.min);break;//结束时间0(分)
			//case 2229:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Elec_state);break;//用电峰谷状态0
			//case 2230:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Price);break;//电价0
			case 2229:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Run_Mode);break;       //
			case 2230:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode);break;//

			//设置1
			case 2231:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Start_time.hour);break;//起始时间1(时)
			case 2232:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Start_time.min);break;//起始时间1(分)
			case 2233:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].End_time.hour);break;//结束时间1(时)
			case 2234:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].End_time.min);break;//结束时间1(分)
			//case 2235:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Elec_state);break;//用电峰谷状态1
			//case 2236:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Price);break;//电价1
			case 2235:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Run_Mode);break;//用电峰谷状态1
			case 2236:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Power_Ctrl_Mode);break;//电价1			
				
			//设置2
			case 2237:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].Start_time.hour);break;//起始时间2(时)
			case 2238:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].Start_time.min);break;//起始时间2(分)
			case 2239:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].End_time.hour);break;//结束时间2(时)
			case 2240:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].End_time.min);break;//结束时间2(分)
			case 2241:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].Run_Mode);break;//用电峰谷状态2
			case 2242:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].Power_Ctrl_Mode);break;//电价2
			
			//设置3
			case 2243:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].Start_time.hour);break;//起始时间3(时)
			case 2244:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].Start_time.min);break;//起始时间3(分)
			case 2245:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].End_time.hour);break;//结束时间3(时)
			case 2246:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].End_time.min);break;//结束时间3(分)
			case 2247:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].Run_Mode);break;//用电峰谷状态3
			case 2248:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].Power_Ctrl_Mode);break;//电价3
			
			//设置4
			case 2249:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].Start_time.hour);break;//起始时间4(时)
			case 2250:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].Start_time.min);break;//起始时间4(分)
			case 2251:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].End_time.hour);break;//结束时间4(时)
			case 2252:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].End_time.min);break;//结束时间4(分)
			case 2253:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].Run_Mode);break;//用电峰谷状态4
			case 2254:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].Power_Ctrl_Mode);break;//电价4
				
			//设置5
			case 2255:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].Start_time.hour);break;//起始时间5(时)
			case 2256:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].Start_time.min);break;//起始时间5(分)
			case 2257:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].End_time.hour);break;//结束时间5(时)
			case 2258:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].End_time.min);break;//结束时间5(分)
			case 2259:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].Run_Mode);break;//用电峰谷状态5
			case 2260:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].Power_Ctrl_Mode);break;//电价5
				
			//设置6
			case 2261:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].Start_time.hour);break;//起始时间6(时)
			case 2262:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].Start_time.min);break;//起始时间6(分)
			case 2263:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].End_time.hour);break;//结束时间6(时)
			case 2264:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].End_time.min);break;//结束时间6(分)
			case 2265:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].Run_Mode);break;//用电峰谷状态6
			case 2266:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].Power_Ctrl_Mode);break;//电价6
				
			//设置7
			case 2267:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].Start_time.hour);break;//起始时间7(时)
			case 2268:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].Start_time.min);break;//起始时间7(分)
			case 2269:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].End_time.hour);break;//结束时间7(时)
			case 2270:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].End_time.min);break;//结束时间7(分)
			case 2271:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].Run_Mode);break;//用电峰谷状态7
			case 2272:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].Power_Ctrl_Mode);break;//电价7
			
			//设置8
			case 2273:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].Start_time.hour);break;//起始时间8(时)
			case 2274:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].Start_time.min);break;//起始时间8(分)
			case 2275:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].End_time.hour);break;//结束时间8(时)
			case 2276:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].End_time.min);break;//结束时间8(分)
			case 2277:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].Run_Mode);break;//用电峰谷状态8
			case 2278:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].Power_Ctrl_Mode);break;//电价8
					
			//设置9
			case 2279:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].Start_time.hour);break;//起始时间9(时)
			case 2280:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].Start_time.min);break;//起始时间9(分)
			case 2281:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].End_time.hour);break;//结束时间9(时)
			case 2282:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].End_time.min);break;//结束时间9(分)
			case 2283:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].Run_Mode);break;//用电峰谷状态9
			case 2284:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].Power_Ctrl_Mode);break;//电价9

			//功率
			case 2285:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Run_Power);break;//运行功率设定0
			case 2286:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Run_Power);break;//运行功率设定1
			case 2287:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].Run_Power);break;//运行功率设定2
			case 2288:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].Run_Power);break;//运行功率设定3
			case 2289:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].Run_Power);break;//运行功率设定4
			case 2290:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].Run_Power);break;//运行功率设定5
			case 2291:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].Run_Power);break;//运行功率设定6
			case 2292:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].Run_Power);break;//运行功率设定7
			case 2293:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].Run_Power);break;//运行功率设定8
			case 2294:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].Run_Power);break;//运行功率设定9
			//END.

			//case 2299:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Baudrate.PCS_Baud);break;    //PCS波特率
			//case 2300:l_data=devices_savedata(l_data,EMS_Info.EMS4x.EMS_Controller_Info.Baudrate.PV_Baud);break;    //PV波特率

            //系统参数设置
			case 2302:l_data=devices_savedata(l_data,System_Info.Modbus_Address);break;    //EMS设备地址
			case 2303:l_data=devices_savedata(l_data,System_Info.Modbus_Port);break;    //端口号
			case 2305:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[0]);break;                 //IP地址
			case 2306:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[1]);break;
			case 2307:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[2]);break;
			case 2308:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.ip[3]);break;
			case 2309:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[0]);break;            //子网掩码
			case 2310:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[1]);break;
			case 2311:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[2]);break;
			case 2312:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.submask[3]);break;
			case 2313:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[0]);break;        //网关
			case 2314:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[1]);break;
			case 2315:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[2]);break;
			case 2316:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.gateway[3]);break;
			
			case 2317:l_data=devices_savedata(l_data,(uint16_t)System_Info.Net.dhcp_flag);break;         //DHCP
			case 2318:l_data=devices_savedata(l_data,(uint16_t)System_Info.IEC104_Address);break;		 //IEC104通讯地址
			case 2319:l_data=devices_savedata(l_data,(uint16_t)System_Info.IEC104_T1);break;			 //IEC104超时参数T1
			case 2320:l_data=devices_savedata(l_data,(uint16_t)System_Info.IEC104_T3);break;			 //IEC104超时参数T3
			case 2321:l_data=devices_savedata(l_data,(uint16_t)System_Info.IEC104_K);break;				 //IEC104参数k
			case 2322:l_data=devices_savedata(l_data,(uint16_t)System_Info.IEC104_W);break;				 //IEC104参数w

			case 2325:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Week);break;//按星期分时0
		    case 2326:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Week);break;//按星期分时1
			case 2327:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].Week);break;//按星期分时2
			case 2328:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].Week);break;//按星期分时3
			case 2329:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].Week);break;//按星期分时4
			case 2330:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].Week);break;//按星期分时5
			case 2331:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].Week);break;//按星期分时6

			case 2336:l_data=devices_savedata(l_data,Balancing_Kp);break;                //KP
			case 2337:l_data=devices_savedata(l_data,wInverterAcVoltDQKp);break;         //KP
			case 2338:l_data=devices_savedata(l_data,wInverterAcVoltDQKi);break;         //KI
			case 2339:l_data=devices_savedata(l_data,wInverterAcVoltDQKp_PVLimit);break; //pv KP
			case 2340:l_data=devices_savedata(l_data,wInverterAcVoltDQKi_PVLimit);break; //pv KI
			case 2341:l_data=devices_savedata(l_data,System_Info.Zero_V_Max);break;      //zero v
			case 2342:l_data=devices_savedata(l_data,Parameter_recovery_kp);break;            //回调比例
			case 2343:l_data=devices_savedata(l_data,Parameter_recovery_dalay_time);break;    //回调延时
			case 2344:l_data=devices_savedata(l_data,Max_anti_reflux_power);break;            //最大逆流功率
			case 2345:l_data=devices_savedata(l_data,Send_Data_To_PV_Broadcast_Enable);break; //光伏广播使能
			
			
			//目标服务器参数
			//服务器0
			case 2349:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[0].ip[0]);break;//IP地址0
			case 2350:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[0].ip[1]);break;//IP地址1
			case 2351:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[0].ip[2]);break;//IP地址2
			case 2352:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[0].ip[3]);break;//IP地址3
			case 2353:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server_Modbus_Port[0]);break;//端口号
			//服务器1
			case 2354:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[1].ip[0]);break;//IP地址0
			case 2355:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[1].ip[1]);break;//IP地址1
			case 2356:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[1].ip[2]);break;//IP地址2
			case 2357:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[1].ip[3]);break;//IP地址3
			case 2358:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server_Modbus_Port[1]);break;//端口号
			//服务器2
			case 2359:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[2].ip[0]);break;//IP地址0
			case 2360:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[2].ip[1]);break;//IP地址1
			case 2361:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[2].ip[2]);break;//IP地址2
			case 2362:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[2].ip[3]);break;//IP地址3
			case 2363:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server_Modbus_Port[2]);break;//端口号
			//服务器3
			case 2364:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[3].ip[0]);break;//IP地址0
			case 2365:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[3].ip[1]);break;//IP地址1
			case 2366:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[3].ip[2]);break;//IP地址2
			case 2367:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[3].ip[3]);break;//IP地址3
			case 2368:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server_Modbus_Port[3]);break;//端口号
			//服务器4
			case 2369:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[4].ip[0]);break;//IP地址0
			case 2370:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[4].ip[1]);break;//IP地址1
			case 2371:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[4].ip[2]);break;//IP地址2
			case 2372:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[4].ip[3]);break;//IP地址3
			case 2373:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server_Modbus_Port[4]);break;//端口号
			//服务器5
			case 2374:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[5].ip[0]);break;//IP地址0
			case 2375:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[5].ip[1]);break;//IP地址1
			case 2376:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[5].ip[2]);break;//IP地址2
			case 2377:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server[5].ip[3]);break;//IP地址3
			case 2378:l_data=devices_savedata(l_data,(uint16_t)System_Info.Server_Modbus_Port[5]);break;//端口号

			//case 2379:l_data=devices_savedata(l_data,(uint16_t)System_Info.Client_Modbus_Port[0]);break;//客户端端口号
			//case 2380:l_data=devices_savedata(l_data,(uint16_t)System_Info.Client_Modbus_Port[1]);break;//客户端端口号
			//case 2381:l_data=devices_savedata(l_data,(uint16_t)System_Info.Client_Modbus_Port[2]);break;//客户端端口号
			//case 2382:l_data=devices_savedata(l_data,(uint16_t)System_Info.Client_Modbus_Port[3]);break;//客户端端口号
			//case 2383:l_data=devices_savedata(l_data,(uint16_t)System_Info.Client_Modbus_Port[4]);break;//客户端端口号
			//case 2384:l_data=devices_savedata(l_data,(uint16_t)System_Info.Client_Modbus_Port[5]);break;//客户端端口号

            //调试参数
			case 3000:l_data=devices_savedata(l_data,(uint16_t)PV_OK_DC_V);break;

			case 3300:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Mgrid_Run_Power);break;//离网运行功率设定0
			case 3301:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[1].Mgrid_Run_Power);break;//离网运行功率设定1
			case 3302:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[2].Mgrid_Run_Power);break;//离网运行功率设定2
			case 3303:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[3].Mgrid_Run_Power);break;//离网运行功率设定3
			case 3304:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[4].Mgrid_Run_Power);break;//离网运行功率设定4
			case 3305:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[5].Mgrid_Run_Power);break;//离网运行功率设定5
			case 3306:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[6].Mgrid_Run_Power);break;//离网运行功率设定6
			case 3307:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[7].Mgrid_Run_Power);break;//离网运行功率设定7
			case 3308:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[8].Mgrid_Run_Power);break;//离网运行功率设定8
			case 3309:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[9].Mgrid_Run_Power);break;//离网运行功率设定9

			case 3320:l_data=devices_savedata(l_data,System_Info.Config.Meter_Position_Enable[0]);break;
			case 3321:l_data=devices_savedata(l_data,System_Info.Config.Meter_Position_Enable[1]);break;
			case 3322:l_data=devices_savedata(l_data,System_Info.Config.Meter_Position_Enable[2]);break;

			case 3400:l_data=devices_savedata(l_data,(uint16_t)EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay);break;
			case 3401:l_data=devices_savedata(l_data,(uint16_t)EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle);break;
			case 3402:l_data=devices_savedata(l_data,System_Info.Config.Ammeter_Protocol_Type);break;
			case 3403:l_data=devices_savedata(l_data,System_Info.Config.Meter_Position_Enable[0]);break;
			case 3404:l_data=devices_savedata(l_data,System_Info.Config.Meter_Position_Enable[1]);break;
			case 3405:l_data=devices_savedata(l_data,System_Info.Config.Meter_Position_Enable[2]);break;

			case 3410:l_data=devices_savedata(l_data,comParam[0].protocol);break;
			case 3411:l_data=devices_savedata(l_data,comParam[1].protocol);break;
			case 3412:l_data=devices_savedata(l_data,comParam[2].protocol);break;
			case 3413:l_data=devices_savedata(l_data,comParam[3].protocol);break;
			case 3414:l_data=devices_savedata(l_data,comParam[4].protocol);break;
			case 3415:l_data=devices_savedata(l_data,comParam[5].protocol);break;

			case 3500:l_data=devices_savedata(l_data, (Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode>=10? \
					Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode:0));break;         //高级功能设置
			case 3501:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power);break;
			case 3502:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power);break;
			case 3503:l_data=devices_savedata(l_data,Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle);break;
			default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
		}
	}
}
void Touch_Get_Data_From_EMS_0x04(uint16_t addr, uint16_t len, uint8_t *data)          //触摸屏处理0x04命令
{
	uint8_t *l_data = data;
	uint16_t i = 0;

	drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

	if ((999 <= addr) && (addr <= 1044))
	{
		memcpy(data, Device_Info[FIRMWARE_VEB1], 30);           //LCD版本
		memcpy(data+30, Device_Info[FIRMWARE_VEB2], 30);		//TOUCH版本
		memcpy(data+60,Device_Info[FIRMWARE_VEB3], 30);			//DSP版本
	}
	else if ((1999 <= addr) && (addr <= 2219))     //运行信息
	{
		for (i = 0; i < len; ++i)
		{
			switch (i + addr)
			{
				case 1999:l_data=devices_savedata(l_data,(uint16_t)Load_Power_Status);break;			   //负载供电方向
				case 2000:l_data=devices_savedata(l_data,(uint16_t)System_Info.Config.EMS_Run_Priority);break; //供电优先级选择
				case 2001:l_data=devices_savedata(l_data,
								(uint16_t)(FAULT?6:(EMGSTOP?9:Run_Status(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status))));break;//工作状态

				case 2004:l_data=devices_savedata(l_data,(uint16_t)((Load_Power_Status<<1)+System_Info.Config.EMS_Run_Priority));break;//供电运行状态

				case 2009:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Run_Time));break;    //总运行时间
				case 2010:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Run_Time));break;
				case 2011:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_MG_Run_Time));break;    //总微网运行时间
				case 2012:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_MG_Run_Time));break;
				case 2013:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Grid_Run_Time));break;    //总电网运行时间
				case 2014:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Grid_Run_Time));break;				
				case 2015:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Charge_Time));break;    //总充电运行时间
				case 2016:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Charge_Time));break;				
				case 2017:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Discharge_Time));break;    //总放电运行时间
				case 2018:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Discharge_Time));break;				
				case 2019:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_PV_Run_Time));break;    //总光伏运行时间
				case 2020:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_PV_Run_Time));break;				
				case 2021:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Load_Supply_Time));break;    //总负载供电运行时间
				case 2022:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Load_Supply_Time));break;				
				case 2023:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Fault_Time));break;    //总故障时间
				case 2024:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Fault_Time));break;				
				case 2025:l_data=devices_savedata(l_data,System_Run_Info.Day_Run_Time);break;    //日运行时间		
				case 2026:l_data=devices_savedata(l_data,System_Run_Info.Day_MG_Run_Time);break;    //日微网运行时间		
				case 2027:l_data=devices_savedata(l_data,System_Run_Info.Day_Grid_Run_Time);break;    //日电网运行时间		
				case 2028:l_data=devices_savedata(l_data,System_Run_Info.Day_Charge_Time);break;    //日充电时间		
				case 2029:l_data=devices_savedata(l_data,System_Run_Info.Day_Discharge_Time);break;    //日放电时间		
				case 2030:l_data=devices_savedata(l_data,System_Run_Info.Day_PV_Run_Time);break;    //日光伏运行时间		
				case 2031:l_data=devices_savedata(l_data,System_Run_Info.Day_Load_Supply_Time);break;    //日负载供电运行时间		
				case 2032:l_data=devices_savedata(l_data,System_Run_Info.Day_Fault_Time);break;    //日故障时间		
				case 2033:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Charge_Energy));break;    //电池总充电量
				case 2034:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Charge_Energy));break;
				case 2035:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Discharge_Energy));break;    //电池总放电量
				case 2036:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Discharge_Energy));break;
				case 2037:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_PV_Energy));break;    //光伏总发电量
				case 2038:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_PV_Energy));break;
				case 2039:l_data=devices_savedata(l_data,LOWORD(System_Run_Info.Total_Load_Energy));break;    //负载总用电量
				case 2040:l_data=devices_savedata(l_data,HIWORD(System_Run_Info.Total_Load_Energy));break;
				case 2041:l_data=devices_savedata(l_data,System_Run_Info.Day_Charge_Energy);break;    //电池日充电量
				case 2042:l_data=devices_savedata(l_data,System_Run_Info.Day_Discharge_Energy);break; //电池日放电量	
				case 2043:l_data=devices_savedata(l_data,System_Run_Info.Day_PV_Energy);break;        //PV总充电量
				case 2044:l_data=devices_savedata(l_data,System_Run_Info.Day_Load_Energy);break;      //Load总放电量	

				case 2049:l_data=devices_savedata(l_data,(Return_Max_Count(System_Info.EMS_PCS_Com_Count,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)+7)>>3);break;   //储能变流器信号强度
				case 2050:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num);break;		//储能变流器在线台数
				case 2053:l_data=devices_savedata(l_data,System_Run_Info.PCS_DC_Voltage);break;		//储能变流器直流电压
				case 2054:l_data=devices_savedata(l_data,System_Run_Info.PCS_AC_Voltage);break;		//储能变流器电网电压
				case 2055:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power);break;		//储能变流器有功功率
				case 2056:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power);break;		//储能变流器无功功率
				case 2057:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy));break;		//储能变流器累计放电
				case 2058:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy));break;	
				case 2059:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy));break;		//储能变流器累计充电
				case 2060:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy));break;

				case 2069:l_data=devices_savedata(l_data,(Return_Max_Count(System_Info.EMS_PV_Com_Count,EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number)+7)>>3);break;   //光伏逆变器信号强度
				case 2070:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num);break;		//光伏逆变器在线台数
				case 2073:l_data=devices_savedata(l_data,System_Run_Info.PV_DC_Voltage);break;		//光伏逆变器直流电压
				case 2074:l_data=devices_savedata(l_data,System_Run_Info.PV_AC_Voltage);break;		//光伏逆变器电网电压
				case 2075:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power);break;		//光伏逆变器有功功率
				case 2076:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power);break;		//光伏逆变器无功功率
				case 2077:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy));break;		//光伏逆变器累计发电
				case 2078:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy));break;		//光伏逆变器累计发电

				case 2091:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);break;		//负载有功功率
				case 2092:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power);break;	//负载无功功率

				//显示PCS信息				
				case 2093:l_data=devices_savedata(l_data,Disp_PCS_Num+1);break;		//当前显示储能变流器编号
				case 2094:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.DC_Voltage);break;		//当前显示储能变流器直流电压
				case 2095:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.DC_Current);break;		//当前显示储能变流器直流电流	
				case 2096:l_data=devices_savedata(l_data,(uint16_t)PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.DC_Power);break;//当前显示储能变流器直流功率
				case 2097:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.VAB);break;		//当前显示储能变流器VAB
				case 2098:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.VBC);break;		//当前显示储能变流器VBC
				case 2099:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.VCA);break;		//当前显示储能变流器VCA
				case 2100:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Grid_Frequency);break;//当前显示储能变流器电网频率
				case 2101:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.IA);break;		//当前显示储能变流器IA
				case 2102:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.IB);break;		//当前显示储能变流器IB
				case 2103:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.IC);break;		//当前显示储能变流器IC
				case 2104:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Active_Power);break;		//当前显示储能变流器有功功率
				case 2105:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Reactive_Power);break;	//当前显示储能变流器无功功率
				case 2106:l_data=devices_savedata(l_data,PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Power_Factor);break;		//当前显示储能变流器功率因数
				case 2107:l_data=devices_savedata(l_data,LOWORD(PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Total_Charge_Energe));break;	//当前显示储能变流器累计充电
				case 2108:l_data=devices_savedata(l_data,HIWORD(PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Total_Charge_Energe));break;	//当前显示储能变流器累计充电
				case 2109:l_data=devices_savedata(l_data,LOWORD(PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Total_Discharge_Energe));break;	//当前显示储能变流器累计放电
				case 2110:l_data=devices_savedata(l_data,HIWORD(PCS_Info[Disp_PCS_Num].PCS3x.Rmeasure.Total_Discharge_Energe));break;	//当前显示储能变流器累计放电
				case 2111:l_data=devices_savedata(l_data,(PCS_NO_FAULT(Disp_PCS_Num))?(Run_Status(PCS_Info[Disp_PCS_Num].PCS3x.RCommunication.Run_Status)):(17));break;		//当前显示储能变流器工作状态

                //显示PV信息
				case 2119:l_data=devices_savedata(l_data,Disp_PV_Num+1);break;		//当前显示光伏逆变器编号
				case 2120:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.DC_Voltage1);break;		//当前显示光伏逆变器直流电压1
				case 2121:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.DC_Current1);break;		//当前显示光伏逆变器直流电流1
				case 2122:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.DC_Voltage2);break;		//当前显示光伏逆变器直流电压2
				case 2123:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.DC_Current2);break;		//当前显示光伏逆变器直流电流2
				case 2124:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.VA);break;		        //当前显示光伏逆变器电网电压VA
				case 2125:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.VB);break;		        //当前显示光伏逆变器电网电压VB
				case 2126:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.VC);break;		        //当前显示光伏逆变器电网电压VC
				case 2127:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.Frequency);break;		//当前显示光伏逆变器电网频率
				case 2128:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.Total_Active_Power);break;	//当前显示光伏逆变器有功功率
				case 2129:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.Reactive_Power);break;		//当前显示光伏逆变器无功功率
				case 2130:l_data=devices_savedata(l_data,PV_Info[Disp_PV_Num].PV3x.RunParameter.Power_Factor);break;		//当前显示光伏逆变器功率因数
				case 2131:l_data=devices_savedata(l_data,LOWORD(PV_Info[Disp_PV_Num].PV3x.RunParameter.Total_Generation));break; //当前显示光伏逆变器累计发电
				case 2132:l_data=devices_savedata(l_data,HIWORD(PV_Info[Disp_PV_Num].PV3x.RunParameter.Total_Generation));break;		
				case 2133:l_data=devices_savedata(l_data,(uint16_t)PV_Info[Disp_PV_Num].PV3x.RunParameter.Inverter_Status);break;//当前显示光伏逆变器工作状态
				case 2134:l_data=devices_savedata(l_data,(uint16_t)PV_Info[Disp_PV_Num].PV4x.ParaSet.Power_ONOFF);break;		 //当前显示光伏逆变器开关机状态
				case 2135:l_data=devices_savedata(l_data,(uint16_t)PV_Info[Disp_PV_Num].PV4x.ParaSet.PowerLimit_Switch);break;	 //当前显示光伏逆变器限功率开关状态
				case 2136:l_data=devices_savedata(l_data,(uint16_t)PV_Info[Disp_PV_Num].PV4x.ParaSet.PowerLimit_Set);break;		 //当前显示光伏逆变器限功率值
				case 2137:l_data=devices_savedata(l_data,(uint16_t)PV_Com_Err[Disp_PV_Num]);break;		//光伏逆变器通讯状态

				case 2139:l_data=devices_savedata(l_data,(uint16_t)(Grid_Exist?1:0));break;		                       //电网状态
				case 2140:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max);break;   //微网电压
				case 2141:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency);break;		   //微网频率
				case 2142:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage);break;		   //负载电压
				case 2143:l_data=devices_savedata(l_data,EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Frequency);break;	   //负载频率
				
				case 2149:l_data=devices_savedata(l_data,(uint16_t)((System_Info.Config.Power_Distribution_Enable==ENABLE)?DIN(Grid_Contacter ):( GRID_RUNNING                 ?1:0)));break;	  //电网侧接触器状态
				case 2150:l_data=devices_savedata(l_data,(uint16_t)((System_Info.Config.Power_Distribution_Enable==ENABLE)?DIN(MGrid_Contacter):((ISLAND_RUNNING||GRID_RUNNING)?1:0)));break;	  //微网侧接触器状态
				case 2151:l_data=devices_savedata(l_data,(uint16_t)Grid_Pic_Direction);break;	  //电网图片滚动方向
				case 2152:l_data=devices_savedata(l_data,(uint16_t)PCS_Pic_Direction);break;	  //储能变流器图片滚动方向
				case 2153:l_data=devices_savedata(l_data,(uint16_t)PV_Pic_Direction);break;		  //光伏逆变器图片滚动方向
				case 2154:l_data=devices_savedata(l_data,(uint16_t)Load_Pic_Direction);break;	  //负载图片滚动方向
				case 2155:l_data=devices_savedata(l_data,(uint16_t)(!DC_BUS_SYS));break;		  //是否交流母线系统？
				case 2156:l_data=devices_savedata(l_data,(uint16_t)MGrid_Pic_Direction);break;	  //微网图片流动方向			

                case 2157:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[0].Run_Mode!=ISLAND_MODE));break;  //时间1#当前是否离网模式
                case 2158:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[1].Run_Mode!=ISLAND_MODE));break;  //时间2#当前是否离网模式
                case 2159:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[2].Run_Mode!=ISLAND_MODE));break;  //时间3#当前是否离网模式
                case 2160:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[3].Run_Mode!=ISLAND_MODE));break;  //时间4#当前是否离网模式
                case 2161:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[4].Run_Mode!=ISLAND_MODE));break;  //时间5#当前是否离网模式
                case 2162:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[5].Run_Mode!=ISLAND_MODE));break;  //时间6#当前是否离网模式
                case 2163:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[6].Run_Mode!=ISLAND_MODE));break;  //时间7#当前是否离网模式
                case 2164:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[7].Run_Mode!=ISLAND_MODE));break;  //时间8#当前是否离网模式
                case 2165:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[8].Run_Mode!=ISLAND_MODE));break;  //时间9#当前是否离网模式
                case 2166:l_data=devices_savedata(l_data,(uint16_t)(Time_Div_Info.Time_Div_Set_Info[9].Run_Mode!=ISLAND_MODE));break;  //时间10#当前是否离网模式

				case 2199:l_data=devices_savedata(l_data,(uint16_t)IO_Set_Touch_Req);break;		  //计量板对时请求
				case 2200:l_data=devices_savedata(l_data,(uint16_t)Touch_Set_Time_Flag);break;	  //触摸屏对时计量板

				case 2202:l_data=devices_savedata(l_data,(uint16_t)Allow_Load_Default);break;	  //允许恢复出厂设置
				case 2203:l_data=devices_savedata(l_data,TOUCH_CAN_SET_DATA);break;		          //是否可本地设置
				case 2204:l_data=devices_savedata(l_data,(uint16_t)IOD_Poweron);break;	          //计量板上电状态

				case 2207:l_data=devices_savedata(l_data,(uint16_t)!SYS_NO_FAULT);break;	                  //系统故障
				case 2208:l_data=devices_savedata(l_data,(uint16_t)SYS_ALARM);break;	                      //系统告警
                case 2209:l_data=devices_savedata(l_data,(uint16_t)( System_Info.Config.BMS_Type!=0));break;  //有BMS
                case 2210:l_data=devices_savedata(l_data,(uint16_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode);break;   //当前并离网状态
                case 2211:l_data=devices_savedata(l_data,(uint16_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode);break;  //当前功率控制模式
                case 2212:l_data=devices_savedata(l_data,(uint16_t)Time_Div_Info.Period);break;               //当前所处时间段
				case 2213:l_data=devices_savedata(l_data,EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max);break; // 系统平均SOC，可用SOC平均值
				case 2214:l_data=devices_savedata(l_data,(uint16_t)(FAULT?1:(STOP?1:(EMGSTOP?1:(INIT?1:0)))));break;    // 可以切换配置标志
				//case 2215:l_data=devices_savedata(l_data,(uint16_t)((VSG_Info.Grid_voltage_Vab+VSG_Info.Grid_voltage_Vbc)/2));break; // 电网电压
				case 2215:l_data=devices_savedata(l_data,(uint16_t)((Meter_Info[0].VAB+Meter_Info[0].VBC+Meter_Info[0].VCA)/3));break; // 电网电压
				case 2216:l_data=devices_savedata(l_data,LOWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power));break; // 电网功率
				case 2217:l_data=devices_savedata(l_data,HIWORD(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power));break; // 电网功率

				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
	}
	else if((addr>=2220)&&(addr<=2750))//DCDC控制器单台信息
    {
        if(System_Info.Config.DCDC_Type==1) //单向
        {
            for (i=0;i<len;++i)
            {
                switch(i+addr)
                {
                    case 2220:l_data=devices_savedata(l_data,Disp_DCDC_Num+1);break;        //当前显示光伏逆变器编号
                    case 2221:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Array_V);break;//阵列电压1
                    case 2222:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Array_I);break;//阵列电流1
                    case 2223:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Bat_V);break;//电池电压1
                    case 2224:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Charge_I);break;//充电电流1
                    case 2225:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Charge_P));break;//充电功率1
                    case 2226:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Charge_P));break;//充电功率1
                    case 2227:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_In_Temp);break;//机内温度1
                    case 2228:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Bat_Temp);break;//电池温度1    
                    case 2229:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?6:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_1.Module_Dev_Status));break; //设备状态1                        
                    case 2230:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Dev_Fault_Status);break;//故障状态1
                    case 2231:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Output_E_Day);break;//日发电量
                    case 2232:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Output_E_Mon));break;//月发电量
                    case 2233:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Output_E_Mon));break;//月发电量
                    case 2234:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Output_E_Total));break;//发电总量
                    case 2235:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Output_E_Total));break;//发电总量
                    case 2236:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2237:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2238:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Run_Min_Day);break;//日运行分钟
                    case 2239:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module_Run_Hour_Mon);break;//月运行时数
                    case 2240:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Run_Hour_Total));break;//总运行时数
                    case 2241:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_1.Module_Run_Hour_Total));break;//总运行时数
                    
                    case 2251:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Array_V);break;//阵列电压2
                    case 2252:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Array_I);break;//阵列电流2
                    case 2253:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Bat_V);break;//电池电压2
                    case 2254:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Charge_I);break;//充电电流2
                    case 2255:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Charge_P));break;//充电功率2
                    case 2256:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Charge_P));break;//充电功率2
                    case 2257:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_In_Temp);break;//机内温度2
                    case 2258:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Bat_Temp);break;//电池温度2
                    case 2259:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?6:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_2.Module_Dev_Status));break; //设备状态1
                    case 2260:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Dev_Fault_Status);break;//故障状态2
                    case 2261:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Output_E_Day);break;//日发电量
                    case 2262:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Output_E_Mon));break;//月发电量
                    case 2263:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Output_E_Mon));break;//月发电量
                    case 2264:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Output_E_Total));break;//发电总量
                    case 2265:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Output_E_Total));break;//发电总量
                    case 2266:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2267:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2268:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Run_Min_Day);break;//日运行分钟
                    case 2269:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module_Run_Hour_Mon);break;//月运行时数
                    case 2270:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Run_Hour_Total));break;//总运行时数
                    case 2271:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_2.Module_Run_Hour_Total));break;//总运行时数
                    
                    case 2281:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Array_V);break;//阵列电压3
                    case 2282:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Array_I);break;//阵列电流3
                    case 2283:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Bat_V);break;//电池电压3
                    case 2284:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Charge_I);break;//充电电流3
                    case 2285:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Charge_P));break;//充电功率3
                    case 2286:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Charge_P));break;//充电功率3
                    case 2287:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_In_Temp);break;//机内温度3
                    case 2288:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Bat_Temp);break;//电池温度3
                    case 2289:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?6:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_3.Module_Dev_Status));break; //设备状态3                        
                    case 2290:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Dev_Fault_Status);break;//故障状态3
                    case 2291:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Output_E_Day);break;//日发电量
                    case 2292:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Output_E_Mon));break;//月发电量
                    case 2293:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Output_E_Mon));break;//月发电量
                    case 2294:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Output_E_Total));break;//发电总量
                    case 2295:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Output_E_Total));break;//发电总量
                    case 2296:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2297:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2298:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Run_Min_Day);break;//日运行分钟
                    case 2299:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module_Run_Hour_Mon);break;//月运行时数
                    case 2300:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Run_Hour_Total));break;//总运行时数
                    case 2301:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_3.Module_Run_Hour_Total));break;//总运行时数

                    case 2311:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Array_V);break;//阵列电压4
                    case 2312:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Array_I);break;//阵列电流4
                    case 2313:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Bat_V);break;//电池电压4
                    case 2314:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Charge_I);break;//充电电流4
                    case 2315:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Charge_P));break;//充电功率4
                    case 2316:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Charge_P));break;//充电功率4
                    case 2317:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_In_Temp);break;//机内温度4
                    case 2318:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Bat_Temp);break;//电池温度4
                    case 2319:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?6:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_4.Module_Dev_Status));break; //设备状态4                        
                    case 2320:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Dev_Fault_Status);break;//故障状态4
                    case 2321:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Output_E_Day);break;//日发电量
                    case 2322:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Output_E_Mon));break;//月发电量
                    case 2323:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Output_E_Mon));break;//月发电量
                    case 2324:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Output_E_Total));break;//发电总量
                    case 2325:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Output_E_Total));break;//发电总量
                    case 2326:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2327:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_CO2_Reduced));break;//二氧化碳减排
                    case 2328:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Run_Min_Day);break;//日运行分钟
                    case 2329:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module_Run_Hour_Mon);break;//月运行时数
                    case 2330:l_data=devices_savedata(l_data,LOWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Run_Hour_Total));break;//总运行时数
                    case 2331:l_data=devices_savedata(l_data,HIWORD(DCDC_Info[Disp_DCDC_Num].Module_4.Module_Run_Hour_Total));break;//总运行时数

                    default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
                }
            }
        }
        else if (System_Info.Config.DCDC_Type==2) //双向
        {
            for (i=0;i<len;++i)
            {
                switch(i+addr)
                {
                    case 2220:l_data=devices_savedata(l_data,(Disp_DCDC_Num+1));break;        //当前显示光伏逆变器编号
                    case 2221:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Bus_Bar_V);break;//母线电压
                    case 2222:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Bus_Bar_I);break;//母线电流
                    case 2223:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Battery_V);break;//蓄电池电压
                    case 2224:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Battery_I);break;//蓄电池电流
                    case 2225:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module1_T);break;//模组1温度
                    case 2226:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_1.Module2_T);break;//模组2温度
                    case 2227:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?0:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_1.Device_Status));break; //设备状态1
                    case 2228:l_data=devices_savedata(l_data,(uint16_t)((DCDC_Info[Disp_DCDC_Num].Module_1.Fault_Status==0x00)?0:1));break; //故障

                    case 2251:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Bus_Bar_V);break;//母线电压
                    case 2252:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Bus_Bar_I);break;//母线电流
                    case 2253:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Battery_V);break;//蓄电池电压
                    case 2254:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Battery_I);break;//蓄电池电流
                    case 2255:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module1_T);break;//模组1温度
                    case 2256:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_2.Module2_T);break;//模组2温度
                    case 2257:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?0:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_2.Device_Status));break; //设备状态2
                    case 2258:l_data=devices_savedata(l_data,(uint16_t)((DCDC_Info[Disp_DCDC_Num].Module_2.Fault_Status==0x00)?0:1));break; //故障
                        
                    case 2281:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Bus_Bar_V);break;//母线电压
                    case 2282:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Bus_Bar_I);break;//母线电流
                    case 2283:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Battery_V);break;//蓄电池电压
                    case 2284:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Battery_I);break;//蓄电池电流
                    case 2285:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module1_T);break;//模组1温度
                    case 2286:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_3.Module2_T);break;//模组2温度
                    case 2287:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?0:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_3.Device_Status));break; //设备状态3
                    case 2288:l_data=devices_savedata(l_data,(uint16_t)((DCDC_Info[Disp_DCDC_Num].Module_3.Fault_Status==0x00)?0:1));break; //故障

                    case 2311:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Bus_Bar_V);break;//母线电压
                    case 2312:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Bus_Bar_I);break;//母线电流
                    case 2313:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Battery_V);break;//蓄电池电压
                    case 2314:l_data=devices_savedata(l_data,(uint16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Battery_I);break;//蓄电池电流
                    case 2315:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module1_T);break;//模组1温度
                    case 2316:l_data=devices_savedata(l_data,(int16_t)DCDC_Info[Disp_DCDC_Num].Module_4.Module2_T);break;//模组2温度
                    case 2317:l_data=devices_savedata(l_data,(uint16_t)(DCDC_Com_Err[Disp_DCDC_Num]?0:
                                                                        DCDC_Info[Disp_DCDC_Num].Module_4.Device_Status));break; //设备状态4
                    case 2318:l_data=devices_savedata(l_data,(uint16_t)((DCDC_Info[Disp_DCDC_Num].Module_4.Fault_Status==0x00)?0:1));break; //故障
                        
                    default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
                }
            }
        }
    }
	
    #if 0
	/*else if((2299 <= addr) && (addr <= 2372))   //充电功率曲线
	{
		for(i=2;i<74;i++)	  
		{	 	             
			SetU16(Touch_Data.Touch_Charge_Power_Curve+i*2,Touch_Data.Touch_Charge_Power_Curve_Temp[i]) ;//72个点  
		}	
		for(i=0;i<len*2;i++)
		{		   
			data[i]=Touch_Data.Touch_Charge_Power_Curve[(addr-2249)*2+i];
		}
	}
	else if((2399 <= addr) && (addr <= 2472))   //放电功率曲线
	{
		for(i=2;i<74;i++)
		{	 	             
			SetU16(Touch_Data.Touch_Discharge_Power_Curve+i*2,Touch_Data.Touch_Discharge_Power_Curve_Temp[i]) ;//72个点  
		}	
		for(i=0;i<len*2;i++)
		{		   
			data[i]=Touch_Data.Touch_Discharge_Power_Curve[(addr-2349)*2+i];
		}
	}*/
    /*else if((2701 <= addr) && (addr <= 2750))   //充放电电量柱
     {
    	  for(i=0;i<24;i++)
    	  {	
    		 SetU16(Touch_Data.Touch_Elec_Column +  i     * 2, (uint16_t)(Touch_Data.Touch_Day_Charge_Elec_Column[i]/Percent_Factor/360000.0));//充电电量柱
    		 SetU16(Touch_Data.Touch_Elec_Column + (i+26) * 2, (uint16_t)(Touch_Data.Touch_Day_Discharge_Elec_Column[i]/Percent_Factor/360000.0));//放电电量柱
    	  }
    	 for(i=0;i<len*2;i++)
    	 {
    		  data[i]=Touch_Data.Touch_Elec_Column[(addr-2701)*2+i];
    	 }
     }*/     
    #endif
 	 
	else if((2801 <= addr) && (addr <= 2900))   //历史事件
	{
		Touch_Load_History_Event_Record();

		for(i=0;i<len*2;i++)
		{
			data[i]=Touch_Data.Touch_History_Event[(addr-2801)*2+i];
		}
	}
	else if((2901 <= addr) && (addr <= 2948))   //历史故障
	{
		Touch_Load_History_Fault_Record();

		for(i=0;i<len*2;i++)
		{
			l_data[i]=Touch_Data.Touch_History_Fault[(addr-2901)*2+i];
		}
	}
	else if((2949 <= addr) && (addr <= 3098))
	{
		for (i=0;i<len;++i)
    	{
        	switch(i+addr)
        	{
        		case 2949:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min);break;	//微网电压下限
        		case 2950:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max);break;	//微网电压上限
        		case 2951:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min);break;//微网频率下限
        		case 2952:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max);break;//微网频率上限
        		case 2953:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Min);break;	//储能变流器台数下限
        		case 2954:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Max);break;	//储能变流器台数上限
        		case 2955:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Min);break;	//光伏逆变器台数下限
        		case 2956:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Max);break;	//光伏逆变器台数上限
        		case 2957:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min);break;	//恒压模式电压下限
        		case 2958:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max);break;	//恒压模式电压上限
        		case 2959:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Min);break;	//恒压模式电流下限
        		case 2960:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Max);break;	//恒压模式电流上限
				case 2961:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Min);break;	    //BMS台数下限
				case 2962:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Max);break;	    //BMS台数上限
				case 2963:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Min);break;	//DCDC台数下限
				case 2964:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Max);break;	//DCDC台数上限

        		case 2967:l_data=devices_savedata(l_data,-(int16_t)((float)System_Info.Config.System_Power_Set*1.1));break;	//EMS并网运行功率设置下限
        		case 2968:l_data=devices_savedata(l_data, (int16_t)((float)System_Info.Config.System_Power_Set*1.1));break;	//EMS并网运行功率设置上限
        		case 2969:l_data=devices_savedata(l_data, 0);break;	//EMS削锋填谷运行功率设置下限
        		case 2970:l_data=devices_savedata(l_data, (int16_t)((float)System_Info.Config.System_Power_Set*1.1));break;	//EMS削锋填谷运行功率设置上限
        		
        		case 2975:l_data=devices_savedata(l_data,-P_Range[System_Info.Config.PCS_Type].Power_Factor.Max);break;	//功率因数下限
        		case 2976:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].Power_Factor.Max);break;	//功率因数上限
        		case 2977:l_data=devices_savedata(l_data,-(int16_t)((float)System_Info.Config.System_Power_Set*0.3));break;	//无功功率下限
        		case 2978:l_data=devices_savedata(l_data, (int16_t)((float)System_Info.Config.System_Power_Set*0.3));break;	//无功功率上限
        		
        		case 2999:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Min);break;	//SOC上限2级下限
        		case 3000:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Max);break;	//SOC上限2级上限
        		case 3001:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Min);break;	//SOC上限1级下限
        		case 3002:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Max);break;	//SOC上限1级上限
        		case 3003:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Min);break;	//SOC下限1级下限
        		case 3004:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Max);break;	//SOC下限1级上限
        		case 3005:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Min);break;	//SOC下限2级下限
        		case 3006:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Max);break;	//SOC下限2级上限

				case 3007:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Min);break;//SOC下限1恢复下限
				case 3008:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Max);break;//SOC下限1恢复上限
				case 3009:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Min);break;//SOC下限2恢复下限
				case 3010:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Max);break;//SOC下限2恢复上限

				case 3013:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Min);break;//SOC控制延迟时间下限
				case 3014:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Max);break;//SOC控制延迟时间上限
				case 3015:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Min);break;//SOC控制恢复时间下限
				case 3016:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Max);break;//SOC控制恢复时间上限

				case 3017:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Min);break;//电池电压上限2级下限
				case 3018:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Max);break;//电池电压上限2级上限
				case 3019:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Min);break;//电池电压上限1级下限
				case 3020:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Max);break;//电池电压上限1级上限
				case 3021:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min);break;//电池电压下限1级下限
				case 3022:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max);break;//电池电压下限1级上限
				case 3023:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Min);break;//电池电压下限2级下限
				case 3024:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Max);break;//电池电压下限2级上限

				case 3025:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Min);break;//电池电压下限1恢复下限
				case 3026:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Max);break;//电池电压下限1恢复上限
				case 3027:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Min);break;//电池电压下限2恢复下限
				case 3028:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Max);break;//电池电压下限2恢复上限

				case 3031:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Min);break;//电池电压控制延迟时间下限
				case 3032:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Max);break;//电池电压控制延迟时间上限
				case 3033:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Min);break;//电池电压控制恢复时间下限
				case 3034:l_data=devices_savedata(l_data,P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Max);break;//电池电压控制恢复时间上限

				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
	}
	else if((3100 <= addr) && (addr <= 3299))
	{
		for (i=0;i<len;++i)
		{
			switch(i+addr)
			{
				//BMS信息
				case 3199:l_data=devices_savedata(l_data,Disp_BMS_Num+1);break;//当前显示BMS编号
				case 3200:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Sys_Voltage);break;
				case 3201:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Sys_Current);break;
				case 3202:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Sys_SOC);break;
				case 3203:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Sys_SOH);break;
				case 3204:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Single_V_Min);break;
				case 3205:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Single_V_Max);break;
				case 3206:l_data=devices_savedata(l_data,(int16_t)BMS_Info[Disp_BMS_Num].Single_T_Min);break;
				case 3207:l_data=devices_savedata(l_data,(int16_t)BMS_Info[Disp_BMS_Num].Single_T_Max);break;
				//case 3208:l_data=devices_savedata(l_data,(uint16_t)BMS_Info.Room_Temp1);break;
				//case 3209:l_data=devices_savedata(l_data,(uint16_t)BMS_Info.Room_Temp2);break;
				case 3209:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Rack_Fault_Word);break;
				case 3210:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Rack_Alarm_Word);break;
				case 3211:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Sys_Alarm_Word);break;
				case 3212:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Sys_Mode);break;
				case 3213:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Discharge_Limit);break;
				case 3214:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Charge_Limit);break;
				case 3215:l_data=devices_savedata(l_data,(uint16_t)BMS_Info[Disp_BMS_Num].Num_Of_Rack_In_Service);break;
				case 3216:l_data=devices_savedata(l_data,(uint16_t)BMS_Info[Disp_BMS_Num].Num_Of_Rack_In_System);break;
				case 3217:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Service_Voltage);break;
				case 3218:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Service_SOC);break;
				case 3219:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].DC_Contactor_P_Status);break;
				case 3220:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].DC_Contactor_N_Status);break;
				case 3221:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Fan_Operating_Status);break;
				case 3222:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Total_G_Charge_Elec);break;
				case 3223:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Total_M_Charge_Elec);break;
				case 3224:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Total_K_Charge_Elec);break;
				case 3225:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Total_G_Discharge_Elec);break;
				case 3226:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Total_M_Discharge_Elec);break;
				case 3227:l_data=devices_savedata(l_data,BMS_Info[Disp_BMS_Num].Total_K_Discharge_Elec);break;

				case 3230:l_data=devices_savedata(l_data,(uint16_t)BMS_FAULT_68AH(Disp_BMS_Num));break;//68ah系统故障
				case 3231:l_data=devices_savedata(l_data,(uint16_t)BMS_ALARM_68AH(Disp_BMS_Num));break;//68ah系统告警

				case 3232:l_data=devices_savedata(l_data,(uint16_t)BMS_FAULT_94AH(Disp_BMS_Num));break;//94ah系统故障
				case 3233:l_data=devices_savedata(l_data,(uint16_t)BMS_ALARM_94AH(Disp_BMS_Num));break;//94ah系统告警

				default:l_data[0]=UINT8_MAX;l_data[1]=UINT8_MAX;l_data+=2;break;
			}
		}
	}
	else
	{
		for(i=0;i<len*2;i++)
		{
			l_data[i]=UINT8_MAX;
		}
	}
}
void Touch_Set_Data_To_EMS_0x10(uint16_t start_address, uint8_t len, uint8_t *data)   //触摸屏处理0x10命令
{
	uint16_t INT32_Val = 0, addr = 0;
	//static uint16_t temp1=0;

	INT32_Val = GetU16(data);
	uint8_t Address_used = 0;

	for (uint8_t i = 0; i < len; i++)
	{
		addr = start_address + i;
		INT32_Val = GetU16(data + i * 2);
		if(addr >= 1999 && addr <= 2099)
		{
			switch (addr)
			{
				case 1999: //系统时钟：秒
					Touch_Write_Time_Flag = true;
					Touch_Write_Time[5] = INT32_Val;//((uint8_t) INT32_Val);
					break;
				case 2000: //系统时钟：分
					Touch_Write_Time_Flag = true;
					Touch_Write_Time[4] = INT32_Val;//((uint8_t) INT32_Val);
					break;
				case 2001: //系统时钟：时
					Touch_Write_Time_Flag = true;
					Touch_Write_Time[3] = INT32_Val;//((uint8_t) INT32_Val);
					break;
				case 2002: //系统时钟：日
					Touch_Write_Time_Flag = true;
					Touch_Write_Time[2] = INT32_Val;//((uint8_t) INT32_Val);
					break;
				case 2003: //系统时钟：月
					Touch_Write_Time_Flag = true;
					Touch_Write_Time[1] = INT32_Val;//((uint8_t) INT32_Val);
					break;
				case 2004: //系统时钟：年
					Touch_Write_Time_Flag = true;
					Touch_Write_Time[0] = (uint8_t) (INT32_Val - 2000);//((uint8_t) (INT32_Val - 2000));
					break;

					//------------------------------------------------------------------------------
					//系统配置参数设置
				case 2005: //运行模式
					if (TOUCH_CAN_SET_DATA)
					{
						if (INT32_Val == 0 || INT32_Val == 1)
						{
							System_Info.Config.EMS_Run_Priority = (uint8_t) INT32_Val;
							write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
						}
					}
					break;
				case 2006: //运行控制
					if (TOUCH_CAN_SET_DATA)
					{
						if (INT32_Val == START_COMMAND)
						{
							Sys_Ctrl.Sys_Start = true;
						}
						if (INT32_Val == STOP_COMMAND)
						{
							Sys_Ctrl.Manual_Stop = true;
						}
					}
					break;
				case 2007: //启动类型
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if (TOUCH_CAN_SET_DATA)
						{
							if (INT32_Val == ACTIVE_MODE || INT32_Val == REACTIVE_MODE)
							{
								System_Info.Config.EMS_Start_Type = (uint8_t) INT32_Val;
								write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
							}
						}
					}
					break;
				case 2008: //母线类型
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if ((INT32_Val == 0) || (INT32_Val == 1))
						{
							System_Info.Config.Bus_Type = (uint8_t) INT32_Val;
							write_eeprom(BUS_TYPE_ADDRESS, &System_Info.Config.Bus_Type, 1);
						}
					}
					break;
				case 2009: //并网类型选择
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if (INT32_Val == 0 || INT32_Val == 1)
						{
							Grid_Connect_Type = (uint8_t) INT32_Val;
							write_eeprom(GRID_CONNECT_TYPE_ADDRESS, &Grid_Connect_Type, 1);
						}
					}
					break;
				case 2010: //直流母线PCS从电网充电使能开关
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if (INT32_Val == 0 || INT32_Val == 1)
						{
							System_Info.Config.DC_Bus_Charge_Permit = (uint8_t) INT32_Val;
							write_eeprom(DC_BUS_CHARGE_PERMIT_ADDRESS, &System_Info.Config.DC_Bus_Charge_Permit, 1);
						}
					}
					break;
				case 2011: //系统功率设置
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if (INT32_Val <= SYS_POWER_SET_MAX)
						{
							System_Info.Config.System_Power_Set = INT32_Val;
							write_eeprom(SYS_POWER_SET_ADDRESS, &System_Info.Config.System_Power_Set, 2);
						}
					}
					break;
				case 2012://PCS型号确定
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if((INT32_Val==PCS_Power_SC30_50)||(INT32_Val==PCS_Power_SC100)||(INT32_Val==PCS_Power_SC250)
								|| (INT32_Val==PCS_Power_SC500)||(INT32_Val==PCS_Power_SC500TL)||(INT32_Val==PCS_Power_SC1000))
						{
							System_Info.Config.PCS_Type=INT32_Val;
							Com_Data.Load_Default_Flag = 0xAA;//设置完需回复出厂设置
							write_eeprom(SYS_PCS_POWER_TYPE_ADDRESS, &System_Info.Config.PCS_Type, 1);
							reboot = true;
							Sys_Ctrl.Sys_Stop = true;
						}
					}
					break;
				case 2013: //分时电价控制使能
	            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==0) || (INT32_Val==1) ||(INT32_Val==2))
						{
							System_Info.Config.Time_Divide_Function = (uint16_t)INT32_Val;
							if(System_Info.Config.Time_Divide_Function != 0)
							{
								Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode = CONST_POWER; //从高级功能退出
								write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode, 1);
							}
							write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
							if(INT32_Val != 0) Clear_Day_Curve_Array();
						}
	            	}
					break;
				case 2014: //PCS通讯模式选择
	            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val== PCS_COMM_SERIAL && Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode != PEAK_LOAD_SHIFTING)||
								(INT32_Val==PCS_COMM_LAN))
						{
							System_Info.Config.PCS_Comm_Type=(uint8_t)INT32_Val;
							write_eeprom(PCS_COMM_ADDRESS, & System_Info.Config.PCS_Comm_Type, 1);
							if(INT32_Val== PCS_COMM_SERIAL){
								ctrl.serialSend_thread.setThreadPriority(SCHED_FIFO);
								ctrl.serialSend_thread.setThreadPriorityParam(99);
							}else{
								ctrl.serialSend_thread.setThreadPriority(SCHED_RR);
								ctrl.serialSend_thread.setThreadPriorityParam(7);
							}
						}
	            	}
					break;
				case 2015: //PCS并网工作模式选择
					if((INT32_Val==GRID_CONST_I_MODE)||(INT32_Val==GRID_CONST_V_MODE)||(INT32_Val==GRID_CONST_P_DC_MODE)
					|| (INT32_Val==GRID_CONST_P_AC_MODE)||(INT32_Val==GRID_STANDBY_MODE))
					{
						System_Info.Config.PCS_Grid_Run_Mode=(uint8_t)INT32_Val;
						write_eeprom(PCS_GRID_RUN_MODE_ADDRESS, & System_Info.Config.PCS_Grid_Run_Mode, 1);
					}
					break;
				case 2016: //配电功能使能/禁止
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==0xAA )||( INT32_Val==0x55))
						{
							System_Info.Config.Power_Distribution_Enable=INT32_Val;
							write_eeprom(POWER_DISTRIBUTION_ENABLE_ADDRESS, &System_Info.Config.Power_Distribution_Enable, 1);
						}
	            	}
					break;
				case 2017: //DCDC通讯使能/禁止
	            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==0 )||( INT32_Val==1)||( INT32_Val==2))
						{
							System_Info.Config.DCDC_Type=INT32_Val;
							write_eeprom(DCDC_TYPE_ADDRESS, &System_Info.Config.DCDC_Type, 1);
							if(System_Info.Config.DCDC_Type == 0){
								webPageShow.dcdc = 0;
								write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
							}else{
								webPageShow.dcdc = 1;
								write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
							}
						}
	            	}
					break;
				case 2018: //Meter通讯使能/禁止
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==0xAA )||( INT32_Val==0x55))
						{
							System_Info.Config.Meter_Comm_Enable=INT32_Val;
							write_eeprom(METER_COMM_ENABLE_ADDRESS, &System_Info.Config.Meter_Comm_Enable, 1);
							if(System_Info.Config.Meter_Comm_Enable == 0x55){
								webPageShow.ammeter=0;
								write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
							}else{
								webPageShow.ammeter=1;
								write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
							}
						}
	            	}
					break;
				case 2019: //新旧协议
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if(INT32_Val==NEW_PROTOCOL || INT32_Val==OLD_PROTOCOL)
						{
							System_Info.Config.PCS_Protocol_Type=(uint8_t)INT32_Val;
							write_eeprom(PCS_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.PCS_Protocol_Type, 1);
						}
	            	}
					break;
				case 2020: //触摸屏硬件版本
					Com_Data.Touch_Hardware_Edition = INT32_Val;
					break;
				case 2021: //触摸屏软件版本
					Com_Data.Touch_Software_Edition = INT32_Val;
					break;
				case 2022: //恢复默认值
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if(INT32_Val == 0xAA && Com_Data.Load_Default_Flag != 0xAA)
						{
							pthread_mutex_lock(&mutex_for_eeporm);
							super_system("rm -f /opt/eeprom_bak 2>null");
							usleep(500);
							Com_Data.Load_Default_Flag = 0xAA;
							Com_Data.Load_Defalut_Net_Flag = 0xAA;
							reboot = true;
							Sys_Ctrl.Sys_Stop = true;
							pthread_mutex_unlock(&mutex_for_eeporm);
						}
	            	}
					break;
				case 2023: //BMS类型
	            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val== 0)||(INT32_Val==1)||(INT32_Val==2))
						{
							System_Info.Config.BMS_Type=(uint8_t)INT32_Val;
							write_eeprom(BMS_TYPE_ADDRESS, & System_Info.Config.BMS_Type, 1);
							if(System_Info.Config.BMS_Type == 0){
								webPageShow.bms=0;
								write_eeprom(WEB_PAGE_BMS_ST_ADDRESS, &webPageShow.bms, 1);
							}else{
								webPageShow.bms=1;
								write_eeprom(WEB_PAGE_BMS_ST_ADDRESS, &webPageShow.bms, 1);
							}
						}
	            	}
				break;
				case 2024: //soc均衡使能控制
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
				case 2025: //PCS离网启动策略
	            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val== 0)||(INT32_Val==1))
						{
							System_Info.Config.PCS_Island_Ctrl_Mode=(uint8_t)INT32_Val;
							write_eeprom(PCS_ISLAND_CTRL_MODE_ADDRESS, &System_Info.Config.PCS_Island_Ctrl_Mode, 1);
						}
	            	}
				break;
				case 2049: //PCC点有功控制
					if(TOUCH_CAN_SET_DATA)
					{
						EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Active_Power_Giving=(int16_t)INT32_Val;
						write_eeprom(PCC_ACTIVE_POWER_GIVING_ADDRESS, &EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Active_Power_Giving, 2);
					}
				break;
				case 2050: //PCC点无功控制
					if (TOUCH_CAN_SET_DATA)
					{
						EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Reactive_Power_Giving = (int16_t) INT32_Val;
						write_eeprom(PCC_REACTIVE_POWER_GIVING_ADDRESS, &EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Reactive_Power_Giving, 2);
					}
					break;
				case 2051: //无功调节方式
					if (TOUCH_CAN_SET_DATA)
					{
						if ((INT32_Val == QP) || (INT32_Val == PF) || (INT32_Val == SHUT_OFF))
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode = (int16_t) INT32_Val;
							write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);

							if (SHUT_OFF == EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
							{
								EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = 0; // caoxh
								EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor = P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
								write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
								write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
							}
							else if (PF == EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
							{
								EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = 0; // caoxh
								write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
							}
							else if (QP == EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
							{
								EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor = P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
								write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
							}

							for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
							{
								//PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
								PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode = EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
								PCS_Ctrl_Data[i].Power_Factor = EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
								Send_Data_To_PCS_0x10_02[i] = true;
							}
						}
					}
					break;
				case 2052: //功率因数
					if (TOUCH_CAN_SET_DATA)
					{
						if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == PF)
						{
							if( ((int16_t)INT32_Val>=P_Range[System_Info.Config.PCS_Type].Power_Factor.Min && (int16_t)INT32_Val <= -900) ||
								((int16_t)INT32_Val >= 900 && (int16_t)INT32_Val<=P_Range[System_Info.Config.PCS_Type].Power_Factor.Max) )
							{
								EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor = (int16_t) INT32_Val;
								write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);

								EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = 0; // caoxh
								write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);

								for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
								{
									PCS_Ctrl_Data[i].Power_Factor = EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
									Send_Data_To_PCS_0x10_02[i] = true;
								}
							}
						}
					}
					break;
				case 2053: //无功功率
					if (TOUCH_CAN_SET_DATA)
					{
						if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == QP)
						{
							if (((int16_t) INT32_Val) >= -(int16_t)((float)System_Info.Config.System_Power_Set * 0.3) && ((int16_t) INT32_Val) <= (int16_t)((float)System_Info.Config.System_Power_Set * 0.3))
							{
								EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = (int16_t) INT32_Val;
								write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);

								EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor = P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
								write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
							}
						}
					}
					break;
				case 2069://负载总用电量校正
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val==0)
						{
							System_Run_Info.E_Load_Adjust=0;
						}
						else if(INT32_Val < 0)
						{
							if(System_Run_Info.Load_Energy_Acc>=(((uint64_t)(abs(INT32_Val)))*36000))
							{
								System_Run_Info.Load_Energy_Acc -= (((uint64_t)(abs(INT32_Val)))*36000);
								System_Run_Info.E_Load_Adjust=(int16_t)INT32_Val;
								System_Run_Info.Total_Load_Energy=(uint32_t)(System_Run_Info.Load_Energy_Acc/36000);
								write_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);
								write_eeprom(TOTAL_LOAD_ENERGY_ADDRESS,&System_Run_Info.Total_Load_Energy,4);
							}
							else
							{
								System_Run_Info.E_Load_Adjust=0;
							}
						}
						else
						{
							if((System_Run_Info.Load_Energy_Acc+(((uint64_t)INT32_Val)*36000))>=System_Run_Info.Load_Energy_Acc)
							{
								System_Run_Info.Load_Energy_Acc += ((uint64_t)INT32_Val)*36000;
								System_Run_Info.E_Load_Adjust=(int16_t)INT32_Val;
								System_Run_Info.Total_Load_Energy=(uint32_t)(System_Run_Info.Load_Energy_Acc/36000);
								write_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);
								write_eeprom(TOTAL_LOAD_ENERGY_ADDRESS,&System_Run_Info.Total_Load_Energy,4);
							}
							else
							{
								System_Run_Info.E_Load_Adjust=0;
							}
						}
					}
				break;
				case 2074:
					EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[0].Modubus_Address=INT32_Val;
					write_eeprom(AMMETER0_ADDRESS+0, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[0].Modubus_Address, 1);
				break;
				case 2075:
					EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[1].Modubus_Address=INT32_Val;
					write_eeprom(AMMETER0_ADDRESS+1, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[1].Modubus_Address, 1);
				break;
				case 2076:
					EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[2].Modubus_Address=INT32_Val;
					write_eeprom(AMMETER0_ADDRESS+2, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[2].Modubus_Address, 1);
				break;
				case 2099: //储能变流器总台数
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if(TOUCH_CAN_SET_DATA)
						{
							if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Max)
							{
								EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number=INT32_Val;
								write_eeprom(TOTAL_PCS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number, 2);
							}
						}
					}
				break;
				default:
				break;
			}
		}
		else if(addr >=2100 && addr <= 2105)
		{
	    	uint8_t index = 0;
	    	index = addr-2100;
			if(TOUCH_CAN_SET_DATA)
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
			}
		}
		else if(addr == 2110)  //光伏变流器总台数
		{
			if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
			{
				if(TOUCH_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number=INT32_Val;
						write_eeprom(TOTAL_PV_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number, 2);
					}
				}
			}
		}
		else if(addr >= 2111 && addr <= 2130)
		{
	    	uint8_t index = 0;
	    	index = addr-2111;
			if(TOUCH_CAN_SET_DATA)
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
			}
		}
		else if(addr == 2131)  //DCDC台数
		{
			if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
			{
				if(TOUCH_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number=INT32_Val;
						write_eeprom(TOTAL_DCDC_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number, 2);
					}
				}
			}
		}
		else if(addr >= 2132 && addr <= 2143)
		{
	    	uint8_t index = 0;
	    	index = addr-2132;
			if(TOUCH_CAN_SET_DATA)
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
		else if(addr == 2145) //BMS台数
		{
			if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
			{
				if(TOUCH_CAN_SET_DATA)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number=INT32_Val;
						write_eeprom(TOTAL_BMS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number, 2);
					}
				}
			}
		}
		else if(addr >= 2146 && addr <= 2151)
		{
	    	uint8_t index = 0;
	    	index = addr-2146;
			if(TOUCH_CAN_SET_DATA)
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
		else if(addr >= 2154 && addr <= 2163)
		{
			switch(addr)
			{
				case 2154: //微网启动电压
					if(TOUCH_CAN_SET_DATA)
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
				case 2155: //微网启动频率
					if(TOUCH_CAN_SET_DATA)
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
				case 2156: //恒压模式电压
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max)
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V=INT32_Val;
							write_eeprom(CONST_V_MODE_V_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V, 2);
						}
					}
					break;
				case 2157: //恒压模式电流
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I=INT32_Val;
						write_eeprom(CONST_V_MODE_I_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I, 2);
					}
					break;
				case 2158: //恒压模式电压下限
					if(TOUCH_CAN_SET_DATA)
					{
						if((INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min) &&
						   (INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max) &&
						   (INT32_Val<EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max))
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min=INT32_Val;
							write_eeprom(CONST_V_MODE_V_MIN_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min, 2);
						}
					}
					break;
				case 2159: //恒压模式电压上限
					if(TOUCH_CAN_SET_DATA)
					{
						if((INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min) &&
						   (INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max) &&
						   (INT32_Val>EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min))
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max=INT32_Val;
							write_eeprom(CONST_V_MODE_V_MAX_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max, 2);
						}
					}
					break;
				case 2160://并网运行功率设置
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set=(int16_t)INT32_Val;
							write_eeprom(EMS_RUN_POWER_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set, 2);
						}
					}
					break;
				case 2161:
					break;
				case 2162:
					break;
				case 2163: //BMS总控制
					if (TOUCH_CAN_SET_DATA)
					{
						if (1 == INT32_Val)
						{
							for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
							{
								BMS_DC_Contactor_off_cmd[i] = true;
								BMS_Switch_Control[i] = INT32_Val;
							}
						}
						else if (2 == INT32_Val)
						{
							for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
							{
								BMS_DC_Contactor_on_cmd[i] = true;
								BMS_Switch_Control[i] = INT32_Val;
							}
						}
						else if (3 == INT32_Val)
						{
							for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
							{
								BMS_All_Fault_Reset_cmd[i] = true;
								BMS_Switch_Control[i] = INT32_Val;
							}
						}
					}
					break;
				default:
					break;
			}
		}
		else if(addr >= 2199 && addr <= 2221)
		{
			switch(addr)
			{
				case 2199: //远程本地控制模式
					if((INT32_Val==RE_LO_MODE) || (INT32_Val==REMOTE_MODE) || (INT32_Val==LOCAL_MODE))
					{
						EMS_RL_Mode=INT32_Val;
						write_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 1);
					}
					break;
				case 2200: //低电量节点1输出模式
					if((INT32_Val==NORMAL_OPEN) || (INT32_Val==NORMAL_CLOSE))
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode=INT32_Val;
						write_eeprom(Low_Battery_Output1_Mode_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode, 1);
					}
					break;
				case 2201: //低电量节点2输出模式
					if((INT32_Val==NORMAL_OPEN) || (INT32_Val==NORMAL_CLOSE))
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode=INT32_Val;
						write_eeprom(Low_Battery_Output2_Mode_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode, 1);
					}
					break;
				case 2202: //SOC控制使能
					if(TOUCH_CAN_SET_DATA)
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
				case 2203: //SOC上限2级
					if(TOUCH_CAN_SET_DATA)
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
				case 2204: //SOC上限1级
					if(TOUCH_CAN_SET_DATA)
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
				case 2205: //SOC下限1级
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Max)
						{
							if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 &&
								INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1 &&
								INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover)
							{
								EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1=INT32_Val;
								write_eeprom(SOC_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1, 2);
							}
						}
					}
					break;
				case 2206: //SOC下限2级
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Max)
						{
							if(INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1 &&
								INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover)
							{
								EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2=INT32_Val;
								write_eeprom(SOC_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2, 2);
							}
						}
					}
					break;
				case 2207: //SOC下限1级恢复
					if(TOUCH_CAN_SET_DATA)
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
				case 2208: //SOC下限2级恢复
					if(TOUCH_CAN_SET_DATA)
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
				case 2209: //SOC滞环控制
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=100)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis = INT32_Val;
							write_eeprom(SOC_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis, 2);
						}
					}
					break;
				case 2210: //SOC控制延迟时间
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Max)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay=INT32_Val;
							write_eeprom(SOC_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay, 2);
						}
					}
					break;
				case 2211: //SOC恢复延迟时间
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Max)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay=INT32_Val;
							write_eeprom(SOC_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay, 2);
						}
					}
					break;
				case 2212: //电池电压控制使能
					if(TOUCH_CAN_SET_DATA)
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
				case 2213: //电池电压上限2级
					if(TOUCH_CAN_SET_DATA)
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
				case 2214: //电池电压上限1级
					if(TOUCH_CAN_SET_DATA)
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
				case 2215: //电池电压下限1级
					if(TOUCH_CAN_SET_DATA)
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
				case 2216: //电池电压下限2级
					if(TOUCH_CAN_SET_DATA)
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
				case 2217: //电池电压下限1级恢复
					if(TOUCH_CAN_SET_DATA)
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
				case 2218: //电池电压下限2级恢复
					if(TOUCH_CAN_SET_DATA)
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
				case 2219: //VBat滞环控制
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=500)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis= INT32_Val;
							write_eeprom(VBAT_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis, 2);
						}
					}
					break;
				case 2220: //电池电压控制延迟时间
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Max)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay=INT32_Val;
							write_eeprom(VBAT_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay, 2);
						}
					}
					break;
				case 2221: //电池电压恢复延迟时间
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Max)
						{
							EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay=INT32_Val;
							write_eeprom(VBAT_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay, 2);
						}
					}
					break;
				default:
					break;
			}
		}
		else if(addr >= 2225 && addr <= 2284)  //分时电价
		{
	    	uint8_t index = 0, sel = 0;
	    	index = (addr-2225)/6;
	    	sel = (addr-2225)%6;
	    	switch(sel)
	    	{
	    		case 0://起始时间0(时)
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=23)
						{
							Time_Div_Info.Time_Div_Set_Info[index].Start_time.hour = (uint8_t)INT32_Val;
							write_eeprom(TIME_DIV_START_TIME_HOUR_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Start_time.hour,1);
						}
					}
					break;
				case 1://起始时间0(分)
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=59)
						{
							Time_Div_Info.Time_Div_Set_Info[index].Start_time.min = (uint8_t)INT32_Val;
							write_eeprom(TIME_DIV_START_TIME_MIN_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Start_time.min,1);
						}
					}
					break;
				case 2://结束时间0(时)
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=23)
						{
							Time_Div_Info.Time_Div_Set_Info[index].End_time.hour = (uint8_t)INT32_Val;
							write_eeprom(TIME_DIV_END_TIME_HOUR_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].End_time.hour,1);
						}
					}
					break;
				case 3://结束时间0(分)
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=59)
						{
							Time_Div_Info.Time_Div_Set_Info[index].End_time.min = (uint8_t)INT32_Val;
							write_eeprom(TIME_DIV_END_TIME_MIN_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].End_time.min,1);
						}
					}
					break;
				case 4://运行模式，0-并网 1-离网 2-并离网切换
					if(TOUCH_CAN_SET_DATA)
					{
						if((INT32_Val==GRID_MODE)|| (INT32_Val==ISLAND_MODE)|| (INT32_Val==GRID_ISLAND_MODE))
						{
							Time_Div_Info.Time_Div_Set_Info[index].Run_Mode = (uint8_t)INT32_Val;
							write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Run_Mode,1);
						}
					}
					break;
				case 5://功率控制模式
					if(TOUCH_CAN_SET_DATA)
					{
						if((INT32_Val==CONST_POWER)|| (INT32_Val==FOLLOW_LOAD_POWER)|| (INT32_Val==PCC_POWER_CTRL)||(INT32_Val==ANTI_REFLUX))
						{
							Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode= (uint8_t)INT32_Val;
							write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode,1);
						}
					}
					break;
				default:
					break;
	    	}
		}
		else if(addr >= 2285 && addr <= 2294) //运行功率设定
		{
	    	uint8_t index = 0;
	    	index = addr-2285;
			if(TOUCH_CAN_SET_DATA)
			{
				if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
				{
					Time_Div_Info.Time_Div_Set_Info[index].Run_Power = (int16_t)INT32_Val;
					write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+index*2,&Time_Div_Info.Time_Div_Set_Info[index].Run_Power,2);
				}
			}
		}
		else if(addr >= 2302 && addr <= 2345)
		{
			switch(addr)
			{
				case 2302: //设备地址
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val>=1 && INT32_Val<=247)
						{
							System_Info.Modbus_Address=INT32_Val;
							write_eeprom(MODBUS_ADDRESS_ADDRESS, &System_Info.Modbus_Address, 1);
						}
					}
					break;
				case 2303: //端口号
					if(TOUCH_CAN_SET_DATA)
					{
						if((1<=INT32_Val)&&(INT32_Val<= UINT16_MAX)&&(INT32_Val!=WEBSERVER_PORT)&&(INT32_Val!=IEC104_PORT))
						{
							System_Info.Modbus_Port=INT32_Val;
							(void)write_eeprom(NET_PORT_ADDRESS, &System_Info.Modbus_Port, 2);
							Com_Data.Touch_Set_Net_Port_Flag = true;
						}
					}
					break;
				case 2305: //IP地址0
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.ip[0]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2306: //IP地址1
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.ip[1]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2307: //IP地址2
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.ip[2]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2308: //IP地址3
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.ip[3]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2309: //子网掩码0
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.submask[0]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2310: //子网掩码1
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.submask[1]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2311: //子网掩码2
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.submask[2]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2312: //子网掩码3
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.submask[3]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2313: //网关0
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.gateway[0]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2314: //网关1
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.gateway[1]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2315: //网关2
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.gateway[2]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2316: //网关3
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							System_Info.Net.gateway[3]=INT32_Val;
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;
				case 2317://DHCP开关
					if(TOUCH_CAN_SET_DATA)
					{
						if((INT32_Val == D_DHCP_ON) || (INT32_Val == D_DHCP_OFF))
						{
							System_Info.Net.dhcp_flag = INT32_Val;
							(void)write_eeprom(NET_DHCP_ADDRESS, &System_Info.Net.dhcp_flag, 1);
							Com_Data.Touch_Set_Net_Flag = true;
						}
					}
					break;

				case 2318://IEC104通讯地址
					if(TOUCH_CAN_SET_DATA)
					{
						if((1<=INT32_Val)&&(INT32_Val<=65535))
						{
							System_Info.IEC104_Address = INT32_Val;
							(void)write_eeprom(IEC104_ADDRESS_ADDRESS,&System_Info.IEC104_Address,2);
						}
					}
					break;
				case 2319://IEC104超时时间T1
					if(TOUCH_CAN_SET_DATA)
					{
						if((1<=INT32_Val)&&(INT32_Val<=System_Info.IEC104_T3))
						{
							System_Info.IEC104_T1 = INT32_Val;
							(void)write_eeprom(IEC104_T1_ADDRESS,&System_Info.IEC104_T1,2);
						}
					}
					break;
				case 2320://IEC104超时时间T3
					if(TOUCH_CAN_SET_DATA)
					{
						if((System_Info.IEC104_T1<=INT32_Val)&&(INT32_Val<=255))
						{
							System_Info.IEC104_T3 = INT32_Val;
							(void)write_eeprom(IEC104_T3_ADDRESS,&System_Info.IEC104_T3,2);
						}
					}
					break;
				case 2321://IEC104参数K
					if(TOUCH_CAN_SET_DATA)
					{
						if((System_Info.IEC104_W<=((uint16_t)INT32_Val*2/3))&&(INT32_Val<=32767))
						{
							System_Info.IEC104_K= INT32_Val;
							(void)write_eeprom(IEC104_K_ADDRESS,&System_Info.IEC104_K,2);
						}
					}
					break;
				case 2322://IEC104参数W
					if(TOUCH_CAN_SET_DATA)
					{
						if((1<=INT32_Val)&&(INT32_Val<=((uint16_t)System_Info.IEC104_K*2/3)))
						{
							System_Info.IEC104_W= INT32_Val;
							(void)write_eeprom(IEC104_W_ADDRESS,&System_Info.IEC104_W,2);
						}
					}
					break;
				case 2336://
					if((INT32_Val<=1000)&&(INT32_Val>=1))
					{
						Balancing_Kp = (uint16_t)INT32_Val;
						write_eeprom(BALANCING_KP_ADDRESS,&Balancing_Kp,2);
					}
					break;

				case 2337://KP
					if((INT32_Val<=6000)&&(INT32_Val>=1))
					{
						wInverterAcVoltDQKp = (uint16_t)INT32_Val;
						write_eeprom(KP_ADDRESS,&wInverterAcVoltDQKp,2);
					}
					break;
				case 2338://KI
					if(INT32_Val<=6000)
					{
						wInverterAcVoltDQKi = (uint16_t)INT32_Val;
						write_eeprom(KI_ADDRESS,&wInverterAcVoltDQKi,2);
					}
					break;

				case 2339://PV KP
					if((INT32_Val<=6000)&&(INT32_Val>=1))
					{
						wInverterAcVoltDQKp_PVLimit = (uint16_t)INT32_Val;
						write_eeprom(KP_PVLIMIT_ADDRESS,&wInverterAcVoltDQKp_PVLimit,2);
					}
					break;
				case 2340://PV KI
					if(INT32_Val<=6000)
					{
						wInverterAcVoltDQKi_PVLimit = (uint16_t)INT32_Val;
						write_eeprom(KI_PVLIMIT_ADDRESS,&wInverterAcVoltDQKi_PVLimit,2);
					}
					break;
				case 2341://zero v max
					if((INT32_Val<=500)&&(INT32_Val>=50))
					{
						System_Info.Zero_V_Max = (uint16_t)INT32_Val;
						write_eeprom(ZERO_V_MAX_ADDRESS,&System_Info.Zero_V_Max,2);
					}
					break;
				case 2342://回调比例
					if((INT32_Val<=10000)&&(INT32_Val>=101))
					{
						Parameter_recovery_kp= (uint16_t)INT32_Val;
						write_eeprom(Parameter_recovery_kp_ADDRESS,&Parameter_recovery_kp,2);
					}
					break;
				case 2343://回调延时
					if((INT32_Val<=100)&&(INT32_Val>=1))
					{
						Parameter_recovery_dalay_time= (uint16_t)INT32_Val;
						write_eeprom(Parameter_recovery_dalay_time_ADDRESS,&Parameter_recovery_dalay_time,2);
					}
					break;
				case 2344://最大逆流功率
					if((INT32_Val >= 0)&&(INT32_Val <= 30000))
					{
						if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode >= 10)
						{
							if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode == ACSIDE_PVLIMIT)
							{
								if(Time_Div_Info.Time_Div_Set_Info[0].Run_Power < INT32_Val)
								{
									Max_anti_reflux_power = INT32_Val;
									write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
								}
							}
							else
							{
								Max_anti_reflux_power = INT32_Val;
								write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
							}
						}
						else
						{
							if(System_Info.Config.Time_Divide_Function == 1)
							{
								int i=0;
								for(i=0; i<10; i++)
								{
									if((Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode == PCC_POWER_CTRL) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Power >= INT32_Val))
									{
										break;
									}
								}
								if(i == 10)
								{
									Max_anti_reflux_power= INT32_Val;
									write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
								}
							}
							else if(System_Info.Config.Time_Divide_Function == 2)
							{
								int i=0;
								for(i=0; i<7; i++)
								{
									if((Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode == PCC_POWER_CTRL) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Power >= INT32_Val))
									{
										break;
									}
								}
								if(i == 7)
								{
									Max_anti_reflux_power= INT32_Val;
									write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
								}
							}
							else
							{
								int i=0;
								for(i=0; i<1; i++)
								{
									if((Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode == PCC_POWER_CTRL) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Power >= INT32_Val))
									{
										break;
									}
								}
								if(i == 1)
								{
									Max_anti_reflux_power= INT32_Val;
									write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
								}
							}
						}
					}
					break;
				case 2345: //光伏广播使能
					if((INT32_Val==ENABLE)||( INT32_Val==DISABLE))
					{
						Send_Data_To_PV_Broadcast_Enable=INT32_Val;
						write_eeprom(Send_Data_To_PV_Broadcast_Enable_ADDRESS, &Send_Data_To_PV_Broadcast_Enable, 2);
					}
				break;
				default:
				break;
			}
		}
		else if(addr >= 2349 && addr <= 2378)  //目标服务器参数
		{
	    	uint8_t index = 0, sel = 0;
	    	index = (addr-2349)/5;
	    	sel = (addr-2349)%5;
	    	switch(sel)
	    	{
				case 0: //IP地址0
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							if(System_Info.Server[index].ip[0]!=INT32_Val)
							{
								System_Info.Server[index].ip[0]=INT32_Val;
								Com_Data.Touch_Set_Server_Port_Flag[index] = true;
								PCS_Svr_Param_Change[index]=true;
								write_eeprom(SERVER_0_IP_0_ADDRESS+index*4, &System_Info.Server[index].ip[0], 1);
							}
						}
					}
					break;
				case 1: //IP地址1
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							if(System_Info.Server[index].ip[1]!=INT32_Val)
							{
								System_Info.Server[index].ip[1]=INT32_Val;
								Com_Data.Touch_Set_Server_Port_Flag[index] = true;
								PCS_Svr_Param_Change[index]=true;
								write_eeprom(SERVER_0_IP_1_ADDRESS+index*4, &System_Info.Server[index].ip[1], 1);
							}
						}
					}
					break;
				case 2: //IP地址2
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							if(System_Info.Server[index].ip[2]!=INT32_Val)
							{
								System_Info.Server[index].ip[2]=INT32_Val;
								Com_Data.Touch_Set_Server_Port_Flag[index] = true;
								PCS_Svr_Param_Change[index]=true;
								write_eeprom(SERVER_0_IP_2_ADDRESS+index*4, &System_Info.Server[index].ip[2], 1);
							}
						}
					}
					break;
				case 3: //IP地址3
					if(TOUCH_CAN_SET_DATA)
					{
						if(INT32_Val<=255)
						{
							if(System_Info.Server[index].ip[3]!=INT32_Val)
							{
								System_Info.Server[index].ip[3]=INT32_Val;
								Com_Data.Touch_Set_Server_Port_Flag[index] = true;
								PCS_Svr_Param_Change[index]=true;
								write_eeprom(SERVER_0_IP_3_ADDRESS+index*4, &System_Info.Server[index].ip[3], 1);
							}
						}
					}
					break;
				case 4: //端口号
					if(TOUCH_CAN_SET_DATA)
					{
						if((1<=INT32_Val)&&(INT32_Val<= UINT16_MAX))
						{
							if(System_Info.Server_Modbus_Port[index]!=INT32_Val)
							{
								System_Info.Server_Modbus_Port[index]=INT32_Val;
								Com_Data.Touch_Set_Server_Port_Flag[index] = true;
								PCS_Svr_Param_Change[index]=true;
								write_eeprom(SERVER_0_PORT_ADDRESS+index*2, &System_Info.Server_Modbus_Port[index], 2);
							}
						}
					}
					break;
				default:
					break;
	    	}
		}
		else if(addr >= 2300)
		{
			switch (addr)
			{
				case 2399://事件上一页
					if(INT32_Val==0)
					{
						if(read_event_record.current_page_nub>1)
						{
							read_event_record.current_page_nub--;
							read_event_record.read_event_record_flash_flag=true;
						}
					}
				break;
				case 2400://事件下一页
					if(INT32_Val==1)
					{
						if(read_event_record.current_page_nub<read_event_record.total_page_nub)
						{
						   read_event_record.current_page_nub++;
						   read_event_record.read_event_record_flash_flag=true;
						}
					}
				break;
				case 2449://故障上一页
					if(INT32_Val==0)
					{
						if(read_fault_record.current_page_nub>1)
						{
							read_fault_record.current_page_nub--;
							read_fault_record.read_fault_record_flash_flag=true;
						}
					}
				break;
				case 2450://故障下一页
					if(INT32_Val==1)
					{
						if(read_fault_record.current_page_nub<read_fault_record.total_page_nub)
						{
						   read_fault_record.current_page_nub++;
						   read_fault_record.read_fault_record_flash_flag=true;
						}
					}
				break;
				case 2499://PCS上一台
					if(INT32_Val==0)
					{
						if(Disp_PCS_Num>0)
						{
							Disp_PCS_Num--;
						}
					}
				break;
				case 2500://PCS下一台
					if(INT32_Val==1)
					{
						if(Disp_PCS_Num<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number-1)
						{
						   Disp_PCS_Num++;
						}
					}
				break;
				case 2549://PV上一台
					if(INT32_Val==0)
					{
						if(Disp_PV_Num>0)
						{
							Disp_PV_Num--;
						}
					}
				break;
				case 2550://PV下一台
					if(INT32_Val==1)
					{
						if(Disp_PV_Num<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number-1)
						{
						   Disp_PV_Num++;
						}
					}
				break;
				case 2599://DCDC上一台
					if(INT32_Val==0)
					{
						if(Disp_DCDC_Num>0)
						{
							Disp_DCDC_Num--;
						}
					}
				break;
				case 2600://DCDC下一台
					if(INT32_Val==1)
					{
						if(Disp_DCDC_Num<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number-1)
						{
						   Disp_DCDC_Num++;
						}
					}
				break;
				case 2649://BMS上一台
					if(INT32_Val==0)
					{
						if(Disp_BMS_Num>0)
						{
							Disp_BMS_Num--;
						}
					}
				break;
				case 2650://BMS下一台
					if(INT32_Val==1)
					{
						if(Disp_BMS_Num<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number-1)
						{
						   Disp_BMS_Num++;
						}
					}
				break;
				//调试参数
				case 3000:
					if(INT32_Val<=8000)
					{
						PV_OK_DC_V=INT32_Val;
					}
				break;
				case 3300://离网运行功率设置0
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[0].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+0*2,&Time_Div_Info.Time_Div_Set_Info[0].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3301://离网运行功率设置1
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[1].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+1*2,&Time_Div_Info.Time_Div_Set_Info[1].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3302://离网运行功率设置2
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[2].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+2*2,&Time_Div_Info.Time_Div_Set_Info[2].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3303://离网运行功率设置3
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[3].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+3*2,&Time_Div_Info.Time_Div_Set_Info[3].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3304://离网运行功率设置4
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[4].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+4*2,&Time_Div_Info.Time_Div_Set_Info[4].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3305://离网运行功率设置5
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[5].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+5*2,&Time_Div_Info.Time_Div_Set_Info[5].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3306://离网运行功率设置6
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[6].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+6*2,&Time_Div_Info.Time_Div_Set_Info[6].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3307://离网运行功率设置7
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[7].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+7*2,&Time_Div_Info.Time_Div_Set_Info[7].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3308://离网运行功率设置8
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[8].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+8*2,&Time_Div_Info.Time_Div_Set_Info[8].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3309://离网运行功率设置9
					if(TOUCH_CAN_SET_DATA)
					{
						if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
						{
							Time_Div_Info.Time_Div_Set_Info[9].Mgrid_Run_Power = (int16_t)INT32_Val;
							write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+9*2,&Time_Div_Info.Time_Div_Set_Info[9].Mgrid_Run_Power,2);
						}
					}
				break;
				case 3320:
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
						{
							System_Info.EMS_Meter_Com_Count[0] = METER_SIGNAL_FULL;
							Meter_Com_Err[0] = false;
							System_Info.Config.Meter_Position_Enable[0] = INT32_Val;
							write_eeprom(Meter_Position_Enable_ADDRESS0 + 0, &System_Info.Config.Meter_Position_Enable[0], 1);
						}
	            	}
	            break;
				case 3321:
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
						{
							System_Info.EMS_Meter_Com_Count[1] = METER_SIGNAL_FULL;
							Meter_Com_Err[1] = false;
							System_Info.Config.Meter_Position_Enable[1] = INT32_Val;
							write_eeprom(Meter_Position_Enable_ADDRESS0 + 1, &System_Info.Config.Meter_Position_Enable[1], 1);
						}
	            	}
				break;
				case 3322:
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
						{
							System_Info.EMS_Meter_Com_Count[2] = METER_SIGNAL_FULL;
							Meter_Com_Err[2] = false;
							System_Info.Config.Meter_Position_Enable[2] = INT32_Val;
							write_eeprom(Meter_Position_Enable_ADDRESS0 + 2, &System_Info.Config.Meter_Position_Enable[2], 1);
						}
	            	}
				break;
				case 3400://网口下发周期
					if((INT32_Val >= 0) && (INT32_Val <= 2000))
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay = INT32_Val;
						write_eeprom(PCS_NET_SETPARA_DELAY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay, 2);
					}
					break;
				case 3401://网口设置周期
					if((INT32_Val >= 1) && (INT32_Val <= 50))
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle = INT32_Val;
						write_eeprom(PCS_NET_SET_CYCLE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle, 2);
					}
					break;
				case 3402: //电表协议选择
					if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
					{
						if(INT32_Val == 0 || INT32_Val == 1)
						{
							System_Info.Config.Ammeter_Protocol_Type = INT32_Val;
							write_eeprom(AMMETER_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.Ammeter_Protocol_Type, 1);
							for(int i=0;i<6;i++)
							{
								if(comParam[i].protocol == SERIAL_PROTOCOL_AMMETER)
								{
									ctrl.serialRev_thread[i].SetComParam(comParam[i]);
									ctrl.serialRev_thread[i].Set_Baud(comParam[i].baud);
								}
							}
						}
					}
				break;
	            case 3403:
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
						{
							System_Info.EMS_Meter_Com_Count[0] = METER_SIGNAL_FULL;
							Meter_Com_Err[0] = false;
							System_Info.Config.Meter_Position_Enable[0] = INT32_Val;
							write_eeprom(Meter_Position_Enable_ADDRESS0 + 0, &System_Info.Config.Meter_Position_Enable[0], 1);
						}
	            	}
	        	break;
	            case 3404:
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
						{
							System_Info.EMS_Meter_Com_Count[1] = METER_SIGNAL_FULL;
							Meter_Com_Err[1] = false;
							System_Info.Config.Meter_Position_Enable[1] = INT32_Val;
							write_eeprom(Meter_Position_Enable_ADDRESS0 + 1, &System_Info.Config.Meter_Position_Enable[1], 1);
						}
	            	}
	        	break;
	            case 3405:
	            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
	            	{
						if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
						{
							System_Info.EMS_Meter_Com_Count[2] = METER_SIGNAL_FULL;
							Meter_Com_Err[2] = false;
							System_Info.Config.Meter_Position_Enable[2] = INT32_Val;
							write_eeprom(Meter_Position_Enable_ADDRESS0 + 2, &System_Info.Config.Meter_Position_Enable[2], 1);
						}
	            	}
	        	break;
	            case 3410:
					if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[0].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
					{
						Serial_Param_Change[0]=true;
						comParam[0].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
						comParam[0].vmin = 255;
						write_eeprom(COM1_PARAM_BAUD_ADDRESS+2, &comParam[0].vmin, 2);
						write_eeprom(COM1_PARAM_BAUD_ADDRESS+6, &comParam[0].protocol, 1);
					}
	            break;
	            case 3411:
					if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[1].protocol != (SERIAL_PROTOCOL_TYPE)INT32_Val)
					{
						Serial_Param_Change[1]=true;
						comParam[1].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
						comParam[1].vmin = 255;
						write_eeprom(COM2_PARAM_BAUD_ADDRESS+2, &comParam[1].vmin, 2);
						write_eeprom(COM2_PARAM_BAUD_ADDRESS+6, &comParam[1].protocol, 1);
					}
	            break;
	            case 3412:
					if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[2].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
					{
						Serial_Param_Change[2]=true;
						comParam[2].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
						comParam[2].vmin = 255;
						write_eeprom(COM3_PARAM_BAUD_ADDRESS+2, &comParam[2].vmin, 2);
						write_eeprom(COM3_PARAM_BAUD_ADDRESS+6, &comParam[2].protocol, 1);
					}
	            break;
	            case 3413:
					if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[3].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
					{
						Serial_Param_Change[3]=true;
						comParam[3].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
						comParam[3].vmin = 255;
						write_eeprom(COM4_PARAM_BAUD_ADDRESS+2, &comParam[3].vmin, 2);
						write_eeprom(COM4_PARAM_BAUD_ADDRESS+6, &comParam[3].protocol, 1);
					}
	            break;
	            case 3414:
					if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[4].protocol !=(SERIAL_PROTOCOL_TYPE)INT32_Val)
					{
						Serial_Param_Change[4]=true;
						comParam[4].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
						comParam[4].vmin = 255;
						write_eeprom(COM5_PARAM_BAUD_ADDRESS+2, &comParam[4].vmin, 2);
						write_eeprom(COM5_PARAM_BAUD_ADDRESS+6, &comParam[4].protocol, 1);
					}
	            break;
	            case 3415:
					if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[5].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
					{
						Serial_Param_Change[5]=true;
						comParam[5].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
						if(comParam[5].protocol == SERIAL_PROTOCOL_MODBUS_TOUCH) comParam[5].vmin = 8;
						else if(comParam[5].protocol == SERIAL_PROTOCOL_MODBUS_MONITOR) comParam[5].vmin = 255;
						write_eeprom(COM6_PARAM_BAUD_ADDRESS+2, &comParam[5].vmin, 2);
						write_eeprom(COM6_PARAM_BAUD_ADDRESS+6, &comParam[5].protocol, 1);
					}
	            break;
				case 3500: //高级功能设置
					if(INT32_Val<20)
					{
						Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=INT32_Val;
						write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode,1);
					}
				break;

				case 3501: // 峰值
					if((((int16_t)INT32_Val)>=0) && (((int16_t)INT32_Val)>Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power))
					{
						Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power = (int16_t)INT32_Val;
						write_eeprom(PEAK_VALLEY_1_START_DISCHARGE_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power,2);
					}
				break;
				case 3502: //谷值
					if((((int16_t)INT32_Val)>=0) && (((int16_t)INT32_Val)<(Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power)))
					{
						Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power = (int16_t)INT32_Val;
						write_eeprom(PEAK_VALLEY_1_START_CHARGE_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power,2);
					}
				break;
				case 3503: //更新周期
	            	if((INT32_Val >= 300) &&(INT32_Val <= 30000))
	            	{
	            		Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle = INT32_Val;
	            		write_eeprom(PEAK_VALLEY_1_UPDATE_CYCLE_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle, 2);
	            	}
				break;
				default:
				break;
			}
		}
	}
}
int EMS_Receive_Data_From_Touch_By_UART(uint8_t *pbuf, uint32_t buflen, uint8_t *sbuf)  //接收触摸屏发来的信息
{
	uint16_t crcresult = 0;  //crc值
	uint16_t modbus_address = (uint16_t) (((uint16_t) pbuf[2] << 8) | pbuf[3]);  //modbus地址
	uint16_t modbus_len = (uint16_t) (((uint16_t) pbuf[4] << 8) | pbuf[5]);
	uint16_t len = 0;
	uint16_t slen = 0;
	uint8_t fault_code = 0;

	crcresult = embf_get_crc16(pbuf, buflen - 2);

	if (LOBYTE(crcresult) == pbuf[buflen - 2] && HIBYTE(crcresult) == pbuf[buflen - 1])
	{
		if (1 == pbuf[0])
		{
			{
				if ((pbuf[1] != 0x01) && (pbuf[1] != 0x03) && (pbuf[1] != 0x04) && (pbuf[1] != 0x06) && (pbuf[1] != 0x10) && (pbuf[1] != 0x42) && (pbuf[1] != 0x43) && (pbuf[1] != 0x44))
				{
					fault_code = 1;				//功能码异常
				}
				else if (((modbus_len < 1) || (modbus_len > 0x7d)) && (pbuf[1] != 0x06))
				{
					fault_code = 3;				//寄存器数目异常
				}
				else if (modbus_address + modbus_len > 39999)
				{
					fault_code = 2;				//寄存器地址异常
				}
				else
				{
					if (modbus_address < 9999)
					{
						mutex_for_timer.lock();
						if (0x01 == pbuf[1])
						{
							g_adjust_flag = 0xA1;   //不处于校表状态
							Touch_Get_Data_From_EMS_0x01(modbus_address, modbus_len, sbuf + 3);
							memcpy(sbuf, pbuf, 2);   //01命令按位读取,返回读取的字节长度
							sbuf[2] = (0 != (modbus_len % 8)) ? (modbus_len / 8 + 1) : (modbus_len / 8);
							crcresult = embf_get_crc16(sbuf, 3 + sbuf[2]);
							sbuf[3 + sbuf[2]] = LOBYTE(crcresult);
							sbuf[4 + sbuf[2]] = HIBYTE(crcresult);
							slen = sbuf[2] + 5;   //返回读取的字节长度
						}
						else if (0x03 == pbuf[1])
						{
							g_adjust_flag = 0xA1;   //不处于校表状态
							Touch_Get_Data_From_EMS_0x03(modbus_address, modbus_len, sbuf + 3);
							memcpy(sbuf, pbuf, 2);
							len = modbus_len * 2;   //返回字节长度
							sbuf[2] = (uint8_t) len;
							crcresult = embf_get_crc16(sbuf, 3 + len);
							sbuf[3 + len] = LOBYTE(crcresult);
							sbuf[4 + len] = HIBYTE(crcresult);
							slen = len + 5;   //返回读取的字节长度
						}
						else if (0x04 == pbuf[1])
						{
							g_adjust_flag = 0xA1;   //不处于校表状态
							Touch_Get_Data_From_EMS_0x04(modbus_address, modbus_len, sbuf + 3);
							memcpy(sbuf, pbuf, 2);
							len = modbus_len * 2;   //返回字节长度
							sbuf[2] = (uint8_t) len;
							crcresult = embf_get_crc16(sbuf, 3 + len);
							sbuf[3 + len] = LOBYTE(crcresult);
							sbuf[4 + len] = HIBYTE(crcresult);
							slen = len + 5;   //返回读取的字节长度
						}
						else if (0x10 == pbuf[1])
						{
							g_adjust_flag = 0xA1;   //不处于校表状态
							Touch_Write_Time[0] = showTime.tm_year;
							Touch_Write_Time[1] = showTime.tm_mon;
							Touch_Write_Time[2] = showTime.tm_day;
							Touch_Write_Time[3] = showTime.tm_hour;
							Touch_Write_Time[4] = showTime.tm_min;
							Touch_Write_Time[5] = showTime.tm_sec;

							Touch_Set_Data_To_EMS_0x10(modbus_address, (uint8_t) modbus_len, pbuf + 7);
						    for(int i=0;i<6;i++)
						    {
						    	if(Serial_Param_Change[i])
						    	{
						    		Serial_Param_Change[i] = false;
						    		mutex_for_timer.lock();
						    		ctrl.serialRev_thread[i].SetComParam(comParam[i]);
						    		ctrl.serialRev_thread[i].Set_Baud(comParam[i].baud);
						    		mutex_for_timer.unlock();
						    	}
						    }
							memcpy(sbuf, pbuf, 6);
							crcresult = embf_get_crc16(sbuf, 6);
							sbuf[6] = LOBYTE(crcresult);
							sbuf[7] = HIBYTE(crcresult);
							slen = 8;
						}
						else
						{
							slen = 0;
						}
						if (Touch_Write_Time_Flag == true)
						{
							Touch_Write_Time_Flag = false;
							Touch_Set_Time_Flag = 0;		//说明触摸屏已经给计量板发送过时间了
							//(void) drv_write(gt_glp.frtc, Touch_Write_Time, 6);		//设置时间函数
							set_sys_time();
							get_sys_time();
                            //PCS
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								Send_Data_To_PCS_0x10_01[i]=true;
							}
							//PV
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
							{
								Send_Data_To_PV_0x10_01[i]=true;
							}
							//DCDC
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
							{
								Send_Data_To_DCDC_0x10_01[i]=true;
							}							
							//Clear_Data = true;								//判断是否是新的一天，是否要清除前一天的数据
						}
						mutex_for_timer.unlock();
					}
					else
					{
						slen = 0;
					}
				}
				if (fault_code != 0)
				{
					sbuf[0] = pbuf[0];
					sbuf[1] = pbuf[1] | 0x80;
					sbuf[2] = fault_code;
					crcresult = embf_get_crc16(sbuf, 2);
					sbuf[3] = LOBYTE(crcresult);		//ccg 2013.5.28
					sbuf[4] = HIBYTE(crcresult);
					slen = 5;
				}
			}
		}
	}
	else
	{
		return -1;
	}
	return slen;
}

uint8_t Run_Status(uint32_t run_status)                                             //DSP与触摸屏的状态值转换
{
	for (uint8_t i = 0; i < 32; i++)
	{
		if ((run_status >> i) == 1)
		{
			return i + 1;
		}
	}
	return (uint8_t) 0;
}

uint8_t Run_Status_Monitor(uint32_t run_status)
{
	for (uint8_t i = 0; i < 32; i++)
	{
		if ((run_status >> i) == 1)
		{
			return i;
		}
	}
	return (uint8_t) 0;
}

uint8_t Run_Status_BMS(uint16_t run_status)
{
	for (uint8_t i = 0; i < 16; i++)
	{
		if ((run_status >> i) == 1)
		{
			return i;
		}
	}
	return (uint8_t) 0;
}

uint8_t Run_Status_For_PV(uint8_t id, uint32_t run_status)
{
	uint8_t b=0;
	switch(run_status)
	{
		case 0x0000:
		  b=(PV_Com_Err[id]==false)?run_status+1:0;
		break;
		case 0x0001:
		  b=2;
		break;
		case 0x0002:
		  b=3;
		break;
		case 0x0004:
		  b=4;
		break;
		case 0x0008:
		  b=5;
		break;
		case 0x0010:
		  b=6;
		break;
		case 0x0020:
		  b=7;
		break;
		case 0x0040:
		  b=8;
		break;
		case 0x0080:
		  b=9;
		break;
		case 0x0100:
		  b=10;
		break;
		case 0x0200:
		  b=11;
		break;
		case 0x0400:
		  b=12;
		break;
		case 0x0800:
		  b=13;
		break;
		case 0x4000:
		  b=16;
		break;
		case 0x8000:
		  b=17;
		break;
		case 0x1200:
		  b=18;
		break;
		case 0x1300:
		 b=19;
		break;
		case 0x1400:
		  b=20;
		break;
		case 0x1500:
		  b=21;
		break;
		case 0x1600:
		  b=22;
		break;
		case 0x1700:
		  b=23;
		break;
		case 0x1800:
		  b=24;
		break;
		case 0x2300:
		  b=25;
		break;
		case 0x2400:
		  b=26;
		break;
		case 0x2500:
		  b=27;
		break;
		case 0x2600:
		  b=28;
		break;
		case 0x2700:
		  b=29;
		break;
		case 0x2800:
		  b=30;
		break;
		case 0x2900:
		  b=31;
		break;
		case 0x2A00:
		  b=32;
		break;
		case 0x3000:
		  b=33;
		break;
		case 0x3300:
		  b=34;
		break;
		case 0x3400:
		  b=35;
		break;
		case 0x3600:
		  b=36;
		break;
		case 0x4010:
		  b=37;
		break;
		case 0x4011:
		  b=38;
		break;
		case 0x4012:
		  b=39;
		break;
		case 0x4013:
		  b=40;
		break;
		case 0x4014:
		  b=41;
		break;
		case 0x4015:
		  b=42;
		break;
		case 0x4016:
		  b=43;
		break;
		case 0x4017:
		  b=44;
		break;
		case 0x4018:
		  b=45;
		break;
		case 0x4019:
		  b=46;
		break;
		case 0x401A:
		  b=47;
		break;
		case 0x401B:
		  b=48;
		break;
		case 0x401C:
		  b=49;
		break;
		case 0x401D:
		  b=50;
		break;
		case 0x4022:
		  b=51;
		break;
		case 0x5000:
		  b=52;
		break;
		case 0x5100:
		  b=53;
		break;
		case 0x5200:
		  b=54;
		break;
		case 0x5300:
		  b=55;
		break;
		case 0x5400:
		  b=56;
		break;
		case 0x5500:
		  b=57;
		break;
		case 0x8100:
		  b=58;
		break;
		case 0x8200:
		  b=59;
		break;
		case 0x9100:
		  b=60;
		break;
		default:
		  b=0;
		 break;
	}
	return b;
}

void Touch_Picture_Disp()
{
	//储能图片流动方向
	PCS_Pic_Direction = (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power > 0 ? 2 : (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power < 0 ? 1 : 0));
	//光伏图片流动方向
	PV_Pic_Direction = ((EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power > 0) ? 1 : 0);

	//电网图片流动方向
	if(System_Info.Config.Power_Distribution_Enable == ENABLE) //配电使能情况
	{
		if (DIN(Grid_Contacter))
		{
			if(DIN(MGrid_Contacter))
			{
				if((int32_t)(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power + (int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)<=
						EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power)
				{
					Grid_Pic_Direction=1;
				}
				else
				{
					Grid_Pic_Direction=2;
				}
			}
			else
			{
				if(GRID_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage))
				{
					Grid_Pic_Direction=1;
				}
				else
				{
					Grid_Pic_Direction=0;
				}
			}
		}
		else
		{
			Grid_Pic_Direction=0;
		}
	}
	else  //非配电情况下
	{
		if(GRID_RUNNING)
		{
			if((int32_t)(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power + (int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)<=
					EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power)
			{
				Grid_Pic_Direction=1;
			}
			else
			{
				Grid_Pic_Direction=2;
			}
		}
		else
		{
			Grid_Pic_Direction=0;
		}
	}

	//负载图片流动方向
	//Load_Pic_Direction=(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power>0?1:0);
	if (EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power > 0)
	{
		Load_Pic_Direction = 1;
	}
	else if (Grid_Pic_Direction != 0)
	{
		Load_Pic_Direction = 1;
	}
	else if (MGrid_Pic_Direction != 0)
	{
		Load_Pic_Direction = 1;
	}
	else
	{
		Load_Pic_Direction = 0;
	}


	//微网输入段图片流动方向
	if(System_Info.Config.Power_Distribution_Enable == ENABLE)  //配电使能情况
	{
		if (DIN(MGrid_Contacter))
		{
			if((EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power+(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)>0)
			{
				MGrid_Pic_Direction=2;
			}
			else if(((EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power+(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)<0) && (DIN(Grid_Contacter)))
			{
				MGrid_Pic_Direction=1;
			}
			else
			{
				MGrid_Pic_Direction=0;
			}
		}
		else
		{
			MGrid_Pic_Direction=0;
		}
	}
	else  //非配电情况下
	{
		if((ISLAND_RUNNING)||(GRID_RUNNING))
		{
			if((EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power+(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)>0)
			{
				MGrid_Pic_Direction=2;
			}
			else if((EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power+(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)<0)
			{
				MGrid_Pic_Direction=1;
			}
			else
			{
				MGrid_Pic_Direction=0;
			}
		}
		else
		{
			MGrid_Pic_Direction=0;
		}
	}
}

void Touch_Load_History_Event_Record(void)                                          //加载历史事件记录
{
	uint16_t i = 0;
	uint16_t page_nb = 0;
	uint8_t array[16];

	if (read_event_record.read_event_record_flash_flag == true)                                          //是否刷新读历史记录
	{
		if (read_event_record.total_nub > 0)                                          //总的记录数>0
		{
			//一共的页数
			read_event_record.total_page_nub = (read_event_record.total_nub - 1) / EVERY_EVENTRECORD_PAGE_NB + 1;
		}
		else
		{
			read_event_record.total_page_nub = 1;
			read_event_record.current_page_nub = 1;
			read_event_record.current_page_nub_record_nb = 0;                                          //当前页的记录数=0
		}

		page_nb = read_event_record.current_page_nub;

		if (page_nb > 0)                                          //当前的页号>0
		{
			if (read_event_record.total_page_nub > 1)                                          //总的页数
			{
				if (page_nb < read_event_record.total_page_nub)                                          //页面号小于总的页面号
				{
					//当前显示的页码中的记录数为最大页面显示数
					read_event_record.current_page_nub_record_nb = EVERY_EVENTRECORD_PAGE_NB;
				}
				else
				{
					//当前显示的页码中的记录数为总数减去最大页面显示数×（页面－1）
					read_event_record.current_page_nub_record_nb = read_event_record.total_nub - (page_nb - 1) * EVERY_EVENTRECORD_PAGE_NB;
				}
			}
			else
			{
				read_event_record.current_page_nub_record_nb = read_event_record.total_nub;
			}

			for (i = 0; i < EVERY_EVENTRECORD_PAGE_NB; i++)
			{
				read_event_record.current_page_nub_record_array[i] = 0; //先清零
			}
			//找到要显示的序列号
			for (i = 0; i < read_event_record.current_page_nub_record_nb; i++)
			{
				//当前页中显示的记录序列号
				read_event_record.current_page_nub_record_array[i] = (page_nb - 1) * EVERY_EVENTRECORD_PAGE_NB + i + 1;

				Read_OnConditionNB_Record(read_event_record.ptr, read_event_record.total_nub, read_event_record.current_page_nub_record_array[i], HISEVENTRECORD_START_ADDRESS, array);

				SetU16(Touch_Data.Touch_History_Event + 6 + i * 16, (uint16_t) read_event_record.current_page_nub_record_array[i]); //序列号
				SetU16(Touch_Data.Touch_History_Event + 8 + i * 16, (array[0]) + 2000); //年
				SetU16(Touch_Data.Touch_History_Event + 10 + i * 16, (array[1]));	    //月
				SetU16(Touch_Data.Touch_History_Event + 12 + i * 16, (array[2]));	    //日
				SetU16(Touch_Data.Touch_History_Event + 14 + i * 16, (array[3]));	    //十
				SetU16(Touch_Data.Touch_History_Event + 16 + i * 16, (array[4]));	    //分
				SetU16(Touch_Data.Touch_History_Event + 18 + i * 16, (array[5]));	    //秒
				SetU16(Touch_Data.Touch_History_Event + 20 + i * 16, array[6]);		//代码
			}
		}

		if (read_event_record.current_page_nub_record_nb < 5)
		{
			//此页面中一共显示无记录的条数
			for (i = read_event_record.current_page_nub_record_nb; i < EVERY_EVENTRECORD_PAGE_NB; i++)
			{
				SetU16(Touch_Data.Touch_History_Event + 6 + i * 16, UINT16_MAX); //序列号
				SetU16(Touch_Data.Touch_History_Event + 8 + i * 16, UINT16_MAX); //年
				SetU16(Touch_Data.Touch_History_Event + 10 + i * 16, UINT16_MAX); //月
				SetU16(Touch_Data.Touch_History_Event + 12 + i * 16, UINT16_MAX); //日
				SetU16(Touch_Data.Touch_History_Event + 14 + i * 16, UINT16_MAX); //时
				SetU16(Touch_Data.Touch_History_Event + 16 + i * 16, UINT16_MAX); //分
				SetU16(Touch_Data.Touch_History_Event + 18 + i * 16, UINT16_MAX); //秒
				SetU16(Touch_Data.Touch_History_Event + 20 + i * 16, UINT16_MAX); //代码
			}
		}

		SetU16(Touch_Data.Touch_History_Event + 0, (uint16_t) read_event_record.total_nub); 		//总记录数
		SetU16(Touch_Data.Touch_History_Event + 2, (uint16_t) read_event_record.total_page_nub); 	//总页数
		SetU16(Touch_Data.Touch_History_Event + 4, (uint16_t) read_event_record.current_page_nub); 	//当前页数

		read_event_record.read_event_record_flash_flag = false;
	}
	if (read_event_record.total_nub == 0) 	//无记录
	{
		for (i = 0; i < 5; i++)
		{
			SetU16(Touch_Data.Touch_History_Event + 6 + i * 16, UINT16_MAX); //序列号
			SetU16(Touch_Data.Touch_History_Event + 8 + i * 16, UINT16_MAX); //年
			SetU16(Touch_Data.Touch_History_Event + 10 + i * 16, UINT16_MAX); //月
			SetU16(Touch_Data.Touch_History_Event + 12 + i * 16, UINT16_MAX); //日
			SetU16(Touch_Data.Touch_History_Event + 14 + i * 16, UINT16_MAX); //十
			SetU16(Touch_Data.Touch_History_Event + 16 + i * 16, UINT16_MAX); //分
			SetU16(Touch_Data.Touch_History_Event + 18 + i * 16, UINT16_MAX); //秒
			SetU16(Touch_Data.Touch_History_Event + 20 + i * 16, UINT16_MAX); //代码
		}
		SetU16(Touch_Data.Touch_History_Event + 0, 0); 			 //总记录数
		SetU16(Touch_Data.Touch_History_Event + 2, 0); 			 //总页数
		SetU16(Touch_Data.Touch_History_Event + 4, 0); 			 //当前页数
	}
}
void Touch_Load_History_Fault_Record(void)                                          //装载历史故障记录
{
	uint16_t i = 0;
	uint16_t page_nb = 0;
	uint8_t array[16];

	if (read_fault_record.read_fault_record_flash_flag == true)                                          //是否刷新读历史记录
	{
		if (read_fault_record.total_nub > 0)                                          //总的记录数>0
		{
			//一共的页数
			read_fault_record.total_page_nub = (read_fault_record.total_nub - 1) / EVERY_FAULTRECORD_PAGE_NB + 1;
		}
		else
		{
			read_fault_record.total_page_nub = 1;
			read_fault_record.current_page_nub = 1;
			read_fault_record.current_page_nub_record_nb = 0; //当前页的记录数=0
		}

		page_nb = read_fault_record.current_page_nub;

		if (page_nb > 0) //当前的页号>0
		{
			if (read_fault_record.total_page_nub > 1) //总的页数
			{
				if (page_nb < read_fault_record.total_page_nub) //页面号小于总的页面号
				{
					//当前显示的页码中的记录数为最大页面显示数
					read_fault_record.current_page_nub_record_nb = EVERY_FAULTRECORD_PAGE_NB;
				}
				else
				{
					//当前显示的页码中的记录数为总数减去最大页面显示数×（页面－1）
					read_fault_record.current_page_nub_record_nb = read_fault_record.total_nub - (page_nb - 1) * EVERY_FAULTRECORD_PAGE_NB;
				}
			}
			else
			{
				read_fault_record.current_page_nub_record_nb = read_fault_record.total_nub;
			}

			for (i = 0; i < EVERY_FAULTRECORD_PAGE_NB; i++)
			{
				read_fault_record.current_page_nub_record_array[i] = 0; //先清零
			}
			//找到要显示的序列号
			for (i = 0; i < read_fault_record.current_page_nub_record_nb; i++)
			{
				//当前页中显示的记录序列号
				read_fault_record.current_page_nub_record_array[i] = (page_nb - 1) * EVERY_FAULTRECORD_PAGE_NB + i + 1;

				Read_OnConditionNB_Record(read_fault_record.ptr, read_fault_record.total_nub, read_fault_record.current_page_nub_record_array[i], HISFAULTRECORD_START_ADDRESS, array);
				//序列号
				SetU16(Touch_Data.Touch_History_Fault + 6 + i * 18, (uint16_t) read_fault_record.current_page_nub_record_array[i]);
				SetU16(Touch_Data.Touch_History_Fault + 8 + i * 18, (array[0]) + 2000); //年
				SetU16(Touch_Data.Touch_History_Fault + 10 + i * 18, (array[1]));	//月
				SetU16(Touch_Data.Touch_History_Fault + 12 + i * 18, (array[2]));	//日
				SetU16(Touch_Data.Touch_History_Fault + 14 + i * 18, (array[3]));	//十
				SetU16(Touch_Data.Touch_History_Fault + 16 + i * 18, (array[4]));    //分
				SetU16(Touch_Data.Touch_History_Fault + 18 + i * 18, (array[5]));    //秒
				SetU16(Touch_Data.Touch_History_Fault + 20 + i * 18, array[6]);	            //代码，传代码最小的一个
			}

		}
		if (read_fault_record.current_page_nub_record_nb < 5)
		{
			for (i = read_fault_record.current_page_nub_record_nb; i < EVERY_FAULTRECORD_PAGE_NB; i++)
			{
				SetU16(Touch_Data.Touch_History_Fault + 6 + i * 18, UINT16_MAX);     //序列号
				SetU16(Touch_Data.Touch_History_Fault + 8 + i * 18, UINT16_MAX);	    //年
				SetU16(Touch_Data.Touch_History_Fault + 10 + i * 18, UINT16_MAX);	//月
				SetU16(Touch_Data.Touch_History_Fault + 12 + i * 18, UINT16_MAX);	//日
				SetU16(Touch_Data.Touch_History_Fault + 14 + i * 18, UINT16_MAX);	//十
				SetU16(Touch_Data.Touch_History_Fault + 16 + i * 18, UINT16_MAX);	//分
				SetU16(Touch_Data.Touch_History_Fault + 18 + i * 18, UINT16_MAX);	//秒
				SetU16(Touch_Data.Touch_History_Fault + 20 + i * 18, UINT16_MAX);	//代码
			}
		}

		SetU16(Touch_Data.Touch_History_Fault + 0, (uint16_t) read_fault_record.total_nub);	        //总记录数
		SetU16(Touch_Data.Touch_History_Fault + 2, (uint16_t) read_fault_record.total_page_nub);    //总页数
		SetU16(Touch_Data.Touch_History_Fault + 4, (uint16_t) read_fault_record.current_page_nub);	//当前页数
		read_fault_record.read_fault_record_flash_flag = false;
	}
	if (read_fault_record.total_nub == 0)	//无记录
	{
		for (i = 0; i < 5; i++)
		{
			SetU16(Touch_Data.Touch_History_Fault + 6 + i * 18, UINT16_MAX);   //序列号
			SetU16(Touch_Data.Touch_History_Fault + 8 + i * 18, UINT16_MAX);	  //年
			SetU16(Touch_Data.Touch_History_Fault + 10 + i * 18, UINT16_MAX);  //月
			SetU16(Touch_Data.Touch_History_Fault + 12 + i * 18, UINT16_MAX);  //日
			SetU16(Touch_Data.Touch_History_Fault + 14 + i * 18, UINT16_MAX);  //十
			SetU16(Touch_Data.Touch_History_Fault + 16 + i * 18, UINT16_MAX);  //分
			SetU16(Touch_Data.Touch_History_Fault + 18 + i * 18, UINT16_MAX);  //秒
			SetU16(Touch_Data.Touch_History_Fault + 20 + i * 18, UINT16_MAX);  //代码
		}
		SetU16(Touch_Data.Touch_History_Fault + 0, 0);	                  //总记录数
		SetU16(Touch_Data.Touch_History_Fault + 2, 0);                     //总页数
		SetU16(Touch_Data.Touch_History_Fault + 4, 0);	                  //当前页数
	}
}

void Write_HisResetRecord_Init(void)                                                //写复位记录初始化
{
	(void) read_eeprom(HISRESETRECORD_PTR_ADDRESS, &write_reset_record.ptr, 2);              //读EEPROM写复位记录指针
	(void) read_eeprom(HISRESETRECORD_NB_ADDRESS, &write_reset_record.total_nub, 2);         //读EEPROM中已写的复位记录总数
	if ((write_event_record.ptr > write_event_record.total_nub) || (write_event_record.ptr > HISEVENTRECORD_NB_MAX) || (write_event_record.total_nub > HISEVENTRECORD_NB_MAX))
	{
		write_reset_record.ptr = 0;
		write_reset_record.total_nub = 0;
		(void) write_eeprom(HISRESETRECORD_PTR_ADDRESS, &write_reset_record.ptr, 2);           //读EEPROM写复位记录指针
		(void) write_eeprom(HISRESETRECORD_NB_ADDRESS, &write_reset_record.total_nub, 2);      //读EEPROM中已写的复位记录总数
	}
	(void) read_eeprom(HISRESETRECORD_PTR_ADDRESS, &read_reset_record.ptr, 2);                 //读复位记录指针
	(void) read_eeprom(HISRESETRECORD_NB_ADDRESS, &read_reset_record.total_nub, 2);            //显示一共有多少条历史复位记录
	read_reset_record.current_page_nub = 1;                                                   //当前显示的页码 第一次进入当前页为第一页
}
void Write_HisResetRecord_Process(void)                                             //写复位记录
{
	uint32_t record_address = 0;
	uint8_t record_reset_array[16];

	//放置于main启动初始化部分
	if (1)
	{
		record_reset_array[0] = showTime.tm_year;                                             //复位时间
		record_reset_array[1] = showTime.tm_mon;
		record_reset_array[2] = showTime.tm_day;
		record_reset_array[3] = showTime.tm_hour;
		record_reset_array[4] = showTime.tm_min;
		record_reset_array[5] = showTime.tm_sec;
		record_reset_array[6] = write_event_record.event_code; //复位代码
		record_reset_array[7] = 0;

		if (write_reset_record.total_nub < HISRESETRECORD_NB_MAX)	   	                        //还没有存满
		{
			record_address = HISRESETRECORD_START_ADDRESS + (write_reset_record.ptr % HISRESETRECORD_NB_MAX) * 16;
			(void) write_eeprom((uint32_t) record_address, record_reset_array, EVERY_RESETRECORD_NB);
			write_reset_record.ptr++;	                                                    //指针加1
			write_reset_record.total_nub++; 		                                        //记录总数加1
		}
		else  //存满后从头开始存储
		{
			write_reset_record.ptr %= HISRESETRECORD_NB_MAX;
			record_address = HISRESETRECORD_START_ADDRESS + (write_reset_record.ptr % HISRESETRECORD_NB_MAX) * 16;
			(void) write_eeprom((uint32_t) record_address, record_reset_array, EVERY_RESETRECORD_NB);
			write_reset_record.ptr++;	                                                    //指针加1 指向下一个待存储的位置
		}

		(void) write_eeprom(HISRESETRECORD_PTR_ADDRESS, &write_reset_record.ptr, 2);
		(void) write_eeprom(HISRESETRECORD_NB_ADDRESS, &write_reset_record.total_nub, 2);

		read_reset_record.total_nub = write_reset_record.total_nub;                           //显示一共有多少条历史复位记录
		read_reset_record.ptr = write_reset_record.ptr;                                       //读复位记录指针
		save_eeprom();
	}
}

void Write_HisEventRecord_Init(void)                                                //写事件记录初始化
{
	(void) read_eeprom(HISEVENTRECORD_PTR_ADDRESS, &write_event_record.ptr, 2);              //读EEPROM写历史事件记录指针
	(void) read_eeprom(HISEVENTRECORD_NB_ADDRESS, &write_event_record.total_nub, 2);         //读EEPROM中已写的历史事件记录总数
	if ((write_event_record.ptr > write_event_record.total_nub) || (write_event_record.ptr > HISEVENTRECORD_NB_MAX) || (write_event_record.total_nub > HISEVENTRECORD_NB_MAX))
	{
		write_event_record.ptr = 0;
		write_event_record.total_nub = 0;
		(void) write_eeprom(HISEVENTRECORD_PTR_ADDRESS, &write_event_record.ptr, 2);           //读EEPROM写历史事件记录指针
		(void) write_eeprom(HISEVENTRECORD_NB_ADDRESS, &write_event_record.total_nub, 2);      //读EEPROM中已写的历史事件记录总数
	}
	(void) read_eeprom(HISEVENTRECORD_PTR_ADDRESS, &read_event_record.ptr, 2);                 //读事件记录指针
	(void) read_eeprom(HISEVENTRECORD_NB_ADDRESS, &read_event_record.total_nub, 2);            //显示一共有多少条历史事件记录
	read_event_record.current_page_nub = 1;                                                   //当前显示的页码 第一次进入当前页为第一页
}
void Write_HisEventRecord_Process(void)                                             //写事件记录
{
	uint32_t record_address = 0;
	uint8_t record_event_array[16];
	bool need_to_write_event = false;

	if (FAULT)
	{
		if (write_event_record.event_code != 6)
		{
			write_event_record.event_code = 6;
			need_to_write_event = true;
		}
	}
	else if ((!FAULT) && EMGSTOP)
	{
		if (write_event_record.event_code != 9)
		{
			write_event_record.event_code = 9;
			need_to_write_event = true;
		}
	}
	else
	{
		if (write_event_record.event_code != Run_Status(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & 0xFFFFFEDF))
		{
			write_event_record.event_code = Run_Status(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & 0xFFFFFEDF);
			need_to_write_event = true;
		}
	}

	if (need_to_write_event == true)
	{
		record_event_array[0] = showTime.tm_year;                                             //事件时间
		record_event_array[1] = showTime.tm_mon;
		record_event_array[2] = showTime.tm_day;
		record_event_array[3] = showTime.tm_hour;
		record_event_array[4] = showTime.tm_min;
		record_event_array[5] = showTime.tm_sec;
		record_event_array[6] = write_event_record.event_code; //事件代码
		record_event_array[7] = 0;

		if (write_event_record.total_nub < HISEVENTRECORD_NB_MAX)	   	                        //还没有存满
		{
			record_address = HISEVENTRECORD_START_ADDRESS + (write_event_record.ptr % HISEVENTRECORD_NB_MAX) * 16;
			(void) write_eeprom((uint32_t) record_address, record_event_array, EVERY_EVENTRECORD_NB);
			write_event_record.ptr++;	                                                    //指针加1
			write_event_record.total_nub++; 		                                        //记录总数加1
		}
		else  //存满后从头开始存储
		{
			write_event_record.ptr %= HISEVENTRECORD_NB_MAX;
			record_address = HISEVENTRECORD_START_ADDRESS + (write_event_record.ptr % HISEVENTRECORD_NB_MAX) * 16;
			(void) write_eeprom((uint32_t) record_address, record_event_array, EVERY_EVENTRECORD_NB);
			write_event_record.ptr++;	                                                    //指针加1 指向下一个待存储的位置
		}

		(void) write_eeprom(HISEVENTRECORD_PTR_ADDRESS, &write_event_record.ptr, 2);
		(void) write_eeprom(HISEVENTRECORD_NB_ADDRESS, &write_event_record.total_nub, 2);

		read_event_record.total_nub = write_event_record.total_nub;                           //显示一共有多少条历史事件记录
		read_event_record.ptr = write_event_record.ptr;                                       //读事件记录指针
		read_event_record.read_event_record_flash_flag = true;
		save_eeprom();
	}
}

void Write_HisFaultRecord_Init(void)                                                //写故障记录初始化
{

	(void) read_eeprom(HISFAULTRECORD_PTR_ADDRESS, &write_fault_record.ptr, 2);                //读EEPROM写历史故障记录指针
	(void) read_eeprom(HISFAULTRECORD_NB_ADDRESS, &write_fault_record.total_nub, 2);           //读EEPROM中已写的历史故障记录总数

	if ((write_fault_record.ptr > write_fault_record.total_nub) || (write_fault_record.ptr > HISFAULTRECORD_NB_MAX) || (write_fault_record.total_nub > HISFAULTRECORD_NB_MAX))
	{

		write_fault_record.ptr = 0;
		write_fault_record.total_nub = 0;

		(void) write_eeprom(HISFAULTRECORD_PTR_ADDRESS, &write_fault_record.ptr, 2);
		(void) write_eeprom(HISFAULTRECORD_NB_ADDRESS, &write_fault_record.total_nub, 2);

	}
	(void) read_eeprom(HISFAULTRECORD_PTR_ADDRESS, &read_fault_record.ptr, 2);                 //读故障记录指针
	(void) read_eeprom(HISFAULTRECORD_NB_ADDRESS, &read_fault_record.total_nub, 2);            //显示一共有多少条历史故障记录

	read_fault_record.current_page_nub = 1;                                                   //当前显示的页码 第一次进入当前页为第一页
}
void Write_HisFaultRecord_Process(void)                                             //写故障记录
{
	uint32_t record_address = 0;
	uint8_t record_fault_array[16];
	uint64_t ret = (((uint64_t) (EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2)) << 32) | ((uint64_t) (EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1));
	uint64_t value = ret & (~write_fault_record.oldfault) & ((uint64_t)0xFFFFFFFFFFFFFFFF);
	write_fault_record.oldfault = ret;

	if (value != 0)
	{
		for (uint32_t i = 1; i <= 64; i++)
		{
			if ((value & 1) == 1)                                                         //故障中的每一位为1的位都写一条记录
			{
				record_fault_array[0] = showTime.tm_year;                                 //故障时间
				record_fault_array[1] = showTime.tm_mon;
				record_fault_array[2] = showTime.tm_day;
				record_fault_array[3] = showTime.tm_hour;
				record_fault_array[4] = showTime.tm_min;
				record_fault_array[5] = showTime.tm_sec;
				record_fault_array[6] = (uint8_t) i;                             		//故障低字
				record_fault_array[7] = 0;                                          //故障高字

				if (write_fault_record.total_nub < HISFAULTRECORD_NB_MAX)	   	   //还没有存满
				{
					record_address = HISFAULTRECORD_START_ADDRESS + (write_fault_record.ptr % HISFAULTRECORD_NB_MAX) * 16;
					(void) write_eeprom((uint32_t) record_address, record_fault_array, EVERY_FAULTRECORD_NB);
					write_fault_record.ptr++;	                                        //指针加1
					write_fault_record.total_nub++; 		                            //记录总数加1
				}
				else                                                                    //存满后从头开始存储
				{
					write_fault_record.ptr %= HISFAULTRECORD_NB_MAX;
					record_address = HISFAULTRECORD_START_ADDRESS + (write_fault_record.ptr % HISFAULTRECORD_NB_MAX) * 16;
					(void) write_eeprom((uint32_t) record_address, record_fault_array, EVERY_FAULTRECORD_NB);
					write_fault_record.ptr++;	                                        //指针加1 指向下一个待存储的位置
				}
				(void) write_eeprom(HISFAULTRECORD_PTR_ADDRESS, &write_fault_record.ptr, 2);
				(void) write_eeprom(HISFAULTRECORD_NB_ADDRESS, &write_fault_record.total_nub, 2);

				read_fault_record.total_nub = write_fault_record.total_nub;               //显示一共有多少条历史事件记录
				read_fault_record.ptr = write_fault_record.ptr;                           //读事件记录指针
				read_fault_record.read_fault_record_flash_flag = true;
			}
			value >>= 1;
		}
		if(read_fault_record.read_fault_record_flash_flag) save_eeprom();
		read_fault_record.read_fault_record_flash_flag = false;
	}
}

void Write_HisAlarmRecord_Init(void)                                                //写故障记录初始化
{

	(void) read_eeprom(HISALARMRECORD_PTR_ADDRESS, &write_alarm_record.ptr, 2);                //读EEPROM写历史告警记录指针
	(void) read_eeprom(HISALARMRECORD_NB_ADDRESS, &write_alarm_record.total_nub, 2);           //读EEPROM中已写的历史告警记录总数

	if ((write_alarm_record.ptr > write_alarm_record.total_nub) || (write_alarm_record.ptr > HISALARMRECORD_NB_MAX) || (write_alarm_record.total_nub > HISALARMRECORD_NB_MAX))
	{

		write_alarm_record.ptr = 0;
		write_alarm_record.total_nub = 0;

		(void) write_eeprom(HISALARMRECORD_PTR_ADDRESS, &write_alarm_record.ptr, 2);
		(void) write_eeprom(HISALARMRECORD_NB_ADDRESS, &write_alarm_record.total_nub, 2);

	}
	(void) read_eeprom(HISALARMRECORD_PTR_ADDRESS, &read_alarm_record.ptr, 2);                 //读告警记录指针
	(void) read_eeprom(HISALARMRECORD_NB_ADDRESS, &read_alarm_record.total_nub, 2);            //显示一共有多少条历史告警记录

	read_alarm_record.current_page_nub = 1;                                                   //当前显示的页码 第一次进入当前页为第一页
}
void Write_HisAlarmRecord_Process(void)                                             //写故障记录
{
	uint32_t record_address = 0;
	uint8_t record_alarm_array[16];
	uint32_t ret = EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word;
	uint32_t value = ret & (~write_alarm_record.oldalarm);
	write_alarm_record.oldalarm = ret;
	if (value != 0)
	{
		for (uint32_t i = 1; i <= 32; i++)
		{
			if ((value & 1) == 1)                                                         //故障中的每一位为1的位都写一条记录
			{
				record_alarm_array[0] = showTime.tm_year;                                 //故障时间
				record_alarm_array[1] = showTime.tm_mon;
				record_alarm_array[2] = showTime.tm_day;
				record_alarm_array[3] = showTime.tm_hour;
				record_alarm_array[4] = showTime.tm_min;
				record_alarm_array[5] = showTime.tm_sec;
				record_alarm_array[6] = (uint8_t) i;                             		//故障低字
				record_alarm_array[7] = 0;                                          //故障高字

				if (write_alarm_record.total_nub < HISALARMRECORD_NB_MAX)	   	   //还没有存满
				{
					record_address = HISALARMRECORD_START_ADDRESS + (write_alarm_record.ptr % HISALARMRECORD_NB_MAX) * 16;
					(void) write_eeprom((uint32_t) record_address, record_alarm_array, EVERY_ALARMRECORD_NB);
					write_alarm_record.ptr++;	                                        //指针加1
					write_alarm_record.total_nub++; 		                            //记录总数加1
				}
				else                                                                    //存满后从头开始存储
				{
					write_alarm_record.ptr %= HISALARMRECORD_NB_MAX;
					record_address = HISALARMRECORD_START_ADDRESS + (write_alarm_record.ptr % HISALARMRECORD_NB_MAX) * 16;
					(void) write_eeprom((uint32_t) record_address, record_alarm_array, EVERY_ALARMRECORD_NB);
					write_alarm_record.ptr++;	                                        //指针加1 指向下一个待存储的位置
				}
				(void) write_eeprom(HISALARMRECORD_PTR_ADDRESS, &write_alarm_record.ptr, 2);
				(void) write_eeprom(HISALARMRECORD_NB_ADDRESS, &write_alarm_record.total_nub, 2);

				read_alarm_record.total_nub = write_alarm_record.total_nub;               //显示一共有多少条历史告警记录
				read_alarm_record.ptr = write_alarm_record.ptr;                           //读告警记录指针
				read_alarm_record.read_alarm_record_flash_flag = true;
			}
			value >>= 1;
		}
		if(read_alarm_record.read_alarm_record_flash_flag) save_eeprom();
		read_alarm_record.read_alarm_record_flash_flag = false;
	}
}

void Read_OnConditionNB_Record(uint16_t ptr, uint16_t total_nub, uint16_t serial_nb, uint32_t base_address, uint8_t *array)
{                                                                                   //读取历史记录
	uint32_t record_address = 0;

	if ((ptr == total_nub) || (serial_nb <= ptr))	                                                //说明还没有循环存储,或者记录号小于ptr
	{
		record_address = base_address + (ptr - serial_nb) * 16;
		(void) read_eeprom((uint32_t) record_address, array, 16);                               //读出此序列号的记录
	}
	else                               //循环存储
	{
		record_address = base_address + (ptr + HISEVENTRECORD_NB_MAX - serial_nb) * 16;
		(void) read_eeprom((uint32_t) record_address, array, 16);                               //读出此序列号的记录
	}
}

uint8_t Return_Max_Count(uint8_t *data, uint16_t num)													//返回最大的PC通讯计数器值
{
	uint8_t temp = 0;
	if (num < 1)
	{
		return 0;
	}
	else
	{
		for (uint8_t i = 0; i < num; i++)
		{
			temp = MAX2(temp,data[i]);
		}
	}
	return temp;
}



