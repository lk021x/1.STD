
#include "includes.h"

/*************************************************************/
//定义往电网放电为正值
//定义从电网取电为负值
/*************************************************************/
// mount -t nfs 192.168.1.10:/nfs_root /mnt/nfs_root -o nolock
//************************************************************/
//定义输出变量
//

int16_t step=1;
int32_t Ramp_Rate_Power=0;
int32_t Peak_Valley_Power;


//削峰填谷
int32_t Peak_Valley_Shift(void)
{
    static int32_t temp_load_pv_power;  //int16范围为最大32767
    static uint8_t loop_delay=0;
    uint8_t sc_time = 1;

	loop_delay++;
    if(loop_delay >= (Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle/300)*sc_time)
	{
		loop_delay = 0;
		temp_load_pv_power = -EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power + EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power;

		//temp_load_pv_power = -EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power + Peak_Valley_Power;

		if(temp_load_pv_power < (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Start_Charge_Power*10)
		{
			Peak_Valley_Power = temp_load_pv_power - (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Start_Charge_Power*10;
		}
		else if(temp_load_pv_power >(int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Start_Discharge_Power*10)
		{
			Peak_Valley_Power = temp_load_pv_power - (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Start_Discharge_Power*10;
		}
		else if((temp_load_pv_power >= (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Start_Charge_Power*10)&&
				(temp_load_pv_power <= (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Start_Discharge_Power*10))
		{
			Peak_Valley_Power = 0;
		}
		else
		{
			Peak_Valley_Power = 0;
		}

		//printf("++ %d   %d   %d  %d \n",   Peak_Valley_Power, EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power , EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power ,Meter_Info[0].Total_active_P);
	}
	else
	{
		//printf("   %d   %d   %d  %d \n",   Peak_Valley_Power, EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power , EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power,Meter_Info[0].Total_active_P );
	}

	return Peak_Valley_Power;
}

//交流测限发
int32_t ACSide_PVLimit(void)
{
    static float ratio_limit;
    static int32_t  temp_power=0;
    static float ratio_max;
    static float ratio_limit_last=1000.0;
    static uint16_t back_delay_time;

    //给定PCS功率
	temp_power = abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Power*10) - ((int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power) ;
	//限光伏功率
	if(DC_BUS_SYS==0) //交流母线
	{
		if(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power>abs((int32_t)Max_anti_reflux_power*10))
		{
			ratio_max = (float)(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)*1000.0
			/ ((int32_t)EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power);
		}
		else
		{
			ratio_max=1000.0;
		}
		//功率
		ratio_limit =
		Pid_PowerCtrl(abs((int32_t)Max_anti_reflux_power*10),EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power, ratio_limit_last,0,PCS_NUM); //61

		if(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power < abs((int32_t)((float)Max_anti_reflux_power*10*0.90)))
		{
			if(back_delay_time++>Parameter_recovery_dalay_time) // 10*300 = 3000
			{
				back_delay_time = 0;
				if(ratio_limit!=0)
				{
					ratio_limit_last = (float)ratio_limit*((float)Parameter_recovery_kp/100.0);
				}
				else
				{
					ratio_limit_last = 10.0;
				}
			}
		}
		else
		{
			ratio_limit_last = ratio_limit;
		}
		if(ratio_limit_last > ratio_max)
		{
			ratio_limit_last = ratio_max;
		}

		//单台光伏出力计算
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
		{
			if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num>0)
			{
				PV_Ctrl_Data[i].PowerLimit_Set = ratio_limit;
				PV_Ctrl_Data[i].PowerLimit_Set = LIMIT(1,PV_Ctrl_Data[i].PowerLimit_Set,1000);
			}
			else
			{
				PV_Ctrl_Data[i].PowerLimit_Set=0;
			}
		}
	}
	return temp_power;
}


#if 0

//新能源平滑
int32_t Ramp_rate_Control(void)
{
	static int32_t pcs_power=0;
	static int16_t index,index2;
	static float pwr,temp_pwr,ratio_3s,pn1,pn2;
	static uint16_t pcs_charge_delay;

	pwr = (float)Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power*10;
	ratio_3s = (float)Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio/(1000.0*(float)Time_Div_Info.Time_Div_Set_Info[0].Ramp_time);
	if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power != 0)
	{
		index++;
		index2++;

		pcs_charge_delay=0;
		if(index>=20)  // 6s 0.5% = 1min 10%
		{
			index=0;

			pn2 = EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power;

			//if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power      > (pwr*(float)(step+1)*ratio_3s))
			if(pn2 > (pn1 + pwr*ratio_3s))
			{
				//step++;
				//temp_pwr = pwr*(float)step*ratio_3s;
				temp_pwr = pn1 + pwr*ratio_3s;
			}
			//else if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power < (pwr*(float)(step-1)*ratio_3s))
			else if(pn2 < (pn1 - pwr*ratio_3s))
			{
				//if(step > 0)step--;
				//temp_pwr = pwr*(float)step*ratio_3s;
				temp_pwr = pn1 - pwr*ratio_3s;
			}
			else
			{
				if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable == ENABLE)
				{
					if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min > Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit)
					{
						//temp_pwr = pwr*(float)(step+1)*ratio_3s;
						temp_pwr = pn1 + pwr*ratio_3s;
					}
					else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min < Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit)
					{
						//temp_pwr = pwr*(float)(step-1)*ratio_3s;
						temp_pwr = pn1 - pwr*ratio_3s;
					}
					else
					{
						pcs_power = 0;
					}
				}
				else if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable == ENABLE)
				{
					if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min > Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit)
					{
						//temp_pwr = pwr*(float)(step+1)*ratio_3s;
						temp_pwr = pn1 + pwr*ratio_3s;
					}
					else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min < Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit)
					{
						//temp_pwr = pwr*(float)(step-1)*ratio_3s;
						temp_pwr = pn1 - pwr*ratio_3s;
					}
					else
					{
						pcs_power  = 0;
					}
				}


			}

			pcs_power = temp_pwr -(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);
			printf("pwr");for(int j=0; j<1; j++) printf(" %f",temp_pwr );printf("\r\n");
			printf("pcs");for(int j=0; j<1; j++) printf(" %d",pcs_power );printf("\r\n");
			printf("pn1");for(int j=0; j<1; j++) printf(" %f",pn1 );     printf("\r\n");
		}

		if(index2>=40) // 12s
		{
			index2=0;
			pn1 = EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power;
		}

	}
	else //光伏没功率自充电
	{
		pn1 = 0;
		pcs_power = 0;

		if(pcs_charge_delay++>2000)
		{
			pcs_charge_delay=0;
			step=1;
			if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable == ENABLE)
			{
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min > Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit)
				{
					//temp_pwr = pwr*(float)(step+1)*ratio_3s;
					pcs_power = 0;
				}
				else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min < Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit)
				{
					//temp_pwr = pwr*(float)(step-1)*ratio_3s;
					pcs_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power*-10;
				}
				//else
				//{
				//	pcs_power = 0;
				//}
			}
			else if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable == ENABLE)
			{
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min > Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit)
				{
					//temp_pwr = pwr*(float)(step+1)*ratio_3s;
					pcs_power = 0;
				}
				else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min < Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit)
				{
					//temp_pwr = pwr*(float)(step-1)*ratio_3s;
					pcs_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power*-10;
				}
				//else
				//{
				//	pcs_power = 0;
				//}
			}
		}
	}
	return pcs_power;
}


#else

uint16_t pcs_charge_delay,pcs_power_fault_delay;
int32_t ppk2;

//新能源平滑
int32_t Ramp_rate_Control(void)
{
	static int32_t pcs_power=0,ppk=0;
	static int16_t index;
	static float pwr,temp_pwr,ratio_3s;

	uint16_t temp1_u16;
	uint32_t temp1_u32;



	if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable == ENABLE)
	{
		//----------------------
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver_Rrc = MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max_Rrc  = MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc  = MAXU16;
		temp1_u16 = 0;
		temp1_u32 = 0;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				temp1_u16++;
				temp1_u32+=BMS_Info[i].Sys_SOC;
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max_Rrc=
								MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max_Rrc,BMS_Info[i].Sys_SOC);
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc=
								MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc,BMS_Info[i].Sys_SOC);
			}
		}
		if (temp1_u16 != 0)
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver_Rrc = temp1_u32 / temp1_u16;
		}
		else
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver_Rrc = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max_Rrc = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc = 0;
		}
	}
	else if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable == ENABLE)
	{

		//电池电压 Max、Min、Average
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver_Rrc = MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max_Rrc  = MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc  = MAXU16;
		temp1_u16 = 0;
		temp1_u32 = 0;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if(PCS_Ready_Status[i])
			{
				temp1_u16++;
				temp1_u32+=PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max_Rrc=
						MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max_Rrc,PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc=
						MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc,PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
			}
		}
		if (temp1_u16 != 0)
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver_Rrc = temp1_u32 / temp1_u16;
		}
		else
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver_Rrc = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max_Rrc = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc = 0;
		}
	}

	//--------------------------------

	pwr = (float)Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power*10;
	ratio_3s = (float)Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio/(2000.0*(float)Time_Div_Info.Time_Div_Set_Info[0].Ramp_time);

	index++;
	if(index>=10)  // 3s 0.5% = 1min 10%
	{
		index=0;
		if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power      > (pwr*(float)(step+1)*ratio_3s))
		{
			step++;
			temp_pwr = pwr*(float)step*ratio_3s;
		}
		else if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power < (pwr*(float)(step-1)*ratio_3s))
		{
			if(step > 0)step--;
			temp_pwr = pwr*(float)step*ratio_3s;
		}
		else
		{
			if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable == ENABLE)
			{
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc > Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit)
				{
					temp_pwr = pwr*(float)(step+1)*ratio_3s;
				}
				else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc < Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit)
				{
					temp_pwr = pwr*(float)(step-1)*ratio_3s;
				}
				else
				{
					//pcs_power = 0;
					//return pcs_power;

					temp_pwr = (float)((int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);
				}
			}
			else if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable == ENABLE)
			{
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc > Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit)
				{
					temp_pwr = pwr*(float)(step+1)*ratio_3s;
				}
				else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc < Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit)
				{
					temp_pwr = pwr*(float)(step-1)*ratio_3s;
				}
				else
				{
					//pcs_power = 0;
					//return pcs_power;
					temp_pwr = (float)((int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);
				}
			}
		}

		ppk = pcs_power;
		pcs_power = temp_pwr -(float)((int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);

		//printf("pwr");for(int j=0; j<1; j++) printf(" %f",temp_pwr );printf("\r\n");
	}

	// pcs全部故障

	if(abs(abs(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power) -abs(ppk)) >
		 (EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*50 + (int32_t)(pwr*ratio_3s)))  //输出和实际差5kw
	{
		if(pcs_power_fault_delay++ >= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number * 20) // 6s
		{
			pcs_power_fault_delay = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number * 20;
			if(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number > EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num)
			{
				step = (int16_t)((float)EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power /(pwr*ratio_3s));
				if(step<1)step=1;
			}
		}
	}
	else
	{
		pcs_power_fault_delay=0;
	}


	if((EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power != 0) && (GRID_RUNNING))
	{
		pcs_charge_delay=0;
		ppk2 = 0;
	}
	else if((EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power == 0) && (GRID_RUNNING))  //光伏没功率自充电
	{
		if((pcs_charge_delay++ >= 2000) && (step < 4))  //光伏没功率且PCS按照斜率走完
		{
			pcs_charge_delay=2010;  // 快速响应充放电
			step=1;
			if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable == ENABLE)
			{
				//----------------------------
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc > Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit)
				{
					pcs_power = 0;
					ppk2 = pcs_power;
				}
				else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min_Rrc < Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit)
				{
					pcs_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power*-10;
					ppk2 = pcs_power;
				}
				else
				{
					pcs_power = ppk2;
				}
			}
			else if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable == ENABLE)
			{
				//---------------------------
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc > Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit)
				{
					pcs_power = 0;
					ppk2 = pcs_power;
				}
				else if(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min_Rrc < Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit)
				{
					pcs_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power*-10;
					ppk2 = pcs_power;
				}
				else
				{
					pcs_power = ppk2;
				}
			}
		}
	}

	//printf("pv    %d\n",       EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power);
	//printf("step  %d  %f\n",   step,(pwr*ratio_3s));
	//printf("pcc1  %f  %d\n",   temp_pwr,EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power);

	//printf("%d   %d   %d   %d   %d\n", pcs_power, step, ppk2,  pcs_charge_delay,  EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power);

	return pcs_power;
}

#endif



//调频
//数字调频和模拟量调频
int32_t FM_Control(void)
{
	static float V_acp,V_rcp,V_mid;
	static int32_t fm_power;

	if(Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital == 1) //模拟量调频
	{
		V_acp = adc.V_adc[0];
		V_rcp = adc.V_adc[1];
		//V_mid  = (float)(Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax - Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin)/2.0;

		V_mid  = (float)Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax/2.0 + (float)Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin/2.0;

		//printf("acp:%f", V_acp);printf("\r\n");

		fm_power = (float)System_Info.Config.System_Power_Set*10*(V_acp - V_mid)/(V_mid-(float)Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin);

		if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == QP) //无功功率
		{
			EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft;

			EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set =(int16_t)((float)System_Info.Config.System_Power_Set*0.3*(V_rcp - V_mid)
																		 /(V_mid-(float)Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin));

			if(EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set > (int16_t)((float)System_Info.Config.System_Power_Set*0.3))
			{
				EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = (int16_t)((float)System_Info.Config.System_Power_Set*0.3);
			}
			else if(EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set < -(int16_t)((float)System_Info.Config.System_Power_Set*0.3))
			{
				EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = -(int16_t)((float)System_Info.Config.System_Power_Set*0.3);
			}
		}
		else if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == PF)
		{
			EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = 0;
			//EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft;
		}
		else if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == SHUT_OFF)
		{
			EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = 0;
			EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft;
		}


		//printf("fmp:%d", fm_power);printf("\r\n");
	}
	else //数字量调频
	{
		fm_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[0].Run_Power*10;
	}

	return fm_power;
	//return 0;
}
































































#if 0
	static uint16_t index;
	static int16_t Num=200;
	static int16_t data[240] = {0}, tmp[240] = {0};
	static float sum1,sum2,sum3,sum4,y_aver,b,b_last,a,a_last,x_aver;

	sum1=0.0;sum2=0.0;sum3=0.0;sum4=0.0;

	data[Num] = EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power;
	for(uint8_t i=0;i<Num;i++)
	{
		tmp[Num]=data[Num];
		tmp[i]=tmp[i+1];
	}
	//printf("tmp:");for(int j=0; j<30; j++) printf(" %d", tmp[j]);printf("\r\n");
	for(uint8_t i=0;i<Num;i++)
	{
		sum1 += tmp[i];
		sum2 += (i+1)*tmp[i];
		sum3 += (i+1)*(i+1);
		sum4 += (i+1);
	}
	y_aver = sum1/(float)Num;
	x_aver = sum4/(float)Num;
	b = (sum2 - Num*x_aver*y_aver)/(sum3-Num*x_aver*x_aver);
	a = y_aver-b*x_aver;


	index++;
	if(index>=200)
	{
		index=0; //1min
		if(b>0)  //rise
		{

		}
		else     //fall
		{

		}
		b_last = b;
		a_last = a;
		//pcs_power = (b*(Num+1) + a) -(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);

		//printf("b     :");for(int j=0; j<1; j++) printf(" %f", b);printf("\r\n");
		//printf("a     :");for(int j=0; j<1; j++) printf(" %f", a);printf("\r\n");
		//printf("b_last:");for(int j=0; j<1; j++) printf(" %f", b_last);printf("\r\n");
		//printf("a_last:");for(int j=0; j<1; j++) printf(" %f", a_last);printf("\r\n");
	}


	if((index<17)&&(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power>100))
	{
		if(index==0)
		{
			p0=EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power;
		}
		index++;
	}
	else
	{
		index=0;
	}


	if((index>=17)&&(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power>100))
	{
		index=0;
		p1=EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power;
		r = (float)(p1-p0)/11.0;

		if(abs(r)>1)
		{
			pwr = p0 ;
		}

		if(abs(r)>1)
		{
			if(r>0)
			{
				pcs_power = (float)pwr*1.01 -(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);
			}
			else
			{
				pcs_power = (float)pwr*0.99 -(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power);
			}
		}
		else
		{
			pcs_power=0;
		}
	}
	else
	{
		//index=0;
	}

#endif

