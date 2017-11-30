/*
 * timer_thread.cpp
 *
 *  Created on: 2017-1-16
 *      Author: root
 */

#include "includes.h"
#include "timer_thread.h"

TimerThread::TimerThread()
{
	m_bStart = true;
	Time_10ms_cur = 0u;
	Time_20ms_cur = 0u;
	//Time_delayms_cur = 0u;
	Time_300ms_cur = 0u;
	Time_500ms_cur = 0u;
	Time_1s_cur = 0u;
	Plus_2s_cur = 0u;
	Plus_3s_cur = 0u;
	Plus_5s_cur = 0u;
	Plus_10s_cur = 0u;
	Plus_1min_cur = 0u;
	Time_10ms_last = 0u;
	Time_20ms_last = 0u;
	//Time_delayms_last = 0u;
	Time_300ms_last = 0u;
	Time_500ms_last = 0u;
	Time_1s_last = 0u;
	Plus_2s_last = 0u;
	Plus_3s_last = 0u;
	Plus_5s_last = 0u;
	Plus_10s_last = 0u;
	Plus_1min_last = 0u;
}

TimerThread::~TimerThread()
{

}

void TimerThread::pro_10ms(void)         //10ms定时器处理函数
{
	iapv2_10ms_proc();
}

void TimerThread::pro_20ms(void)         //20ms定时器处理函数
{
	static uint8_t OP_Led_=UTILS_IO_HIGH;
//-----------------------------------------------------------------
	//急停处理
	EMG_Stop_Proc();
//-----------------------------------------------------------------
	if((GRID_RUNNING)||(ISLAND_RUNNING))
	{
		drv_write_light_gpio(GPIO_LIGHT_RUN, OP_Led_^=UTILS_IO_LOW);
	}
}

void fm_test(void)
{
	//EMS_参数计算
	EMS_Calc();
	//SOC状态计算
	SOC_Status();

	if ((!Switch_to_Island_Status) && (!Switch_to_Grid_Status))
	{
		//SOC控制
		SOC_Control();
		//AGC控制
		AGC_Control();
		//运行模式控制
		RunMode_Control();
	}

	//系统通讯处理
	Com_Process();
}

void TimerThread::pro_300ms(void)        //300ms定时器处理函数
{
	//EMS_参数计算
	EMS_Calc();
	//SOC状态计算
	SOC_Status();

	//--------------------------------------------------系统配电切换逻辑部分-------------------------------
	//系统自检

	if (Sys_Start_Flag && (!Self_Check_OK) && (!Sys_Stop_Flag) && SYS_NO_FAULT && (TIME_IN_PERIOD))
	{
		Self_Check();
	}
	//系统启动
	if (Self_Check_OK && (!Sys_Running_Flag) && (!Sys_Stop_Flag) && (TIME_IN_PERIOD))
	{
		if (Island_Start_Flag)
		{
			Island_Start();
		}
		else if (Grid_Start_Flag)
		{
			Grid_Connected_Start();
		}
		else
		{
			Sys_Ctrl.Sys_Stop = true;
		}
	}
	//系统运行控制
	if ((!Sys_Stop_Flag) && (TIME_IN_PERIOD))
	{
		Running_Control();
	}

	//并网到离网切换
	if (Switch_to_Island_Status && (!Sys_Stop_Flag) && (TIME_IN_PERIOD))
	{
		Switch_to_Grid_Status = false;
		Switch_to_Grid_Flag = 0;
		Switch_to_Island();
	}
	//离网到并网切换
	if (Switch_to_Grid_Status && (!Sys_Stop_Flag) && (TIME_IN_PERIOD))
	{
		Switch_to_Island_Status = false;
		Switch_to_Island_Flag = 0;
		Switch_to_Grid();
	}
	//----------------------------------------------------------------------------------------------------

	if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode != FM)
	{
		if ((!Switch_to_Island_Status) && (!Switch_to_Grid_Status))
		{
			//SOC控制
			SOC_Control();
			//AGC控制
			AGC_Control();
			//运行模式控制
			RunMode_Control();
		}
	}

	if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode == RAMP_RATE_CONTROL)
	{
		Ramp_Rate_Power = Ramp_rate_Control();
	}

	//画面控制
	Touch_Picture_Disp();
	//告警判断
	Alarm_Check();
	//故障判断
	Fault_Check();
	//系统逻辑
	System_Logic();

	//系统停机
	if ((Sys_Stop_Flag) && (!Reset_Require))
	{
		System_Stop();
	}
	//系统复位
	if (Reset_Require && (!EMG_Stop))
	{
		System_Reset();
	}
//----------------------------------------------------------------------------
	if (STOP)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if (!PCS_Com_Err[i])
			{
				if (PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
				{
					PCS_Ind_Ctrl[i].Stop = true;
				}
			}
		}
	}

	//系统通讯处理
	Com_Process();

//-------------------------------------------------------------
	Write_HisEventRecord_Process(); //判断记录历史事件
	Write_HisFaultRecord_Process(); //判断记录历史故障
	Write_HisAlarmRecord_Process(); //判断记录历史告警


	//printf("%d   %d  %d \n", EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power , EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power,Meter_Info[0].Total_active_P );
}

void TimerThread::pro_500ms(void)        //500ms定时器的处理函数
{
	static uint8_t Run_Led=UTILS_IO_HIGH;

//-------------------------------------------------------------

	drv_write_light_gpio(GPIO_LIGHT_RUN, Run_Led^=UTILS_IO_LOW);

//-------------------------------------------------------------
	//PC、PCS、BMS通讯计数器减一
	if (delay_temp > 0)
	{
		delay_temp--;
	}
	if (System_Info.EMS_MONITOR_Web_Http_Com_Count > 0)	//与webserver通讯信号计数(HTTP)
	{
		System_Info.EMS_MONITOR_Web_Http_Com_Count--;
	}
	//PCS
	if (webPageShow.pcs == 1)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if (System_Info.EMS_PCS_Com_Count[i] > 0)
			{
				System_Info.EMS_PCS_Com_Count[i]--;
			}
			else
			{
				PCS_Com_Err[i] = true;
				PCS_Com_OK[i]=false;
				Search_PCS_Info_0x03[i] = true;
				Send_Data_To_PCS_0x10_01[i] = true;
				Send_Data_To_PCS_0x10_02[i] = false;
				memset(&PCS_Info[i], 0, sizeof(PCS_Info[i]));
			}
		}
	}
	//PV
	if (DC_BUS_SYS == 0 && webPageShow.pv == 1)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
		{
			if (System_Info.EMS_PV_Com_Count[i] > 0)
			{
				System_Info.EMS_PV_Com_Count[i]--;
			}
			else
			{
				PV_Com_Err[i] = true;
				Search_PV_Info_0x03[i] = true;
				Send_Data_To_PV_0x10_01[i] = true;
				Send_Data_To_PV_0x10_02[i] = false;
				memset(&PV_Info[i], 0, sizeof(PV_Info[i]));
			}
		}
	}
	//PVS
	if (webPageShow.pvs == 1)
	{
		for(uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number; i++)
		{
			if (System_Info.EMS_PVS_Com_Count[i] > 0)
			{
				System_Info.EMS_PVS_Com_Count[i]--;
			}
			else
			{
				PVS_Com_Err[i] = true;
				memset(&PVS_Info[i], 0, sizeof(PVS_Info[i]));
			}
		}
	}
	//ENV
	if (webPageShow.env == 1)
	{
		for(uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_ENV_Number; i++)
		{
			if (System_Info.EMS_ENV_Com_Count[i] > 0)
			{
				System_Info.EMS_ENV_Com_Count[i]--;
			}
			else
			{
				ENV_Com_Err[i] = true;
				memset(&ENV_Info[i], 0, sizeof(ENV_Info[i]));
			}
		}
	}
	//BMS
	if(System_Info.Config.BMS_Type != 0 && webPageShow.ammeter == 1)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (System_Info.EMS_BMS_CAN_Com_Count[i] > 0)
			{
				System_Info.EMS_BMS_CAN_Com_Count[i]--;
			}
			else
			{
				BMS_Com_Err[i] = true;
				BMS_Com_OK[i] = false;
				memset(&BMS_Info[i], 0, sizeof(BMS_Info[i]));
			}
		}
	}
	//DCDC
	if (DC_BUS_SYS == 1 && System_Info.Config.DCDC_Type != 0 && webPageShow.dcdc == 1)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number; i++)
		{
			if (System_Info.EMS_DCDC_Com_Count[i] > 0)
			{
				System_Info.EMS_DCDC_Com_Count[i]--;
			}
			else
			{
				DCDC_Com_Err[i] = true;
				memset(&DCDC_Info[i], 0, sizeof(DCDC_Info[i]));
			}
		}
	}
	//Meter
	if (webPageShow.ammeter == 1)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number; i++)
		{
			if(System_Info.Config.Meter_Position_Enable[i]==ENABLE)
			{
				if(System_Info.EMS_Meter_Com_Count[i] > 0)
				{
					System_Info.EMS_Meter_Com_Count[i]--;
				}
				else
				{
					Meter_Com_Err[i] = true;
					memset(&Meter_Info[i], 0, sizeof(Meter_Info[i]));
					Meter_System_Init(i);
				}
			}
			else
			{
				Meter_Com_Err[i] = true;
				memset(&Meter_Info[i], 0, sizeof(Meter_Info[i]));
				Meter_System_Init(i);
			}
		}
	}

	// 与DSP通讯信号计数,caoxh
	/*if (System_Info.EMS_DSP_Com_Count[0] > 0)
	{
		System_Info.EMS_DSP_Com_Count[0]--;
	}
	else
	{
		DSP_Com_Err[0] = true;
		memset(&VSG_Info, 0, sizeof(VSG_Info));
	}*/

	//与后台MODBUS通讯计数（以太网）
	for (uint8_t i = 0; i < TCP_MODBUS_LINK_NUM; i++)
	{
		if (System_Info.EMS_MONITOR_Modbus_TCP_Com_Count[i] > 0)
		{
			System_Info.EMS_MONITOR_Modbus_TCP_Com_Count[i]--;
		}
	}
	//与后台IEC104通讯计数（以太网）
	for (uint8_t i = 0; i < TCP_IEC104_LINK_NUM; i++)
	{
		if (System_Info.EMS_MONITOR_IEC104_TCP_Com_Count[i] > 0)
		{
			System_Info.EMS_MONITOR_IEC104_TCP_Com_Count[i]--;
		}
	}
}

void TimerThread::pro_1s(void)           //1s定时器的处理函数
{
	static bool Already_In = false;

	//获取最新日期
	get_sys_time();

	//每半时向触摸屏请求一次对时
	if (showTime.tm_min == 0x00 || showTime.tm_min == 0x30)
	{
		if (Already_In == false)			//防止30整时，多次进入
		{
			if (delay10s == false)			//上位机没有对时的话
			{
				Already_In = true;
				Touch_Set_Time_Flag = 1;
			}
		}
	}
	else
	{
		Already_In = false;
	}

	//判断是否到另一天，一些数据要清0
	Next_Day_Data_Process();


	if(ENABLE != System_Info.Config.Power_Distribution_Enable)  //非配电情况下
	{
		System_Run_Info.Load_Energy_Acc += (uint64_t)Meter_Info[1].Total_active_P;  //0.1kw
		System_Run_Info.Day_Load_Energy_Acc += (uint64_t)Meter_Info[1].Total_active_P; //0.1kw
		System_Run_Info.Day_Load_Energy = (uint16_t) (System_Run_Info.Day_Load_Energy_Acc / 3600);//0.1kwh
		System_Run_Info.Total_Load_Energy = (uint32_t) (System_Run_Info.Load_Energy_Acc / 36000);//1kwh
		write_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);
		write_eeprom(DAY_LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Day_Load_Energy_Acc, 8);
		write_eeprom(TOTAL_LOAD_ENERGY_ADDRESS, &System_Run_Info.Total_Load_Energy, 4);
		write_eeprom(DAY_LOAD_ENERGY_ADDRESS, &System_Run_Info.Day_Load_Energy, 2);
	}
	else  // 配电情况下
	{
		//充放电功率计算
		if (Load_Power_Status != 0)
		{
			System_Run_Info.Load_Energy_Acc += (uint64_t)Meter_Info[1].Total_active_P;  //0.1kw
			System_Run_Info.Day_Load_Energy_Acc += (uint64_t)Meter_Info[1].Total_active_P; //0.1kw
			System_Run_Info.Day_Load_Energy = (uint16_t) (System_Run_Info.Day_Load_Energy_Acc / 3600);//0.1kwh
			System_Run_Info.Total_Load_Energy = (uint32_t) (System_Run_Info.Load_Energy_Acc / 36000);//1kwh
			write_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);
			write_eeprom(DAY_LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Day_Load_Energy_Acc, 8);
			write_eeprom(TOTAL_LOAD_ENERGY_ADDRESS, &System_Run_Info.Total_Load_Energy, 4);
			write_eeprom(DAY_LOAD_ENERGY_ADDRESS, &System_Run_Info.Day_Load_Energy, 2);
		}
	}
	System_Run_Info.Day_Charge_Energy = 0;
	System_Run_Info.Total_Charge_Energy = 0;
	System_Run_Info.Day_Discharge_Energy = 0;
	System_Run_Info.Total_Discharge_Energy = 0;
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		System_Run_Info.Day_Charge_Energy += PCS_Info[i].PCS3x.Rmeasure.Day_Charge_Energe;  //PCS日充电量
		System_Run_Info.Total_Charge_Energy += PCS_Info[i].PCS3x.Rmeasure.Total_Charge_Energe;  //PCS总充电量
		System_Run_Info.Day_Discharge_Energy += PCS_Info[i].PCS3x.Rmeasure.Day_Discharge_Energe;  //PCS日放电量
		System_Run_Info.Total_Discharge_Energy += PCS_Info[i].PCS3x.Rmeasure.Total_Discharge_Energe;  //PCS总放电量
	}
	System_Run_Info.Day_PV_Energy = 0;
	System_Run_Info.Total_PV_Energy = 0;
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
	{
		System_Run_Info.Day_PV_Energy += PV_Info[i].PV3x.RunParameter.Day_Generation;  //PV日发电量
		System_Run_Info.Total_PV_Energy += PV_Info[i].PV3x.RunParameter.Total_Generation;  //PV总发电量
	}
}

void TimerThread::pro_2s(void)           //2s定时器的处理函数
{
	static uint8_t temp = 0;
	static uint16_t count[5]={0};
	static int64_t power_acc[5]={0};
	static int32_t power_average[5]={0};
	static uint8_t Time_Count = 0;
	uint16_t i=0;

	//--------------------------------------------------
	if (delay10s == true)											//用这个条件判断，需要给触摸屏来对时了
	{
		if (temp < 5)												//对时5次
		{
			temp++;
			if (IO_Set_Touch_Req == 0)								//上升沿和下降沿对时
			{
				IO_Set_Touch_Req = 1;
			}
			else
			{
				IO_Set_Touch_Req = 0;
			}
		}
		else														//对时5次后结束对时
		{
			temp = 0;
			delay10s = false;
			IO_Set_Touch_Req = 0;
		}
	}

	//曲线时间设置
	if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode != RAMP_RATE_CONTROL)
	{
		Time_900_seconds = 900; //15min
	}
	else
	{
		Time_900_seconds = 60; //1min
	}
	//并网曲线
	if((GRID_RUNNING)||(ISLAND_RUNNING))
	{
		if(count[0] < Time_900_seconds / 2)
		{
			//grid
			power_acc[0] += -EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power;
			count[0]++;
			if(count[0]!=0)power_average[0] = power_acc[0]/count[0];
			//pcs
			power_acc[1] += EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power;
			count[1]++;
			if(count[1]!=0)power_average[1] = power_acc[1]/count[1];
			//pv
			power_acc[2] += EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power;
			count[2]++;
			if(count[2]!=0)power_average[2] = power_acc[2]/count[2];
			//load
			power_acc[3] += EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power;
			count[3]++;
			if(count[3]!=0)power_average[3] = power_acc[3]/count[3];
			//曲线保存
			Save_Power_To_Buffer(Grid_Power_Curve_Buffer, power_average[0], PCS_Power_Curve_Buffer, power_average[1],  \
					             PV_Power_Curve_Buffer,   power_average[2], Load_Power_Curve_Buffer,power_average[3]);
		}
		else
		{
			for(i=0;i<5;i++)
			{
				count[i] = 0;
				power_acc[i]=0;
			}
		}
	}

	Time_Count++;
	//每采样时间的1/3来保存数据曲线
	if (Time_Count >= Time_900_seconds/6)
	{
		Time_Count = 0;
	    //写当前的年月日时分秒
		(void)write_eeprom(YEAR_LAST_ADDRESS,&Com_Data.Year_Last,1);		//年
		(void)write_eeprom(MONTH_LAST_ADDRESS,&Com_Data.Month_Last,1);		//月
		(void)write_eeprom(DATE_LAST_ADDRESS,&Com_Data.Day_Last,1);			//日
		(void)write_eeprom(HOUR_LAST_ADDRESS,&Com_Data.Hour_Last,1);		//时
		(void)write_eeprom(MINUTE_LAST_ADDRESS,&Com_Data.Minute_Last,1);	//分
		(void)write_eeprom(SECOND_LAST_ADDRESS,&Com_Data.Second_Last,1);	//秒
		Write_Day_Curve_Array();			//写当天曲线数据
	}
}

void TimerThread::pro_1min(void)         //1min定时器的处理函数
{
	//1分钟写一次运行记录到flash
	if(Sys_Running_Flag)
	{
		Write_Histroy_RunRecord();
	}
	//计算系统离网模式下的运行时间
	if (ISLAND_STARTING || ISLAND_RUNNING)
	{
		System_Run_Info.MG_Run_min++;
		System_Run_Info.Day_MG_Run_Time++;
		System_Run_Info.Total_MG_Run_Time = System_Run_Info.MG_Run_min / 60;
		write_eeprom(MG_RUNMIN_ADDRESS, &System_Run_Info.MG_Run_min, 4);
		write_eeprom(DAY_MG_RUNTIME_ADDRESS, &System_Run_Info.Day_MG_Run_Time, 2);
		write_eeprom(TOTAL_MG_RUNTIME_ADDRESS, &System_Run_Info.Total_MG_Run_Time, 4);
	}
	//计算系统并网模式下的运行时间
	if (GRID_STARTING || GRID_RUNNING)
	{
		System_Run_Info.Grid_Run_min++;
		System_Run_Info.Day_Grid_Run_Time++;
		System_Run_Info.Total_Grid_Run_Time = System_Run_Info.Grid_Run_min / 60;
		write_eeprom(GRID_RUNMIN_ADDRESS, &System_Run_Info.Grid_Run_min, 4);
		write_eeprom(DAY_GRID_RUNTIME_ADDRESS, &System_Run_Info.Day_Grid_Run_Time, 2);
		write_eeprom(TOTAL_GRID_RUNTIME_ADDRESS, &System_Run_Info.Total_Grid_Run_Time, 4);
	}
	//计算负载供电时间
	if (Load_Power_Status != 0)
	{
		System_Run_Info.Load_Supply_min++;
		System_Run_Info.Day_Load_Supply_Time++;
		System_Run_Info.Total_Load_Supply_Time = System_Run_Info.Load_Supply_min / 60;
		write_eeprom(LOAD_SUPPLY_MIN_ADDRESS, &System_Run_Info.Load_Supply_min, 4);
		write_eeprom(DAY_LOAD_SUPPLY_TIME_ADDRESS, &System_Run_Info.Day_Load_Supply_Time, 2);
		write_eeprom(TOTAL_LOAD_SUPPLY_TIME_ADDRESS, &System_Run_Info.Total_Load_Supply_Time, 4);
	}
	//计算系统故障时间
	if (FAULT)
	{
		System_Run_Info.Fault_min++;
		System_Run_Info.Day_Fault_Time++;
		System_Run_Info.Total_Fault_Time = System_Run_Info.Fault_min / 60;
		write_eeprom(FAULT_MIN_ADDRESS, &System_Run_Info.Fault_min, 4);
		write_eeprom(DAY_FAULT_TIME_ADDRESS, &System_Run_Info.Day_Fault_Time, 2);
		write_eeprom(TOTAL_FAULT_TIME_ADDRESS, &System_Run_Info.Total_Fault_Time, 4);
	}
	//计算PCS工作的充放电时间
	if (ISLAND_STARTING || ISLAND_RUNNING || GRID_STARTING || GRID_RUNNING)
	{
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power < 0)
		{
			System_Run_Info.Charge_min++;
			System_Run_Info.Day_Charge_Time++;    //PCS充电时间
			System_Run_Info.Total_Charge_Time = System_Run_Info.Charge_min / 60;  //PCS总充电时间
			write_eeprom(CHARGE_MIN_ADDRESS, &System_Run_Info.Charge_min, 4);
			write_eeprom(DAY_CHARGE_TIME_ADDRESS, &System_Run_Info.Day_Charge_Time, 2);
			write_eeprom(TOTAL_CHARGE_TIME_ADDRESS, &System_Run_Info.Total_Charge_Time, 4);
		}
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power > 0)
		{
			System_Run_Info.Discharge_min++;
			System_Run_Info.Day_Discharge_Time++;  //PCS日放电时间
			System_Run_Info.Total_Discharge_Time = System_Run_Info.Discharge_min / 60;  //PCS总放电时间
			write_eeprom(DISCHARGE_MIN_ADDRESS, &System_Run_Info.Discharge_min, 4);
			write_eeprom(DAY_DISCHARGE_TIME_ADDRESS, &System_Run_Info.Day_Discharge_Time, 2);
			write_eeprom(TOTAL_DISCHARGE_TIME_ADDRESS, &System_Run_Info.Total_Discharge_Time, 4);
		}
		System_Run_Info.Run_min++;
		System_Run_Info.Day_Run_Time++;  //PCS日放电时间
		System_Run_Info.Total_Run_Time = System_Run_Info.Run_min / 60;  //PCS总放电时间
		write_eeprom(RUNMIN_ADDRESS, &System_Run_Info.Run_min, 4);
		write_eeprom(DAY_RUNTIME_ADDRESS, &System_Run_Info.Day_Run_Time, 2);
		write_eeprom(TOTAL_RUNTIME_ADDRESS, &System_Run_Info.Total_Run_Time, 4);
	}
	//计算光伏逆变器运行时间
	if (!DC_BUS_SYS)
	{
		bool PV_Run_Flag = false;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
		{
			if (!PV_Com_Err[i] && PV_Info[i].PV3x.RunParameter.Total_DC_Power > 0)
			{
				PV_Run_Flag = true;
			}
		}
		if (PV_Run_Flag)
		{
			System_Run_Info.PV_Run_min++;
			System_Run_Info.Day_PV_Run_Time++;  //PV日运行时间
			System_Run_Info.Total_PV_Run_Time = System_Run_Info.PV_Run_min / 60;  //PV总运行时间
			write_eeprom(PV_RUNMIN_ADDRESS, &System_Run_Info.PV_Run_min, 4);
			write_eeprom(DAY_PV_RUNTIME_ADDRESS, &System_Run_Info.Day_PV_Run_Time, 2);
			write_eeprom(TOTAL_PV_RUNTIME_ADDRESS, &System_Run_Info.Total_PV_Run_Time, 4);
		}
	}
}

inline int TimerThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	cur = (int64_t)tv.tv_sec*1000+(int64_t)tv.tv_usec/1000; //毫秒

	if(last==0 || cur<last){last=cur;return 0;}
	if(cur > last && (cur-last) >= ms){ //定时时间到
		last = cur;
		return 1;
	}else{
		return 0;
	}
}

void TimerThread::run()
{
	static bool sw = false;
	static int bak = 0;
	bool first = true;

	while (m_bStart)
	{
		if(CheckTimeout(10, Time_10ms_cur, Time_10ms_last))
		{
			pro_10ms();
		}

		if (CheckTimeout(20, Time_20ms_cur, Time_20ms_last))
		{
			mutex_for_timer.lock();
			pro_20ms();
			mutex_for_timer.unlock();
		}

		/*if (CheckTimeout(EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay, Time_delayms_cur, Time_delayms_last))
		{
			mutex_for_timer.lock();
			pro_delay_nms();
			mutex_for_timer.unlock();
		}*/

		if (CheckTimeout(300, Time_300ms_cur, Time_300ms_last))
		{
			mutex_for_timer.lock();
			pro_300ms();
			mutex_for_timer.unlock();
		}

		if (CheckTimeout(500, Time_500ms_cur, Time_500ms_last))
		{
			mutex_for_timer.lock();
			pro_500ms();
			mutex_for_timer.unlock();
		}

		if (CheckTimeout(1000, Time_1s_cur, Time_1s_last))
		{
			mutex_for_timer.lock();
			pro_1s();
			mutex_for_timer.unlock();
		}

		if (CheckTimeout(2000, Plus_2s_cur, Plus_2s_last))
		{
			mutex_for_timer.lock();
			pro_2s();
			mutex_for_timer.unlock();
		}

		if (CheckTimeout(3000, Plus_10s_cur, Plus_10s_last))
		{
			mutex_for_timer.lock();
			if((!reboot) && Com_Data.Load_Default_Flag != 0xAA)
			{
				if(sw){
					sw = false;
					bak = save_eeprom();
				}else{
					sw = true;
					if(bak){
						bak_eeprom();
					}else if(first){
						bak_eeprom();
						first = false;
					}
				}
			}
			mutex_for_timer.unlock();
		}

		if (CheckTimeout(60000, Plus_1min_cur, Plus_1min_last))
		{
			mutex_for_timer.lock();
			pro_1min();
			mutex_for_timer.unlock();
		}

		if(Com_Data.Load_Default_Flag == 0xAA && (!Sys_Running_Flag))  //恢复出场默认值
		{
			mutex_for_timer.lock();
			Com_Data.Load_Default_Flag = 0x55;
			EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number = 0;
			Sys_Ctrl.Sys_Stop = true;
			Load_Default_Process();
			if(Com_Data.Load_Defalut_Net_Flag == 0xAA) Load_Default_Net_Process();
			Com_Data.Load_Defalut_Net_Flag = 0x55;
			save_eeprom();
			bak_eeprom();
			mutex_for_timer.unlock();
		}

		if(reboot && Com_Data.Load_Default_Flag != 0xAA)
		{
			sleep(1);
			System_Reboot();
		}

		usleep(2);
	}
}

