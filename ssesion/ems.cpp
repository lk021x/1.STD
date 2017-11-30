#include "includes.h"

//并网启动
void Grid_Connected_Start()
{
	static bool PCS_Start_OK;
	static bool PV_Start;
	static bool PV_Start_OK;

	static uint16_t GC_Time=3;
	
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<2;

	switch(GC_Flag)
	{
		case 0://初始化
			PCS_Start_OK=false;
			PV_Start=false;
			PV_Start_OK=false;
			GC_Flag=1;
			break;
		
		case 1: //电网电压判断
			if(ENABLE==System_Info.Config.Power_Distribution_Enable)
			{
				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
				if (Grid_Exist) // 电网有电信号接点，接点闭合DIN为1
				{
					GC_Counter[0]++;
					if(GC_Counter[0]>1*GC_Time)
					{
						GC_Flag=2;
						GC_Counter[0]=0;
					}
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<20;			//电网供电启动失败
				}
			}
			else
			{
				if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage)&& MG_FREQUENCY_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency))
				{
					GC_Counter[12]++;
					if(GC_Counter[12]>5*GC_Time)
					{
						if(DC_BUS_SYS==1) //直流母线
						{
							GC_Flag=9;
						}
						else
						{
							GC_Flag=7; //交流母线
						}
						GC_Counter[12]=0;
					}
				}
				else
				{
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Ind_Ctrl[i].Stop=true;
					}
				}
				GC_Counter[13]++;
				if(GC_Counter[13]>20*GC_Time)
				{
					GC_Counter[13]=0;
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<20;			//电网供电启动失败
				}
			}
			break;

		case 2:			//微网侧接触器检查
			(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);
			MGrid_Contactor_Control_State = OP_LOW;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

			if (DIN(MGrid_Contacter))
			{
				GC_Counter[1] = 0;
			}
			else
			{
				GC_Counter[1]++;
				if(GC_Counter[1]>1*GC_Time)
				{
					GC_Flag=3;
					GC_Counter[1]=0;
				}
			}
			
			GC_Counter[2]++;
			if(GC_Counter[2]>5*GC_Time)
			{
				//EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<19;			//电网供电启动超时
				//EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<9;			//微网侧接触器故障
				Sys_Ctrl.Sys_Stop = true;
				MGRID_CONTACTOR_FAULT_SET;
				GC_Counter[2]=0;
			}
			break;
		
		case 3: //合电网侧接触器
			(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_HIGH,0);
			Grid_Contactor_Control_State = OP_HIGH;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

			if(!DIN(Grid_Contacter))
			{
				GC_Counter[3] = 0;
			}
			else
			{
				GC_Counter[3]++;
				if(GC_Counter[3]>1*GC_Time)
				{
					Load_Power_Status=1;
					if(GRID_TYPE==0) // 不允许PCS并电网
					{
						GC_Flag=9;
					}
					else
					{
						if(MGRID_CONTACTOR_FAULT)
						{
							GC_Flag=9;
						}
						else
						{
							GC_Flag=4;
						}
					}
					GC_Counter[3]=0;
				}
			}
	
			GC_Counter[4]++;
			if(GC_Counter[4]>5*GC_Time)
			{
				if(Grid_Exist)
				{
					GRID_CONTACTOR_FAULT_SET;
				}
				Sys_Ctrl.Sys_Stop = true;
				GC_Counter[4]=0;				
			}
			break;
		case 4: //合微网侧接触器
			(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_HIGH,0);
			MGrid_Contactor_Control_State = OP_HIGH;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

			if(!DIN(MGrid_Contacter))
			{
				GC_Counter[5] = 0;
			}
			else
			{
				GC_Counter[5]++;
				if(GC_Counter[5]>1*GC_Time)
				{
					GC_Flag=5;
					GC_Counter[5]=0;
				}
			}
	
			GC_Counter[6]++;
			if(GC_Counter[6]>5*GC_Time)
			{
			    MGRID_CONTACTOR_FAULT_SET;
				(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);
				MGrid_Contactor_Control_State = OP_LOW;
				GC_Flag=9;
				GC_Counter[6]=0;				
			}			
			break;
		case 5: //PCS启动条件判断
			if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage)&& MG_FREQUENCY_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency))
			{
				GC_Counter[7]++;
				if(GC_Counter[7]>EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*GC_Time)
				{
					//GC_Flag=6;     	//原来的控制方式，启动过程中启动PCS
					//GC_Flag = 9;      //启动过程中不启动PCS，只闭合微网接触器，在AGC中启动PCS
					GC_Counter[7]=0;
				}
			}
			
			GC_Counter[8]++;
			if(GC_Counter[8]>5*GC_Time)
			{
				if(DC_BUS_SYS==1)
				{
					GC_Flag=9;
				}
				else
				{
					GC_Flag=7;
				}
				GC_Counter[9]=0;	
				GC_Counter[19]=0;					
			}
			break;
		case 6: //PCS启动
			PCS_Start_OK=true;
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				PCS_Ctrl_Data[i].Const_V_Mode_V=EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V;
				PCS_Ctrl_Data[i].Const_V_Mode_I=0;
				if(!PCS_Com_Err[i])
				{
					if((!PCS_GRID_CONNECTED_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)) ||
						 (PCS_Info[i].PCS4x.RAdjust.Const_V_Mode_V!=PCS_Ctrl_Data[i].Const_V_Mode_V) ||
						 (PCS_Info[i].PCS4x.RAdjust.Const_V_Mode_I!=PCS_Ctrl_Data[i].Const_V_Mode_I))
					{
						PCS_Ind_Ctrl[i].Grid_Const_V_Mode=true;
						PCS_Start_OK=false;
					}
				}
			}
			
			GC_Counter[9]++;
			GC_Counter[19]++;
			if((PCS_Start_OK&&(GC_Counter[19]>5*GC_Time))||GC_Counter[9]>15*GC_Time)
			{
				if(DC_BUS_SYS==1)
				{
					GC_Flag=9;
				}
				else
				{
					GC_Flag=7;
				}
				GC_Counter[9]=0;
				GC_Counter[19]=0;
			}
			break;
		case 7: //PV启动条件判断
			GC_Flag=9;
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
			{
				if(!PV_Com_Err[i])
				{
					GC_Flag=8;
				}
			}
			break;
		case 8: //PV启动
			if(!PV_Start)
			{
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					PV_Start=true;
					if(!PV_Com_Err[i])
					{
						PV_Ctrl_Data[i].PowerLimit_Set=1;
						if(PV_Info[i].PV4x.ParaSet.PowerLimit_Set!=PV_Ctrl_Data[i].PowerLimit_Set)
						{
							PV_Ind_Ctrl[i].Active_Power_Limit=true;
							PV_Start=false;
						}
					}
				}
			}
			else
			{
				PV_Start_OK=true;
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					if((!PV_POWER_ON(PV_Info[i].PV4x.ParaSet.Power_ONOFF))&&(!PV_Com_Err[i]))
					{
						PV_Start_OK=false;
						PV_Ind_Ctrl[i].Power_On=true;
					}
				}
			}
			GC_Counter[10]++;
			if(PV_Start_OK || GC_Counter[10]>20*GC_Time)
			{
				GC_Flag=9;
				GC_Counter[10]=0;
			}	
			break;
		case 9: //启动成功
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<4;
			//(void)drv_ioctl(gt_glp.fdido,OP_LED|DO_STATE_HIGH,0);
			Load_Power_Status=1;   //电网
			Sys_Running_Flag=true;
			Grid_Start_Flag=false;
			break;
		default:
			break;
	}
}

//孤岛启动
void Island_Start()
{
	static bool PCS_Soft_Start_OK;
    static bool PCS_Modify_OK;
    static bool PCS_Master_Modify_OK;
	static bool PV_Start;
	static bool PV_Start_OK;
	static uint8_t PCS_tsn;

	static uint16_t IS_Time=3;
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<1;

	switch(IS_Flag)
	{
		case 0://初始化
			PCS_Soft_Start_OK=false;
 			PCS_Master_Modify_OK=false;
 			PCS_Modify_OK=false;
			PV_Start=false;
			PV_Start_OK=false;
			PCS_tsn=0;
			IS_Flag=1;
			break;
		case 1:	//判断微网电压
			EMS_Info.EMS3x.Island_Start_Info.RCommunication.Status=1<<0;
			if(ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage)) //微网没有电压，则符合启动条件
			{
				IS_Counter[0]++;
				if(IS_Counter[0]>2*IS_Time)
				{
					if(ENABLE==System_Info.Config.Power_Distribution_Enable) //使能配电
					{
						IS_Flag=2;
					}
					else
					{
						IS_Flag=4;
					}
					IS_Counter[0]=0;
				}
			}
			else
			{
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<18;			//微网供电启动失败
			}
			break;
		case 2:  //断电网开关
			(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_LOW,0);	//断电网接触器
			Grid_Contactor_Control_State = OP_LOW;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);


			if (DIN(Grid_Contacter))
			{
				IS_Counter[1]=0;
			}
			else
			{
				IS_Counter[1]++;
				if(IS_Counter[1]>1)
				{
					IS_Flag=3;
					IS_Counter[1]=0;
				}
			}			
			IS_Counter[2]++;
			if(IS_Counter[2]>5*IS_Time)
			{
				Sys_Ctrl.Sys_Stop = true;
				GRID_CONTACTOR_FAULT_SET;
				IS_Counter[2]=0;
			}
			break;	
		case 3:  //断微网开关
			(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);	//断微网接触器
			MGrid_Contactor_Control_State = OP_LOW;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

			if(DIN(MGrid_Contacter))
			{
				IS_Counter[3]=0;
			}
			else
			{
				IS_Counter[3]++;
				if(IS_Counter[3]>1)
				{
					IS_Flag=4;
					IS_Counter[3]=0;
				}
			}			
			IS_Counter[4]++;
			if(IS_Counter[4]>5*IS_Time)
			{
				Sys_Ctrl.Sys_Stop = true;
				MGRID_CONTACTOR_FAULT_SET;
				IS_Counter[4]=0;
			}
			break;			
		case 4:	//PCS启动条件判断
			//if(ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage)&&ZERO_FREQUENCY(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency))
			if(ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
			{
				IS_Counter[5]++;
				if(IS_Counter[5]>EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*IS_Time)
				{
					IS_Flag=5;
					IS_Counter[5]=0;
				}
			}
			else
			{
				IS_Counter[5]=0;
			}			
			IS_Counter[6]++;
			if(IS_Counter[6]>10*IS_Time)
			{
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<17;			//微网供电启动超时
				IS_Counter[6]=0;						
			}
			break;
		case 5:			//PCS启动
			if(!PCS_Soft_Start_OK)
			{
                if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
                {
    				if(PCS_tsn<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
    				{
    					//if(PCS_INVERT_STATUS(PCS_Info[PCS_tsn].PCS3x.RCommunication.Run_Status)&& PCS_VOLTAGE_OK(PCS_Info[PCS_tsn].PCS3x.Rmeasure.VAB)) //无缓启
    					if(PCS_INVERT_STATUS(PCS_Info[PCS_tsn].PCS3x.RCommunication.Run_Status)&& PCS_SOFT_VOLTAGE_OK(PCS_Info[PCS_tsn].PCS3x.Rmeasure.VAB)) //缓启
    					{
    						IS_Counter[7]++;
    						if(IS_Counter[7]>1*IS_Time)
    						{
    							PCS_tsn++;
    							IS_Counter[7]=0;
    						}
    					}
    					else
    					{
							if( System_Info.Config.PCS_Type  == PCS_Power_SC500TL)
							{
								PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=SOFT_START_315_V;
							}
							else
							{
								PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=SOFT_START_400_V;
							}
    						//PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
    						PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_F=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
    						PCS_Ind_Ctrl[PCS_tsn].Invert=true;
    						IS_Counter[7]=0;
    					}
    				}
    				else
    				{
    					if(!ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
    					{
    						IS_Counter[8]++;
    						if(IS_Counter[8]>4*IS_Time)
    						{
    							PCS_Soft_Start_OK=true;
    							IS_Counter[8]=0;
    						}
    					}
    					else
    					{
    						IS_Counter[8]=0;
    						EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<18;			//微网供电启动失败
    					}
    				}
				}
                else //部分正常
                {
    				if(PCS_tsn<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
    				{

    					//if((PCS_INVERT_STATUS(PCS_Info[PCS_tsn].PCS3x.RCommunication.Run_Status)&&PCS_VOLTAGE_OK(PCS_Info[PCS_tsn].PCS3x.Rmeasure.VAB))||(!PCS_Ready_Status[PCS_tsn])) //无缓启
    					if((PCS_INVERT_STATUS(PCS_Info[PCS_tsn].PCS3x.RCommunication.Run_Status)&&PCS_SOFT_VOLTAGE_OK(PCS_Info[PCS_tsn].PCS3x.Rmeasure.VAB))||(!PCS_Ready_Status[PCS_tsn])) //缓启
    					{
    						IS_Counter[7]++;
    						if(IS_Counter[7]>1*IS_Time)
    						{
    							PCS_tsn++;
    							IS_Counter[7]=0;
    						}
    					}
    					else
    					{
    						if( System_Info.Config.PCS_Type  == PCS_Power_SC500TL)
							{
								PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=SOFT_START_315_V;
							}
							else
							{
								PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=SOFT_START_400_V;
							}
    						//PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
    						PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_F=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
    						PCS_Ind_Ctrl[PCS_tsn].Invert=true;
    						IS_Counter[7]=0;
    					}
    				}
    				else
    				{
    					if(!ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
    					{
    						IS_Counter[8]++;
    						if(IS_Counter[8]>4*IS_Time)
    						{
    							PCS_Soft_Start_OK=true;
    							IS_Counter[8]=0;
    						}
    					}
    					else
    					{
    						IS_Counter[8]=0;
    						EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<18;			//微网供电启动失败
    					}
    				}
				}
			}

			//修改主PCS输出电压为额定值
			if(PCS_Soft_Start_OK && (!PCS_Master_Modify_OK))
			{
				if(!ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
				{
					IS_Counter[9]++;
					if(IS_Counter[9]>2*IS_Time)
					{
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
						{
							if(PCS_Info[i].PCS3x.Rmeasure.Master_Slave==1)
							{
								PCS_Ctrl_Data[i].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
							}
							else
							{
								//PCS_Ctrl_Data[i].Ind_Inverter_V=SOFT_START_V;
								if( System_Info.Config.PCS_Type  == PCS_Power_SC500TL)
								{
									PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=SOFT_START_315_V;
								}
								else
								{
									PCS_Ctrl_Data[PCS_tsn].Ind_Inverter_V=SOFT_START_400_V;
								}
							}
						}
						PCS_Master_Modify_OK=true;
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
						{
							if(!PCS_Com_Err[i])
							{
								if(PCS_Info[i].PCS4x.RAdjust.Ind_Inverter_V!=PCS_Ctrl_Data[i].Ind_Inverter_V)
								{
									PCS_Ind_Ctrl[i].Invert=true;
									PCS_Master_Modify_OK=false;
								}
							}
						}
					}
				}
				else
				{
					IS_Counter[9]=0;
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<18;         //微网供电启动失败
				}
			}

			//修改PCS输出电压为额定值
			if(PCS_Master_Modify_OK && (!PCS_Modify_OK) && MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
			{
				IS_Counter[11]++;
				if(IS_Counter[11]>2*IS_Time)
				{
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Ctrl_Data[i].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
					}
					PCS_Modify_OK=true;
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						if(!PCS_Com_Err[i])
						{
							if(PCS_Info[i].PCS4x.RAdjust.Ind_Inverter_V!=PCS_Ctrl_Data[i].Ind_Inverter_V)
							{
								PCS_Ind_Ctrl[i].Invert=true;
								PCS_Modify_OK=false;
							}
						}
					}
				}
			}
			else
			{
				IS_Counter[11]=0;
			}

			//缓启完成
			//if(PCS_Soft_Start_OK && MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage)) //无缓启动
			if(PCS_Modify_OK && MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage)) //缓启
			{
				if(ENABLE==System_Info.Config.Power_Distribution_Enable) //使能配电
				{
					IS_Flag=6;
				}
				else
				{
					IS_Counter[17]++;
					if(IS_Counter[17]>1*IS_Time)
					{
						Load_Power_Status=2;
						if(DC_BUS_SYS==1)
						{
							IS_Flag=9;
						}
						else
						{
							IS_Flag=7;
						}
						IS_Counter[17]=0;
					}
				}
			}
			//启动超时判断
			IS_Counter[12]++;
			if(IS_Counter[12]>EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*90*IS_Time)
			{
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max-EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min>1000)
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word|= 1<<6;			//储能变流器交流未连接
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<17;			//微网供电启动超时
				}
				IS_Counter[12]=0;						
			}
			break;
		case 6:			//合微网侧接触器
			(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_HIGH,0); //合微网接触器
			MGrid_Contactor_Control_State = OP_HIGH;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);


			if(!DIN(MGrid_Contacter))
			{
				IS_Counter[13]=0;
			}
			else
			{
				IS_Counter[13]++;
				if(IS_Counter[13]>1*IS_Time)
				{
					Load_Power_Status=2;
					if(DC_BUS_SYS==1)
					{
						IS_Flag=9;
					}
					else
					{
						IS_Flag=7;							
					}
					IS_Counter[13]=0;
				}
			}
			//启动超时判断			
			IS_Counter[14]++;
			if(IS_Counter[14]>10*IS_Time)
			{
				//EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<17;			//微网供电启动超时
				//EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<9;			//微网侧接触器故障
				Sys_Ctrl.Sys_Stop = true;
				MGRID_CONTACTOR_FAULT_SET;
				IS_Counter[14]=0;						
			}
			break;
		case 7:				//PV启动条件判断
			EMS_Info.EMS3x.Island_Start_Info.RCommunication.Status=1<<9;
			if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage)&& 
			   MG_FREQUENCY_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency))
			{
				IS_Counter[15]++;
				if(IS_Counter[15]>2*IS_Time)
				{
					IS_Flag=8;
					IS_Counter[15]=0;
				}
			}
			else
			{
				IS_Flag=9;
			}
			break;
		case 8:				//PV启动
			if(!PV_Start)
			{
				PV_Start=true;
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					if(!PV_Com_Err[i])
					{
						PV_Ctrl_Data[i].PowerLimit_Set=1;
						if(PV_Info[i].PV4x.ParaSet.PowerLimit_Set != PV_Ctrl_Data[i].PowerLimit_Set)
						{
							PV_Ind_Ctrl[i].Active_Power_Limit=true;
							PV_Start=false;
						}
					}
				}
			}
			else
			{
				PV_Start_OK=true;
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					if(!PV_Com_Err[i])
					{
						if(!PV_POWER_ON(PV_Info[i].PV4x.ParaSet.Power_ONOFF))
						{
							PV_Ind_Ctrl[i].Power_On=true;
							PV_Start_OK=false;
						}
					}
				}
			}			
			IS_Counter[16]++;
			if(PV_Start_OK || (IS_Counter[16]>60*IS_Time))
			{
				IS_Flag=9;
				IS_Counter[16]=0;
			}
			break;
		case 9:				//孤岛启动成功
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<3;
			//(void)drv_ioctl(gt_glp.fdido,OP_LED|DO_STATE_HIGH,0);
			Load_Power_Status=2;   // 微网
			Sys_Running_Flag=true;
			Island_Start_Flag=false;
			break;
		default:
			break;
	}
}

//系统自检
void Self_Check(void)
{

	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<0; //自检过程
	static uint16_t SC_Time=3;
	switch(SC_Flag)
	{
		case 0:
			if(ENABLE==System_Info.Config.Power_Distribution_Enable)
			{
				if (ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
				{
					SC_Counter[0]++;
					if(SC_Counter[0]>1*SC_Time)
					{
						SC_Flag=1;
						SC_Counter[0]=0;
					}
				}
				else
				{
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Ind_Ctrl[i].Stop=true;
					}
				}

				SC_Counter[9]++;
				if(SC_Counter[9]>20*SC_Time)
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<16;			   //自检超时
					SC_Counter[9]=0;
				}
			}
			else
			{
				SC_Flag=1;
			}
			break;
		case 1:
            if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_MODE) //并网
            {
            	//if(ENABLE==System_Info.Config.Meter_Comm_Enable)
            	if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
            	{
					drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
					if(Grid_Exist &&(!GRID_CONTACTOR_FAULT)) // 电网有电
					{
						Grid_Start_Flag=true;
						Self_Check_OK=true;
					}
					else
					{
						if(System_Info.Config.EMS_Start_Type==0)
						{
							if(!Fault_Once)
							{
								Fault_Once=true;
								EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;     //自检失败
							}
						}
						else
						{
							EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;         //自检失败
						}
					}
            	}
            	else
            	{
            		bool temp=true;
            		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            		{
            			if(PCS_Ready_Status[i] && GRID_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
            			{
            				temp=false;
            			}
            		}
            		if(temp) //全部故障
            		{
						if(System_Info.Config.EMS_Start_Type==0)
						{
							if(!Fault_Once)
							{
								Fault_Once=true;
								EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;     //自检失败
							}
						}
						else
						{
							EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;         //自检失败
						}
            		}
            		else
            		{
						Grid_Start_Flag=true;
						Self_Check_OK=true;
            		}
            	}
            }
		    else if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == ISLAND_MODE) //离网
		    {
                if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
                {
      				PCS_Ready=true;
    				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    				{
    					if(!PCS_Ready_Status[i]|| MGRID_FAULT)
    					{
    						PCS_Ready=false;    						
    					}        					
                        PCS_Ctrl_Data[i].Grid_Island=1;
                        PCS_Ctrl_Data[i].Island_Mode_Set=1;
                        PCS_Ctrl_Data[i].Island_Mode_ONOFF=0;
                        Send_Data_To_PCS_0x10_02[i]=true;
    				}
				}
				else // 部分正常
				{
      				PCS_Ready=false;
    				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    				{
    					if(PCS_Ready_Status[i]&& (!MGRID_FAULT))
    					{
    						PCS_Ready=true;
    					}    					
                        PCS_Ctrl_Data[i].Grid_Island=1;
                        PCS_Ctrl_Data[i].Island_Mode_Set=1;
                        PCS_Ctrl_Data[i].Island_Mode_ONOFF=0;
                        Send_Data_To_PCS_0x10_02[i]=true;
    				}    				
				}				
				if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
				{
    				if(Bat_Status.SOC_L30) 
    				{
    					PCS_Ready=false;
    				}
				}
				else
				{
    				if(Bat_Status.Vbat_L30) 
    				{
    					PCS_Ready=false;
    				}
				}
				if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
				{
					PCS_Ready=false;
				}
				if(PCS_Ready)
				{
					Island_Start_Flag=true;
					Fault_Once=false;
					Self_Check_OK=true;
					
				}
				else
				{
                    if(System_Info.Config.EMS_Start_Type==0)
                    {
                        if(!Fault_Once)
                        {
                            Fault_Once=true;
                            EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;     //自检失败
                        }
                    }
                    else
                    {
                        EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;         //自检失败                          
                    }
                }
		    }
		    //else if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换 //yuan
            else if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
            {
    			if(Island_First==0xAA) //微网优先
    			{
                    if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
                    {
        				PCS_Ready=true;
        				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        				{
        					if(!PCS_Ready_Status[i] || MGRID_FAULT)
        					{
        						PCS_Ready=false;
        					}        					
                            PCS_Ctrl_Data[i].Grid_Island=1;
                            PCS_Ctrl_Data[i].Island_Mode_Set=1;
                            PCS_Ctrl_Data[i].Island_Mode_ONOFF=0;
                            Send_Data_To_PCS_0x10_02[i]=true;
                        }        				
    				}
    				else
    				{
        				PCS_Ready=false;
        				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        				{
        					if(PCS_Ready_Status[i]&& (!MGRID_FAULT))
        					{
        						PCS_Ready=true;
        					}        					
                            PCS_Ctrl_Data[i].Grid_Island=1;
                            PCS_Ctrl_Data[i].Island_Mode_Set=1;
                            PCS_Ctrl_Data[i].Island_Mode_ONOFF=0;
                            Send_Data_To_PCS_0x10_02[i]=true;
        				}        				
    				}
    				
    				if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
    				{
        				if(Bat_Status.SOC_L30) 
        				{
        					PCS_Ready=false;
        				}
    				}
    				else
    				{
        				if(Bat_Status.Vbat_L30) 
        				{
        					PCS_Ready=false;
        				}
    				}    				
    				if(PCS_Ready)
    				{
    					Island_Start_Flag=true;
    					Fault_Once=false;
    					Self_Check_OK=true;
    				}
    				else
    				{
    					drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    					if(Grid_Exist && (!GRID_CONTACTOR_FAULT))
    					{
    						Grid_Start_Flag=true;
    						Fault_Once=false;
    						Self_Check_OK=true;
    					}
    					else
    					{
    						if(System_Info.Config.EMS_Start_Type==0)
    						{
    							if(!Fault_Once)
    							{
    								Fault_Once=true;
    								EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;	    //自检失败
    							}
    						}
    						else
    						{
    							EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;			//自检失败							
    						}
    					}
    				}
    			}
    			else if(Grid_First==0xAA)//电网优先
    			{
    				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    				if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
    				{
    					Grid_Start_Flag=true;
    					Self_Check_OK=true;
    				}
    				else
    				{   					
                        if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
                        {
                            PCS_Ready=true;
                            for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                            {
                                if(!PCS_Ready_Status[i] || MGRID_FAULT)
                                {
                                    PCS_Ready=false;
                                }                                
                                PCS_Ctrl_Data[i].Grid_Island=1;
                                PCS_Ctrl_Data[i].Island_Mode_Set=1;
                                PCS_Ctrl_Data[i].Island_Mode_ONOFF=0;
                                Send_Data_To_PCS_0x10_02[i]=true;
                            }                            
                        }
                        else
                        {
                            PCS_Ready=false;
                            for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                            {
                                if(PCS_Ready_Status[i]&& (!MGRID_FAULT))
                                {
                                    PCS_Ready=true;
                                }                                
                                PCS_Ctrl_Data[i].Grid_Island=1;
                                PCS_Ctrl_Data[i].Island_Mode_Set=1;
                                PCS_Ctrl_Data[i].Island_Mode_ONOFF=0;
                                Send_Data_To_PCS_0x10_02[i]=true;
                            }                            
                        }                        
    					if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
    					{
        					if(Bat_Status.SOC_L30)					
        					{
        						PCS_Ready=false;
        					}
    					}
    					else
    					{					
        					if(Bat_Status.Vbat_L30)					
        					{
        						PCS_Ready=false;
        					}
    					}    					
    					if(PCS_Ready)
    					{
    						Island_Start_Flag=true;
    						Self_Check_OK=true;
    					}
    					else					
    					{
    						if(System_Info.Config.EMS_Start_Type== ACTIVE_MODE) //0)
    						{
    							if(!Fault_Once)
    							{
    								Fault_Once=true;
    								EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;		//自检失败
    							}
    						}
    						else
    						{
    							EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;			//自检失败							
    						}				
    					}
    				}
    			}
			}
            else
            {
				if(System_Info.Config.EMS_Start_Type== ACTIVE_MODE) //0)
				{
					if(!Fault_Once)
					{
						Fault_Once=true;
						EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;		//自检失败
					}
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<15;			//自检失败
				}
			}
			break;
		default:
			break;
	}
}

//系统停机
void System_Stop()
{
	static bool PV_Stop_OK=false;
	static bool PCS_Stop_OK=false;
	static uint16_t SS_Time=3;
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&=((1<<5)|(1<<8));   //停机不清除故障位和紧急停机位
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status |= 1<<6; //停机中
	switch(SS_Flag)
	{
		case 0:  //初始化
			PV_Stop_OK=false;
			PCS_Stop_OK=false;
			if(GRID_TYPE) //并网型
			{
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
				{
					if((!PCS_Com_Err[i])&&(PCS_GRID_CONNECTED_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
					{
						PCS_Ind_Ctrl[i].Stop=true;
					}
					else
					{
						PCS_Ind_Ctrl[i].Stop=false;
					}
					if(PCS_Ind_Ctrl[i].Stop)
					{
						SS_Counter[3]=0;
					}
				}
				SS_Counter[3]++;
				if(SS_Counter[3]>SS_Time)
				{
					SS_Flag=1;
				}
			}
			else
			{
				SS_Flag=1;
			}
				
			SS_Counter[4]++;
			if(SS_Counter[4]>30*SS_Time)
			{
				SS_Flag=1;
				SS_Counter[4]=0;
			}
			break;
		case 1:  //PV停机
			if(DC_BUS_SYS!=1)
			{
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					PV_Ctrl_Data[i].PowerLimit_Set=0;
				}
				
				PV_Stop_OK=true;
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					if(!PV_Com_Err[i])
					{
						if(PV_Info[i].PV4x.ParaSet.PowerLimit_Set!=PV_Ctrl_Data[i].PowerLimit_Set)
						{
							PV_Stop_OK=false;
							PV_Ind_Ctrl[i].Active_Power_Limit=true;
						}
					}
				}
			}
			else
			{
				SS_Flag=2;
			}
			
			SS_Counter[0]++;
			if(PV_Stop_OK || SS_Counter[0]>60*SS_Time)
			{
				PV_Stop_OK=false;
				SS_Flag=2;
				SS_Counter[0]=0;
			}
			break;
		case 2:  //开关柜接触器断开处理
			if(ENABLE==System_Info.Config.Power_Distribution_Enable)
			{
				(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_LOW,0);	//断电网接触器
				(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);	//断微网接触器
				//(void)drv_ioctl(gt_glp.fdido,OP_LED|DO_STATE_LOW,0);
				Grid_Contactor_Control_State = OP_LOW;
				MGrid_Contactor_Control_State = OP_LOW;

				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

				if((!DIN(Grid_Contacter))&&(!DIN(MGrid_Contacter)))
				{
					SS_Flag=3;
				}

				SS_Counter[1]++;
				if(SS_Counter[1]>5*SS_Time)
				{
					if(DIN(Grid_Contacter))
					{
						GRID_CONTACTOR_FAULT_SET;
					}
					if(DIN(MGrid_Contacter))
					{
						MGRID_CONTACTOR_FAULT_SET;
					}
					SS_Counter[1]=0;
					SS_Flag=3;
				}
			}
			else
			{
				SS_Flag=3;
			}
			break;

		case 3:  //PCS停机
			PCS_Stop_OK=true;
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				PCS_Ind_Ctrl[i].Stop=true;
			}
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				if(!PCS_Com_Err[i])
				{
					if(PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
					{
						PCS_Stop_OK=false;	
					}
				}
			}
					
			if(PCS_Stop_OK)
			{
				PCS_Stop_OK=false;
				SS_Flag=4;
			}
			
			SS_Counter[2]++;
			if(SS_Counter[2]>60*SS_Time)
			{
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<23;			//置停机超时状态
				SS_Flag=4;
				SS_Counter[2]=0;
			}
			break;

		case 4:  //复位处理
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status &= ~(1<<6);	//清停机中状态
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status |= 1<<7;		//置停机状态
			Self_Check_OK=false;
			Sys_Running_Flag=false;
			Load_Power_Status=0;
			SS_Counter[3]++;
			if(SS_Counter[3]>EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*SS_Time)
			{
				Reset_Require=true;
				SS_Counter[3]=0;
			}
			break;
		default:
			break;
	}
}

//复位
void System_Reset()
{
	Island_Start_Flag=false;
	Grid_Start_Flag=false;
	Sys_Running_Flag=false;
	Reset_Require=false;
	Sys_Stop_Flag=false;
	
	EMG_Counter=0;
	Load_Power_Status=0;
	Grid_Pic_Direction=0;
	PCS_Pic_Direction=0;
	PV_Pic_Direction=0;
	Load_Pic_Direction=0;
	
	//Island_Start
	IS_Flag=0;
	GC_Flag=0;
	SC_Flag=0;
	SS_Flag=0;
	for(uint8_t i=0;i<PCS_NUM;i++)
	{
		IS_Counter[i]=0;
		GC_Counter[i]=0;
		SC_Counter[i]=0;
		SS_Counter[i]=0;
		FC_Counter[i]=0;
		//SOC_Counter[i]=0;
		RC_Counter[i]=0;
		AGC_Counter[i]=0;
		AC_Counter[i]=0;
		StG_Counter[i]=0;
		StI_Counter[i]=0;
	}
	
	Self_Check_OK=false;
	PCS_Ready=false;
	Switch_to_Grid_Status=false;
	Switch_to_Island_Status=false;
	Switch_to_Grid_Flag=0;
	Switch_to_Island_Flag=0;
	
	//EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<0);
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<4);   //复位光伏逆变器故障
	//EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<9);
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<15);  //复位自检失败
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<16);  //复位自检超时
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<17);  //复位微网供电启动超时
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<18);  //复位微网供电启动失败
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<19);  //复位电网供电启动超时
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<20);  //复位电网供电启动失败
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1&= ~((uint32_t)1<<23);	 //复位停机超时

    //PID参数停机清零
    for(uint8_t i=0;i<PCS_NUM;i++)
    {
        memset(&sPid,0,sizeof(sPid[i]));
    }	

    //削锋填谷清零
    Peak_Valley_Power = 0;

    //平滑停机清零
    step = 1;
    pcs_charge_delay = 0;
    pcs_power_fault_delay = 0;
    ppk2 = 0 ;

}

//告警判断
void Alarm_Check(void)
{
	bool temp = false;
//-------------------------------------- ----------------------------------------------------------	
	//储能变流器故障告警
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		if(!PCS_NO_FATAL_FAULT(i))
		{
			temp=true;
			break;
		}
	}

	if(temp)
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<0);
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<0);	
	}

	//储能变流器通讯故障告警
	temp = false;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		if(PCS_Com_Err[i])
		{
			temp=true;
			break;
		}
	}

	if(temp)
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<1);	
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<1);	
	}

//------------------------------------------------------------------------------------------------
    if( System_Info.Config.BMS_Type == BMS_68AH)
    {
    	//BMS通讯故障告警
    	temp = false;
    	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
    	{
    		if(BMS_Com_Err[i])
    		{
    			temp=true;
    			break;
    		}
    	}
    	if(temp)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<9);	
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<9);	
    	}

    	//BMS故障告警
        temp = false;	
    	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
    	{
            if(BMS_Info[i].Rack_Fault_Word != 0)
    		{                          
    			temp=true;
    			break;
    		}
    	}
    	if(temp)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<10);	
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<10);
    	}

        //BMS告警
    	temp = false;	
    	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
    	{
            if((BMS_Info[i].Rack_Alarm_Word!= 0)||(BMS_Info[i].Sys_Alarm_Word != 0))
    		{                          
    			temp=true;
    			break;
    		}
    	}
    	if(temp)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<11);	
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<11);
    	}
    }
    else if( System_Info.Config.BMS_Type == BMS_94AH)
    {
    	//BMS通讯故障告警
    	temp = false;
    	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
    	{
    		if(BMS_Com_Err[i])
    		{
    			temp=true;
    			break;
    		}
    	}
    	if(temp)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<9);	
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<9);	
    	}

    	//BMS故障告警
        temp = false;	
    	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
    	{
            if((BMS_Info[i].Protection1!=0)||(BMS_Info[i].Protection2!=0)||(BMS_Info[i].Protection3!=0)||(BMS_Info[i].Protection4!=0))
    		{                          
    			temp=true;
    			break;
    		}
    	}
    	if(temp)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<10);	
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<10);
    	}

        //BMS告警
    	temp = false;	
    	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
    	{
            if((BMS_Info[i].Alarm1!=0)||(BMS_Info[i].Alarm2!=0)||(BMS_Info[i].Alarm3!=0)||(BMS_Info[i].Alarm4!=0)||(BMS_Info[i].Sys_Alarm_Status!=0))
    		{                          
    			temp=true;
    			break;
    		}
    	}
    	if(temp)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<11);	
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<11);
    	}
    }    
    else
    {
        EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<9);  
        EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<10);
        EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<11);
    }
//------------------------------------------------------------------------------------------------
	if(DC_BUS_SYS == 0)  // 交流母线
	{
        //光伏逆变器故障告警
		temp = false;
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
		{
			if(PV_FAULT(i))
			{
				temp=true;
				break;
			}
		}
		if(temp)
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<2);
		}
		else
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<2);	
		}
		
		//光伏逆变器通讯故障告警
		temp = false;
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
		{
			if(PV_Com_Err[i])
			{
				temp=true;
				break;
			}
		}

		if(temp)
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<3);	
		}
		else
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<3);	
		}
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<2);	
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<3);	
	}

//------------------------------------------------------------------------------------------------
    if(DC_BUS_SYS == 1) //直流母线
    {
        if(System_Info.Config.DCDC_Type!=0) //使能DCDC通讯
    	{
            if(System_Info.Config.DCDC_Type==1)//单向DCDC
            {
                //DCDC故障告警
        		temp = false;
        		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
        		{
        			if((DCDC_Info[i].Module_1.Module_Dev_Fault_Status!=0)||(DCDC_Info[i].Module_2.Module_Dev_Fault_Status!=0)||
        			   (DCDC_Info[i].Module_3.Module_Dev_Fault_Status!=0)||(DCDC_Info[i].Module_4.Module_Dev_Fault_Status!=0))
        			{
        				temp=true;
        				break;
        			}
        		}
        		if(temp)
        		{
        			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<12);
        		}
        		else
        		{
        			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<12);	
        		}
    		}		
            else if(System_Info.Config.DCDC_Type==2) //双向DCDC
            {
                //DCDC故障告警
        		temp = false;
        		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
        		{
        			if((DCDC_Info[i].Module_1.Fault_Status!=0)||(DCDC_Info[i].Module_2.Fault_Status!=0)||
        			   (DCDC_Info[i].Module_3.Fault_Status!=0)||(DCDC_Info[i].Module_4.Fault_Status!=0))
        			{
        				temp=true;
        				break;
        			}
        		}
        		if(temp)
        		{
        			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<12);
        		}
        		else
        		{
        			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<12);	
        		}
            }
    		
    		//DCDC通讯故障告警
    		temp = false;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
    		{
    			if(DCDC_Com_Err[i])
    			{
    				temp=true;
    				break;
    			}
    		}

    		if(temp)
    		{
    			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<13);	
    		}
    		else
    		{
    			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<13);	
    		}
    	}	
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<12);	
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<13);	
    	}
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<12);	
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<13);	
	}

    //电表通信告警
    //if(System_Info.Config.Meter_Comm_Enable == ENABLE)
    if(1)
    {
    	//meter通讯故障告警
    	temp = false;
    	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;i++)
    	{
    		if((System_Info.Config.Meter_Position_Enable[i] == ENABLE) && Meter_Com_Err[i])
    		{
    			temp=true;
    			break;
    		}
    	}

    	if(temp)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<15);
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<15);
    	}
    }
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<15);
	}

	//PVS通信告警
    if(webPageShow.pvs == 1)
    {
		temp = false;
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number;i++)
		{
			if(PVS_Com_Err[i])
			{
				temp=true;
				break;
			}
		}

		if(temp)
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<16);
		}
		else
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<16);
		}
    }
    else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<16);
	}

	//环境监测仪通信告警
	if(webPageShow.env == 1)
	{
		temp = false;
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_ENV_Number;i++)
		{
			if(ENV_Com_Err[i])
			{
				temp=true;
				break;
			}
		}

		if(temp)
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<17);
		}
		else
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<17);
		}
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<17);
	}

    // DSP通讯告警    
    /*if(DSP_Com_Err[0])
    {
        EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<14);
    }
    else
    {
        EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<14);  
    }*/
	
//------------------------------------------------------------------------------------------------
	//电池电量低告警
	if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
	{
    	if(Bat_Status.SOC_L40)   // 电池电量低一级告警
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<7);
            //Out1 ctrl
    		if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_LOW,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_HIGH,0);                         
    		}
            //Out2 ctrl
    		/*if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_LOW,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_HIGH,0);                         
    		}*/
    	}		
    	else //告警恢复
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<7);	
            //Out1 ctrl
    		if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_HIGH,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_LOW,0);                         
    		}
            //Out1 ctrl
    		/*if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_HIGH,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_LOW,0);                         
    		} */
    	}
	}
	else
	{
    	if(Bat_Status.Vbat_L40)   // 电池电量低一级告警
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<7);    		
            //Out1 ctrl
    		if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_LOW,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_HIGH,0);                         
    		}
            //Out2 ctrl
    		/*if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_LOW,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_HIGH,0);                         
    		}*/
    	}		
    	else //告警恢复
    	{ 
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<7);	    		
            //Out1 ctrl
    		if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_HIGH,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT1|DO_STATE_LOW,0);                         
    		}
            //Out1 ctrl
    		/*if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_OPEN)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_HIGH,0);                         
    		}
    		else if (EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode == NORMAL_CLOSE)
    		{
                (void)drv_ioctl(gt_glp.fdido,LB_OUT2|DO_STATE_LOW,0);                         
    		}*/
    	}
	}
	
	//电池充电满提醒
	if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
	{
    	if(!Bat_Status.SOC_L80)    // 电池电量高一级告警
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<8);
    	}		
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<8);	
    	}
	}
	else
	{
    	if(!Bat_Status.Vbat_L80)    // 电池电量高一级告警
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<8);
    	}		
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<8);	
    	}
	}

//------------------------------------------------------------------------------------------------
	//接触器实时对比查看是否故障
	if ((!Switch_to_Grid_Status) && (!Switch_to_Island_Status) && (Sys_Running_Flag) && (!Sys_Stop_Flag))
	{
		if(ENABLE==System_Info.Config.Power_Distribution_Enable)
		{
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

			if (Grid_Exist) //电网有电
			{
				if(Grid_Contactor_Control_State == OP_HIGH)
				{
					if(!DIN(Grid_Contacter))
					{
						AC_Counter[0]++;
						if(AC_Counter[0]>18*3) //16.2s判断掉电
						{
							AC_Counter[0] = 0;
							GRID_CONTACTOR_FAULT_SET;
							if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)
							{
								Switch_to_Island_Status=true;
								Switch_to_Island_Flag=0;
								Switch_to_Grid_Status=false;
								Switch_to_Grid_Flag=0;
							}
							if(GRID_RUNNING)
							{
								Sys_Ctrl.Sys_Stop = true;
							}
						}
					}
					else
					{
						AC_Counter[0] = 0;
					}
				}
				else
				{
					if(DIN(Grid_Contacter))
					{
						AC_Counter[1]++;
						if(AC_Counter[1]>2*3)
						{
							AC_Counter[1] = 0;
							GRID_CONTACTOR_FAULT_SET;
							if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)
							{
								Switch_to_Island_Status=true;
								Switch_to_Island_Flag=0;
								Switch_to_Grid_Status=false;
								Switch_to_Grid_Flag=0;
							}
							//if(ISLAND_RUNNING)
							//{
							//	Sys_Ctrl.Sys_Stop = true;
							//}
						}
					}
					else
					{
						AC_Counter[1] = 0;
					}
				}
			}

			if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
			{
				if(MGrid_Contactor_Control_State == OP_HIGH)
				{
					if(!DIN(MGrid_Contacter))
					{
						AC_Counter[2]++;
						if(AC_Counter[2]>6*EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
						{
							AC_Counter[2] = 0;
							MGRID_CONTACTOR_FAULT_SET;
							if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)
							{
								Switch_to_Grid_Status=true;
								Switch_to_Grid_Flag=0;
								Switch_to_Island_Status=false;
								Switch_to_Island_Flag=0;
							}
							//if(ISLAND_RUNNING)
							//{
							//	Sys_Ctrl.Sys_Stop = true;
							//}
						}
					}
					else
					{
						AC_Counter[2] = 0;
					}
				}
				else
				{
					if(DIN(MGrid_Contacter))
					{
						AC_Counter[3]++;
						if(AC_Counter[3]>2*3)
						{
							AC_Counter[3] = 0;
							MGRID_CONTACTOR_FAULT_SET;
							if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)
							{
								Switch_to_Grid_Status=true;
								Switch_to_Grid_Flag=0;
								Switch_to_Island_Status=false;
								Switch_to_Island_Flag=0;
							}
							//Sys_Ctrl.Sys_Stop = true;
						}
					}
					else
					{
						AC_Counter[3] = 0;
					}
				}
			}
		}
		if (EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max - EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min > 1000)
		{
			AC_Counter[4]++;
			AC_Counter[5]=0;
			if(AC_Counter[4]>6*EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
			{
				AC_Counter[4]=0;
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word|= 1<<6;			//储能变流器交流未连接
			}
		}
		else if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
		{
			if(GRID_TYPE==1)
			{
				AC_Counter[5]++;
				AC_Counter[4]=0;
				if(AC_Counter[5]>3)
				{
					AC_Counter[5]=0;
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~(1<<6);	//储能变流器交流未连接恢复	
				}
			}
		}
	}
}

//故障判断
void Fault_Check()
{
	static bool temp;
	//static uint16_t FC_Time=3;

	if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
	{
    	//PCS故障
    	if(ISLAND_RUNNING)
    	{
    		temp=true;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(PCS_NO_FATAL_FAULT(i))
    			{
    				temp=false;
    				break;
    			}
    		}
    	}
    	else
    	{
    		temp=false;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(!PCS_NO_FATAL_FAULT(i))
    			{
    				temp=true;
    				break;
    			}
    		}
    	}
    	if(temp==true)
    	{
    		//if(ENABLE==System_Info.Config.Meter_Comm_Enable)
    		if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
    		{
				if(Grid_Missed)
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<0;			//PCS故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<0);	   //PCS故障恢复
				}
    		}
    		else
    		{
    			if(ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<0;			//PCS故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<0);	   //PCS故障恢复
				}
    		}
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<0);			//PCS故障恢复	
    	}
    	
    	//PCS通讯故障
    	if(ISLAND_RUNNING)
    	{
    		temp=true;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(!PCS_Com_Err[i])
    			{
    				temp=false;
    			}
    		}
    	}
    	else
    	{
    		temp=false;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(PCS_Com_Err[i])
    			{
    				temp=true;
    			}
    		}
    	}
    	if(temp==true)
    	{
    		//if(ENABLE==System_Info.Config.Meter_Comm_Enable)
    		if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
    		{
				if(Grid_Missed)
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<1;			//PCS通讯故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<1);			//PCS通讯故障恢复
				}
    		}
    		else
    		{
    			if(ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<1;			//PCS通讯故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<1);			//PCS通讯故障恢复
				}
    		}
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<1);			//PCS通讯故障恢复	
    	}
	}
	else // 部分正常
	{
    	//PCS故障
    	if(ISLAND_RUNNING)
    	{
    		temp=true;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(PCS_NO_FATAL_FAULT(i))
    			{
    				temp=false;
    				break;
    			}
    		}
    	}
    	else
    	{
    		temp=true;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(PCS_NO_FATAL_FAULT(i))
    			{
    				temp=false;
    				break;
    			}
    		}
    	}
    	if(temp==true)
    	{
    		//if(ENABLE==System_Info.Config.Meter_Comm_Enable)
    		if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
    		{
				if(Grid_Missed)
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<0;			//PCS故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<0);			//PCS故障恢复
				}
    		}
    		else
    		{
    			if(ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<0;			//PCS故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<0);			//PCS故障恢复
				}
			}

    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<0);			//PCS故障恢复	
    	}
    	
    	//PCS通讯故障
    	if(ISLAND_RUNNING)
    	{
    		temp=true;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(!PCS_Com_Err[i])
    			{
    				temp=false;
    			}
    		}
    	}
    	else
    	{
    		temp=true;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(!PCS_Com_Err[i])
    			{
    				temp=false;
    			}
    		}
    	}
    	if(temp==true)
    	{
    		//if(ENABLE==System_Info.Config.Meter_Comm_Enable)
    		if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
    		{
				if(Grid_Missed)
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<1;			//PCS通讯故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<1);			//PCS通讯故障恢复
				}
    		}
    		else
    		{
    			if(ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<1;			//PCS通讯故障
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<1);			//PCS通讯故障恢复
				}
			}
    	}
    	else
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<1);			//PCS通讯故障恢复	
    	}
	}
	
	//储能变流器地址规则判断
	EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= (uint32_t)(~(1<<2));	//PCS地址冲突
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		for(uint8_t j=i+1;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;j++)
		{
			if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address ==
			   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[j].Modubus_Address)
			{
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<2;			//PCS地址冲突
				break;
			}
		}
	}

	//光伏逆变器地址规则判断
	if(!DC_BUS_SYS)
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= (uint32_t)(~(1<<6));	//PV地址冲突
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
		{
			for(uint8_t j=i+1;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;j++)
			{
				if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address ==
				   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[j].Modubus_Address)
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<6;			//PV地址冲突
					break;
				}
			}
		}
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= (uint32_t)(~(1<<6));	//PV地址冲突
	}
	
	//光伏通讯故障判断
	/*if(!DC_BUS_SYS)
	{
		temp=true;
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
		{
			if(!PV_Com_Err[i])
			{
				temp=false;
			}
		}

		if(temp==true)
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<5;			//PV通讯故障
		}
		else
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<5);			//PV通讯故障恢复
		}
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 &= ~(1<<5);			//PV通讯故障恢复
	}*/

	//接触器故障
	if(ALL_CONTACTOR_FAULT)
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<8;	//电网和微网接触器均故障
	}

    //PCS状态检查
    for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    {
        if(PCS_NO_FATAL_FAULT(i)&&(!PCS_Com_Err[i])&&(!PCS_E_STOP(i))&&(PCS_Com_OK[i]))
        {
            PCS_Ready_Status[i]=true;
        }
        else
        {
            PCS_Ready_Status[i]=false;
        }
    }


    //输入状态监测
    for(uint8_t i=0;i<8;i++)
    {
    	//static uint16_t pin_counts[8];
    	if(System_Info.Config.InputPin_Function[i]==ALARM_PIN)
    	{
    		if(System_Info.Config.InputPin_NC_NO[i]==NORMAL_OPEN)
    		{
				if(DIN(i+5))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<(i+20));
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<(i+20));
				}
    		}
    		else if(System_Info.Config.InputPin_NC_NO[i]==NORMAL_CLOSE)
			{
				if(!DIN(i+5))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= ((uint32_t)1<<(i+20));
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<(i+20));
				}
			}
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2 &= ~((uint32_t)1<<(i+0));
    	}
    	else if(System_Info.Config.InputPin_Function[i]==FAULT_PIN)
    	{
    		if(System_Info.Config.InputPin_NC_NO[i]==NORMAL_OPEN)
    		{
				if(DIN(i+5))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2 |= ((uint32_t)1<<(i+0));
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2 &= ~((uint32_t)1<<(i+0));
				}
    		}
    		else if(System_Info.Config.InputPin_NC_NO[i]==NORMAL_CLOSE)
    		{
				if(!DIN(i+5))
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2 |= ((uint32_t)1<<(i+0));
				}
				else
				{
					EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2 &= ~((uint32_t)1<<(i+0));
				}
			}
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<(i+20));
    	}
    	else if (System_Info.Config.InputPin_Function[i]==NODEF_PIN)
    	{
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word  &= ~((uint32_t)1<<(i+20));
    		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2 &= ~((uint32_t)1<<(i+0));
    	}
        EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.InputPin_Status
        =((DIN(5)<<0)|(DIN(6)<<1)|(DIN(7)<<2)|(DIN(8)<<3)|(DIN(9)<<4)|(DIN(10)<<5)|(DIN(11)<<6)|(DIN(12)<<7)); //输入节点扫描

    }

    	
	if(SYS_NO_FAULT)
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status &= ~(1<<5);
		//(void)drv_ioctl(gt_glp.fdido,FA_LED|DO_STATE_LOW,0);
		//Fault_Light_Control_State = OP_LOW;
		drv_write_light_gpio(GPIO_LIGHT_FAULT, UTILS_IO_LOW);
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status &= 1<<7;
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status |= 1<<5;
		//(void)drv_ioctl(gt_glp.fdido,FA_LED|DO_STATE_HIGH,0);
		//Fault_Light_Control_State = OP_HIGH;
		drv_write_light_gpio(GPIO_LIGHT_FAULT, UTILS_IO_HIGH);
		Sys_Ctrl.Flt_Stop=true;
	}

	//Alarm check
	//PCS通讯告警
	for(uint8_t i=0;i<PCS_NUM;i++)
	{
		if(PCS_Com_Err[i])
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PCS_Comm_Fault|=(uint32_t)(1<<i);
		}
		else
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PCS_Comm_Fault&=(uint32_t)(~(1<<i));
		}
	}

	//PV通讯告警
	if(!DC_BUS_SYS)
	{
		for(uint8_t i=0;i<PV_NUM;i++)
		{
			if(PV_Com_Err[i])
			{
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PV_Comm_Fault|=(uint32_t)(1<<i);
			}
			else
			{
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PV_Comm_Fault&=(uint32_t)(~(1<<i));
			}
		}
	}
	else
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.PV_Comm_Fault&=(uint32_t)0;
	}
}

//系统逻辑
void System_Logic()
{
	//PV总控制-----------------------------------------------------------------
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
	{
		if(Sys_Ctrl.Power_On)
		{
			PV_Ind_Ctrl[i].Power_On=true;
		}
		
		if(Sys_Ctrl.Power_Off)
		{
			PV_Ind_Ctrl[i].Power_Off=true;
		}
		
		if(Sys_Ctrl.Active_Power_Limit)
		{
			PV_Ind_Ctrl[i].Active_Power_Limit=true;
		}		
		
		if(Sys_Ctrl.Reactive_Power_Limit)
		{
			PV_Ind_Ctrl[i].Reactive_Power_Limit=true;
		}
	}
	
	//PCS总控制------------------------------------------------------------------
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		if(Sys_Ctrl.Stop)
		{
			PCS_Ind_Ctrl[i].Stop=true;
			Sys_Ctrl.Stop=false;
		}
		
		if(Sys_Ctrl.Standby)
		{
			PCS_Ind_Ctrl[i].Standby=true;
			Sys_Ctrl.Standby=false;
		}
		
		if(Sys_Ctrl.Invert)
		{
			PCS_Ind_Ctrl[i].Invert=true;
			Sys_Ctrl.Invert=false;
		}
		
		if(Sys_Ctrl.Reactive_Power_Set)
		{
			PCS_Ind_Ctrl[i].Reactive_Power_Set=true;
			Sys_Ctrl.Reactive_Power_Set=false;
		}
		
		if(Sys_Ctrl.Grid_Const_I_Mode)
		{
			PCS_Ind_Ctrl[i].Grid_Const_I_Mode=true;
			Sys_Ctrl.Grid_Const_I_Mode=false;
		}
		
		if(Sys_Ctrl.Grid_Const_V_Mode)
		{
			PCS_Ind_Ctrl[i].Grid_Const_V_Mode=true;
			Sys_Ctrl.Grid_Const_V_Mode=false;
		}
		
		if(Sys_Ctrl.Grid_Const_P_DC_Mode)
		{
			PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode=true;
			Sys_Ctrl.Grid_Const_P_DC_Mode=false;
		}
		
		if(Sys_Ctrl.Grid_Const_P_DC_Mode)
		{
			PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode=true;
			Sys_Ctrl.Grid_Const_P_DC_Mode=false;
		}
		
		if(Sys_Ctrl.Parameter_Modify)
		{
			PCS_Ind_Ctrl[i].Parameter_Modify=true;
			Sys_Ctrl.Parameter_Modify=false;
		}
	}
	
	//启动指令-----------------------------------------------
	if(Sys_Ctrl.Sys_Start)
	{
		Sys_Start_Flag=true;
		Sys_Ctrl.Sys_Start=false;
	}
	
	//停机指令
	//系统停机-----------------------------------------------
	if(Sys_Ctrl.Sys_Stop)
	{
		Sys_Stop_Flag=true;
		Fault_Once=false;
		Sys_Ctrl.Sys_Stop=false;
	}
	//故障停机-----------------------------------------------
	if(Sys_Ctrl.Flt_Stop)
	{
		Sys_Stop_Flag=true;
		if(System_Info.Config.EMS_Start_Type == REACTIVE_MODE) //1)
		{
			Sys_Start_Flag=false;
		}
		Sys_Ctrl.Flt_Stop=false;
	}
	//系统手动停机-----------------------------------------------
	if(Sys_Ctrl.Manual_Stop)
	{
		Sys_Stop_Flag=true;
		Sys_Start_Flag=false;
		Fault_Once=false;
		Sys_Ctrl.Manual_Stop=false;
	}
	
	//系统启动优先级判断------------------------------------------
	if(System_Info.Config.EMS_Run_Priority == 1)
	{
		Grid_First = DISABLE;
		Island_First = ENABLE;
	}
	else
	{
		Grid_First = ENABLE;
		Island_First = DISABLE;
	}
}

//系统通讯处理
void Com_Process()
{
	//DCDC数据下发--------------------------------------------------------------------------
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
	{
		if(!DCDC_Com_Err[i])
		{
			//DCDC开机
			if(DCDC_Ind_Ctrl[i].Power_On)
			{
				DCDC_Ctrl_Data[i].Power_ONOFF=0xCF; // CF 开机 CE 关机
				Send_Data_To_DCDC_0x10_02[i]=true;
			}
			//DCDC关机
			if(DCDC_Ind_Ctrl[i].Power_Off)
			{
				DCDC_Ctrl_Data[i].Power_ONOFF=0xCE;  // CF 开机 CE 关机
				Send_Data_To_DCDC_0x10_02[i]=true;
			}
		}
	}

	//PV数据下发--------------------------------------------------------------------------
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
	{
		if(!PV_Com_Err[i])
		{
			//PV开机
			if(PV_Ind_Ctrl[i].Power_On)
			{
				PV_Ind_Ctrl[i].Active_Power_Limit=PV_Info[i].PV4x.ParaSet.PowerLimit_Set;
				PV_Ctrl_Data[i].Power_ONOFF=0xCF; // CF 开机 CE 关机
				Send_Data_To_PV_0x10_02[i]=true;
				PV_Ind_Ctrl[i].Power_On=false;
			}
			//PV关机
			if(PV_Ind_Ctrl[i].Power_Off)
			{
				PV_Ind_Ctrl[i].Active_Power_Limit=PV_Info[i].PV4x.ParaSet.PowerLimit_Set;
				PV_Ctrl_Data[i].Power_ONOFF=0xCE;  // CF 开机 CE 关机
				Send_Data_To_PV_0x10_02[i]=true;
				PV_Ind_Ctrl[i].Power_Off=false;
			}
			//PV有功限功率
			if(PV_Ind_Ctrl[i].Active_Power_Limit)
			{
				PV_Ctrl_Data[i].Power_ONOFF=0xCF;
				PV_Ctrl_Data[i].PowerLimit_Switch=0xAA;
				Send_Data_To_PV_0x10_02[i]=true;
				Send_Data_To_PV_Broadcast=true;
				PV_Ind_Ctrl[i].Active_Power_Limit=false;
			}
		}
	}
	
	//PCS数据下发-------------------------------------------------------------------------
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		if(!PCS_Com_Err[i])
		{
			//PCS停机
			if(PCS_Ind_Ctrl[i].Stop)
			{
				PCS_Ctrl_Data[i].Grid_Mode_Set=5;
				PCS_Ctrl_Data[i].Island_Mode_ONOFF=0;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Stop=false;
			}
			//PCS待机
			if(PCS_Ind_Ctrl[i].Standby)
			{
				PCS_Ctrl_Data[i].Grid_Island=0;
				PCS_Ctrl_Data[i].Grid_Mode_Set=4;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Standby=false;
			}
			//PCS独立逆变
			if(PCS_Ind_Ctrl[i].Invert)
			{
				PCS_Ctrl_Data[i].Grid_Island=1;
				PCS_Ctrl_Data[i].Island_Mode_Set=1;
				PCS_Ctrl_Data[i].Island_Mode_ONOFF=1;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Invert=false;
			}
			//PCS恒流模式
			if(PCS_Ind_Ctrl[i].Grid_Const_I_Mode)
			{
				PCS_Ctrl_Data[i].Grid_Island=0;
				PCS_Ctrl_Data[i].Grid_Mode_Set=0;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Grid_Const_I_Mode=false;
			}
			//PCS恒压模式
			if(PCS_Ind_Ctrl[i].Grid_Const_V_Mode)
			{
				PCS_Ctrl_Data[i].Grid_Island=0;
				PCS_Ctrl_Data[i].Grid_Mode_Set=1;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Grid_Const_V_Mode=false;
			}
			//PCS恒直流功率充电
			if(PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode)
			{
				PCS_Ctrl_Data[i].Grid_Island=0;
				PCS_Ctrl_Data[i].Grid_Mode_Set=2;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode=false;
			}
			//PCS恒功率放电
			if(PCS_Ind_Ctrl[i].Grid_Const_P_AC_Mode)
			{
				PCS_Ctrl_Data[i].Grid_Island=0;
				PCS_Ctrl_Data[i].Grid_Mode_Set=3;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Grid_Const_P_AC_Mode=false;
			}			
			//PCS电压修改
			if(PCS_Ind_Ctrl[i].Parameter_Modify)
			{
				PCS_Ctrl_Data[i].Grid_Island=PCS_Info[i].PCS4x.RControl.Grid_Island;
				Send_Data_To_PCS_0x10_02[i]=true;
				PCS_Ind_Ctrl[i].Parameter_Modify=false;
			}
		}
	}
}

//紧急启停机判断
void EMG_Stop_Proc(void)
{
	//紧急启停机判断------------------------------------------
	drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);

	if (DIN(Input2))
	{
		EMG_Counter=0;
		EMG_Stop=false;
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status &= ~(1<<8);
	}
	else
	{
		if(EMG_Counter>4)
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status|=1<<8;
			EMG_Stop=true;
			Sys_Stop_Flag=true;
			Sys_Start_Flag=false;
			Fault_Once=false;
			if(ENABLE == System_Info.Config.Power_Distribution_Enable)
			{
				//急停直接断接触器
				(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_LOW,0);	//断电网接触器
				(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);	//断微网接触器
				//(void)drv_ioctl(gt_glp.fdido,OP_LED|DO_STATE_LOW,0);
				Grid_Contactor_Control_State = OP_LOW;
				MGrid_Contactor_Control_State = OP_LOW;
				//Run_Light_Control_State = OP_LOW;
			}
		}
		else
		{
			EMG_Counter++;	
		}
	}
}

//运行控制
void Running_Control(void)
{
	uint16_t RC_Time=3;	
    bool temp1,temp2,temp3,temp4,temp5,temp6,temp7;
	if(ISLAND_STARTING) //微网供电启动
	{
        if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
        {   
            temp1=false;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(!PCS_Ready_Status[i]||MGRID_FAULT)
    			{
    				temp1=true;  
    			}
    		}
        }
        else // 部分正常
        {
            temp1=true;
    		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    		{
    			if(PCS_Ready_Status[i] && (!MGRID_FAULT))
    			{
    				temp1=false;  
    			}
    		}
		}
		if(temp1)
		{
			Sys_Ctrl.Sys_Stop=true;
		}
		//微网启动情况下，电池电压低的处理
        if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
        {
    		if(Bat_Status.SOC_L30)
    		{
    			RC_Counter[0]++;
    			if(RC_Counter[0]>2*RC_Time)
    			{
    				Sys_Ctrl.Sys_Stop=true;
    				RC_Counter[0]=0;
    			}
    		}
    		else
    		{
    			RC_Counter[0]=0;
    		}
        }
        else
        {
    		if(Bat_Status.Vbat_L30)
    		{
    			RC_Counter[0]++;
    			if(RC_Counter[0]>2*RC_Time)
    			{
    				Sys_Ctrl.Sys_Stop=true;
    				RC_Counter[0]=0;
    			}
    		}
    		else
    		{
    			RC_Counter[0]=0;
    		}
        }
        if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_MODE)
        {
            Sys_Ctrl.Sys_Stop=true;
        }
        //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
        {
    		if(Grid_First==0xAA) //电网优先
    		{
    			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    			if(Grid_Exist && (!GRID_CONTACTOR_FAULT))
    			{
    				RC_Counter[1]++;
    				if(RC_Counter[1]>2*RC_Time)
    				{
    					Sys_Ctrl.Sys_Stop=true;
    					RC_Counter[1]=0;
    				}
    			}
    			else
    			{
    				RC_Counter[1]=0;
    			}
    		}	
		}
	}
	else if(GRID_STARTING)
	{
        if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == ISLAND_MODE)
        {
            Sys_Ctrl.Sys_Stop=true;
        }
        //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
        {
    		if(Island_First==0xAA) //微网优先
    		{
                if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
                {
        			if(!Bat_Status.SOC_L30)
        			{
        				RC_Counter[2]++;
        				if(RC_Counter[2]>2*RC_Time)
        				{
                            if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
                            {
            					temp2=true;
            					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            					{
            						if(!PCS_Ready_Status[i]|| MGRID_FAULT)
            						{
            							temp2=false;
            						}
            					}
        					}
        					else //部分正常
        					{
            					temp2=false;
            					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            					{
            						if(PCS_Ready_Status[i]&&(!MGRID_FAULT))
            						{
            							temp2=true;
            						}
            					}
        					}        					
        					if(temp2)
        					{
        						Sys_Ctrl.Sys_Stop=true;
        					}
        					RC_Counter[2]=0;
        				}
        			}
        			else
        			{
        				RC_Counter[2]=0;
        			}
    			}
                else
                {
        			if(!Bat_Status.Vbat_L30)
        			{
        				RC_Counter[2]++;
        				if(RC_Counter[2]>2*RC_Time)
        				{       					
                            if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
                            {
            					temp2=true;
            					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            					{
            						if(!PCS_Ready_Status[i]|| MGRID_FAULT)
            						{
            							temp2=false;
            						}
            					}
        					}
        					else //部分正常
        					{
            					temp2=false;
            					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            					{
            						if(PCS_Ready_Status[i]&&(!MGRID_FAULT))
            						{
            							temp2=true;
            						}
            					}
        					}        					
        					if(temp2)
        					{
        						Sys_Ctrl.Sys_Stop=true;
        					}
        					RC_Counter[2]=0;
        				}
        			}
        			else
        			{
        				RC_Counter[2]=0;
        			}
    			}			
    		}
		}
		
        //if(ENABLE == System_Info.Config.Meter_Comm_Enable) //使能电表通信
        if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
        {
			//else if(Grid_First==0xAA) //电网优先
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
			if(Grid_Missed || GRID_CONTACTOR_FAULT) //电网没电
			{
				RC_Counter[3]++;
				if(RC_Counter[3]>2*RC_Time)
				{
					Sys_Ctrl.Sys_Stop=true;
					RC_Counter[3]=0;
				}
			}
			else
			{
				RC_Counter[3]=0;
			}
        }
	}
	else if(ISLAND_RUNNING && (!Switch_to_Grid_Status))  //微网供电运行情况处理
	{
		if(DC_BUS_SYS==1) //直流母线系统
		{
 			//if(Island_First==0xAA) //微网优先
			//微网供电情况下，所有PCS故障情况处理
			temp3=true;
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				//if((!PCS_E_STOP(i)) && PCS_NO_FAULT(i) && (!PCS_Com_Err[i]) && (!MGRID_FAULT))
				if(PCS_Ready_Status[i] && (!MGRID_FAULT)) //有一台正常就继续运行
				{
					temp3=false;
				}
			}
			if(temp3)	//PCS全部故障则执行
			{
                //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
		        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                {
    				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    				if( Grid_Exist &&(!GRID_CONTACTOR_FAULT))
    				{
    					RC_Counter[4]++;
    					if(RC_Counter[4]>2*RC_Time)
    					{
    						Switch_to_Grid_Status=true;
    						Switch_to_Grid_Flag=0;
    						Switch_to_Island_Status=false;
    						Switch_to_Island_Flag=0;
    						RC_Counter[4]=0;
    						return;
    					}
    				}
    				else
    				{
    					Sys_Ctrl.Sys_Stop=true;
    					RC_Counter[4]=0;
    					return;
    				}
				}
				else
				{
                    Sys_Ctrl.Sys_Stop=true;
                    return;
				}
			}
			//微网供电情况下，电池电压低的处理
			if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
            {
                if(Bat_Status.SOC_L30)
                {        
                    //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
                    if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                    {
                        drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
                        if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
                        {
                            RC_Counter[5]++;
                            if(RC_Counter[5]>2*RC_Time)
                            {
                                Switch_to_Grid_Status=true;
                                Switch_to_Grid_Flag=0;
                                Switch_to_Island_Status=false;
                                Switch_to_Island_Flag=0;
                                RC_Counter[5]=0;
                                return;
                            }
                        }
                        else
                        {
                            Sys_Ctrl.Sys_Stop=true;
                            RC_Counter[5]=0;
                            return;
                        }
                    }
                    else
                    {
                        Sys_Ctrl.Sys_Stop=true;
                        return;
                    }
                }
            }
			else
			{
    			if(Bat_Status.Vbat_L30)
    			{
                    //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
    		        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                    {
        				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
        				if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
        				{
        					RC_Counter[5]++;
        					if(RC_Counter[5]>2*RC_Time)
        					{
        						Switch_to_Grid_Status=true;
        						Switch_to_Grid_Flag=0;
        						Switch_to_Island_Status=false;
        						Switch_to_Island_Flag=0;
        						RC_Counter[5]=0;
        						return;
        					}
        				}
        				else
        				{
        					Sys_Ctrl.Sys_Stop=true;
        					RC_Counter[5]=0;
        					return;
        				}
    				}
    				else
    				{
                        Sys_Ctrl.Sys_Stop=true;
                        return;
    				}
    			}
			}
			//并网停机
            if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_MODE ) // 并网模式
            {
                if(ENABLE==System_Info.Config.Power_Distribution_Enable)
                {
					if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
					{
						Switch_to_Grid_Status=true;
						Switch_to_Grid_Flag=0;
						Switch_to_Island_Status=false;
						Switch_to_Island_Flag=0;
						return;
					}
					else
					{
						Sys_Ctrl.Sys_Stop=true;
						return;
					}
                }
                else //不使能配电直接停机
                {
                	Sys_Ctrl.Sys_Stop=true;
					return;
                }
            }            
            //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
	        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
            {			
    			if(Grid_First==0xAA)  //电网优先
    			{
    				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    				if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
    				{
    					RC_Counter[6]++;
    					if(RC_Counter[6]>2*RC_Time)
    					{
    						Switch_to_Grid_Status=true;
    						Switch_to_Grid_Flag=0;
    						Switch_to_Island_Status=false;
    						Switch_to_Island_Flag=0;
    						RC_Counter[6]=0;
    						return;
    					}
    				}
    				else
    				{
    					RC_Counter[6]=0;
    				}
    			}
			}
			
			//启动系统中未运行的PCS
			if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
			{
				RC_Counter[7]=0;
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
				{
					//if((!PCS_E_STOP(i)) && PCS_NO_FAULT(i) && (!PCS_Com_Err[i]) && (!MGRID_FAULT))
					if(PCS_Ready_Status[i] && (!MGRID_FAULT))
					{
						if(!PCS_INVERT_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))
						{
							PCS_Ind_Ctrl[i].Invert=true;
						}
					}
				}
			}
			else
			{
				RC_Counter[7]++;
				if(RC_Counter[7]>2*RC_Time)
				{
					RC_Counter[7]=0;					
                    //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
    		        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                    {					
    					drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    					if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
    					{
    						RC_Counter[8]++;
    						if(RC_Counter[8]>2*RC_Time)
    						{
    							Switch_to_Grid_Status=true;
    							Switch_to_Grid_Flag=0;
    							Switch_to_Island_Status=false;
    							Switch_to_Island_Flag=0;
    							RC_Counter[8]=0;
    							return;
    						}
    					}
    					else
    					{
    						Sys_Ctrl.Sys_Stop=true;
    						RC_Counter[8]=0;
    					}
					}
					else
					{
                        Sys_Ctrl.Sys_Stop=true;
                        return;
					}
					if(EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max-EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min>1000)
					{
						EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word|= 1<<6;			//储能变流器交流未连接
					}
				}
			}
		}
		
		else //交流母线系统
		{
			//if(Island_First==0xAA) //微网优先
			//微网供电情况下，所有PCS故障情况处理
			temp4=true;
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				//if(!PCS_E_STOP(i) && PCS_NO_FAULT(i)&& !PCS_Com_Err[i]&& !MGRID_FAULT)
				if(PCS_Ready_Status[i] && (!MGRID_FAULT))
				{
					temp4=false;
				}
			}
			if(temp4)
			{
                //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
		        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                {
    				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    				if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
    				{
    					RC_Counter[9]++;
    					if(RC_Counter[9]>2*RC_Time)
    					{
    						Switch_to_Grid_Status=true;
    						Switch_to_Grid_Flag=0;
    						Switch_to_Island_Status=false;
    						Switch_to_Island_Flag=0;
    						RC_Counter[9]=0;
    						return;
    					}
    				}
    				else
    				{
    					Sys_Ctrl.Sys_Stop=true;
    					RC_Counter[9]=0;
    					return;
    				}
				}
				else
				{
                    Sys_Ctrl.Sys_Stop=true;                    
                    return;
				}
			}
			
			//微网供电情况下，电池电压低的处理
			if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
			{
    			if(Bat_Status.SOC_L30)			
    			{
                    //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
    		        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                    {
        				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
        				if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
        				{
        					RC_Counter[10]++;
        					if(RC_Counter[10]>2*RC_Time)
        					{
        						Switch_to_Grid_Status=true;
        						Switch_to_Grid_Flag=0;
        						Switch_to_Island_Status=false;
        						Switch_to_Island_Flag=0;
        						RC_Counter[10]=0;
        						return;
        					}
        				}
        				else
        				{
        					Sys_Ctrl.Sys_Stop=true;
        					RC_Counter[10]=0;
        					return;
        				}
    				}
    				else
    				{
                        Sys_Ctrl.Sys_Stop=true;
                        return;
    				}
    			}
			}
			else
			{
    			if(Bat_Status.Vbat_L30)			
    			{
                    //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
    		        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                    {
        				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
        				if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
        				{
        					RC_Counter[10]++;
        					if(RC_Counter[10]>2*RC_Time)
        					{
        						Switch_to_Grid_Status=true;
        						Switch_to_Grid_Flag=0;
        						Switch_to_Island_Status=false;
        						Switch_to_Island_Flag=0;
        						RC_Counter[10]=0;
        						return;
        					}
        				}
        				else
        				{
        					Sys_Ctrl.Sys_Stop=true;
        					RC_Counter[10]=0;
        					return;
        				}
    				}
    				else
    				{
                        Sys_Ctrl.Sys_Stop=true;
                        return;
    				}
    			}
			}
			//切并网
            if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_MODE)
            {
                if(ENABLE==System_Info.Config.Power_Distribution_Enable)
                {
					if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
					{
						Switch_to_Grid_Status=true;
						Switch_to_Grid_Flag=0;
						Switch_to_Island_Status=false;
						Switch_to_Island_Flag=0;
						return;
					}
					else
					{
						Sys_Ctrl.Sys_Stop=true;
						return;
					}
                }
                else
                {
                	Sys_Ctrl.Sys_Stop=true;
                	return;
                }
            }
            //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
	        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
            {
    			if(Grid_First==0xAA)  //电网优先
    			{
    				if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
    				{
    					RC_Counter[11]++;
    					if(RC_Counter[11]>2*RC_Time)
    					{
    						Switch_to_Grid_Status=true;
    						Switch_to_Grid_Flag=0;
    						Switch_to_Island_Status=false;
    						Switch_to_Island_Flag=0;
    						RC_Counter[11]=0;
    						return;
    					}
    				}
    				else
    				{
    					RC_Counter[11]=0;
    				}
    			}
			}
			//启动系统中未运行的PCS及PV
			if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
			{
				RC_Counter[12]=0;
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
				{
					//if(!PCS_E_STOP(i) && PCS_NO_FAULT(i) && !PCS_Com_Err[i] && !MGRID_FAULT)
					if(PCS_Ready_Status[i]&& (!MGRID_FAULT))
					{
						if(!PCS_INVERT_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))
						{
							PCS_Ind_Ctrl[i].Invert=true;
						}
					}
				}
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					if(!PV_Com_Err[i])
					{
						if(PV_Info[i].PV4x.ParaSet.PowerLimit_Set != PV_Ctrl_Data[i].PowerLimit_Set)
						{
						    //PV_Ctrl_Data[i].PowerLimit_Set=1;
							PV_Ind_Ctrl[i].Active_Power_Limit=true;
						}
						else
						{
							if(PV_Info[i].PV4x.ParaSet.Power_ONOFF != 0xCF)
							{
								PV_Ind_Ctrl[i].Power_On = true;
							}
						}
					}
				}
			}
			else
			{
				RC_Counter[12]++;
				if(RC_Counter[12]>2*RC_Time)
				{
					RC_Counter[12]=0;
                    //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
    		        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
                    {
    					drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
    					if(Grid_Exist &&(!GRID_CONTACTOR_FAULT))
    					{
    						RC_Counter[13]++;
    						if(RC_Counter[13]>2*RC_Time)
    						{
    							Switch_to_Grid_Status=true;
    							Switch_to_Grid_Flag=0;
    							Switch_to_Island_Status=false;
    							Switch_to_Island_Flag=0;
    							RC_Counter[13]=0;
    							return;
    						}
    					}
    					else
    					{
    						Sys_Ctrl.Sys_Stop=true;
    						RC_Counter[13]=0;
    						return;
    					}
					}
					else
					{
						Sys_Ctrl.Sys_Stop=true;
						return;
					}
					if(EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max-EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min>1000)
					{
						EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word|= 1<<6;			//储能变流器交流未连接
					}
				}
			}			
		}
	}

	else if(GRID_RUNNING && (!Switch_to_Island_Status))   //电网供电运行情况处理
	{
		if(DC_BUS_SYS==1) //直流母线系统
		{
            //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
	        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
            {
    			if(Island_First==0xAA) //微网优先
    			{
                    if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
                    {
                        if(!Bat_Status.SOC_L30)
                        {
                            RC_Counter[14]++;
                            if(RC_Counter[14]>2*RC_Time)
                            {
                                RC_Counter[14]=0;
                                if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
                                {
                                    temp5=true;
                                    for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                                    {
                                        //if(PCS_E_STOP(i) || (!PCS_NO_FATAL_FAULT(i)) || PCS_Com_Err[i] || MGRID_FAULT)
                                        if(!PCS_Ready_Status[i]|| MGRID_FAULT)
                                        {
                                            temp5=false;
                                        }
                                    }
                                }
                                else //部分正常
                                {
                                    temp5=false;
                                    for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                                    {
                                        if(PCS_Ready_Status[i]&& (!MGRID_FAULT))
                                        {
                                            temp5=true;
                                        }
                                    }
                                }                                
                                if(temp5)
                                {
                                    Switch_to_Island_Status=true;
                                    Switch_to_Island_Flag=0;
                                    Switch_to_Grid_Status=false;
                                    Switch_to_Grid_Flag=0;
                                    return;
                                }
                            }
                        }
                        else
                        {
                            RC_Counter[14]=0;
                        }
                    }
                    else
                    {
        				if(!Bat_Status.Vbat_L30)
        				{
        					RC_Counter[14]++;
        					if(RC_Counter[14]>2*RC_Time)
        					{
        						RC_Counter[14]=0;
                                if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
                                {
            						temp5=true;
            						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            						{
            							if(!PCS_Ready_Status[i]|| MGRID_FAULT)
            							{
            								temp5=false;
            							}
            						}
        						}
        						else //部分正常
        						{
                                    temp5=false;
                                    for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                                    {
                                        if(PCS_Ready_Status[i]&& (!MGRID_FAULT))
                                        {
                                            temp5=true;
                                        }
                                    }
                                }
        						if(temp5)
        						{
        							Switch_to_Island_Status=true;
        							Switch_to_Island_Flag=0;
        							Switch_to_Grid_Status=false;
        							Switch_to_Grid_Flag=0;
        							return;
        						}
        					}
        				}
        				else
        				{
        					RC_Counter[14]=0;
        				}
    				}
    			}
			}
            //切离网
            if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == ISLAND_MODE) //切离网
            {                
                if(ENABLE==System_Info.Config.Power_Distribution_Enable)
				{
					if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
					{
						if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
						{
							temp7=true;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(!PCS_Ready_Status[i]|| MGRID_FAULT || Bat_Status.SOC_L30)
								{
									temp7=false;
								}
							}
						}
						else //部分正常
						{
							temp7=false;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(PCS_Ready_Status[i]&&(!MGRID_FAULT)&&(!Bat_Status.SOC_L30))
								{
									temp7=true;
								}
							}
						}
					}
					else
					{
						if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
						{
							temp7=true;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(!PCS_Ready_Status[i] || MGRID_FAULT || Bat_Status.Vbat_L30)
								{
									temp7=false;
								}
							}
						}
						else //部分正常
						{
							temp7=false;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(PCS_Ready_Status[i]&& (!MGRID_FAULT)&&(!Bat_Status.Vbat_L30))
								{
									temp7=true;
								}
							}
						}
					}
					if(temp7)
					{
						Switch_to_Island_Status=true;
						Switch_to_Island_Flag=0;
						Switch_to_Grid_Status=false;
						Switch_to_Grid_Flag=0;
					}
					else
					{
						Sys_Ctrl.Sys_Stop=true;
					}
				}
                else
                {
                	Sys_Ctrl.Sys_Stop=true;
                }
            }
			
            //if(ENABLE==System_Info.Config.Meter_Comm_Enable)
            if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
			{
				//else if(Grid_First==0xAA)//电网掉电，电网接触器错误
				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
				if(Grid_Missed || GRID_CONTACTOR_FAULT)
				{
					RC_Counter[15]++;
					if(RC_Counter[15]>2*RC_Time)
					{
						RC_Counter[15]=2*RC_Time+1;
						//if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
						if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
						{
							if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
							{
								if(!Bat_Status.SOC_L30)
								{
									RC_Counter[16]++;
									if(RC_Counter[16]>2*RC_Time)
									{
										if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
										{
											temp5=true;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if((!PCS_Ready_Status[i])|| MGRID_FAULT)
												{
													temp5=false;
												}
											}
										}
										else
										{
											temp5=false;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if((PCS_Ready_Status[i])&&(!MGRID_FAULT))
												{
													temp5=true;
												}
											}
										}
										if(temp5)
										{
											Switch_to_Island_Status=true;
											Switch_to_Island_Flag=0;
											Switch_to_Grid_Status=false;
											Switch_to_Grid_Flag=0;
										}
										else
										{
											Sys_Ctrl.Sys_Stop=true;
										}
										RC_Counter[15]=0;
										RC_Counter[16]=0;
									}
								}
								else
								{
									RC_Counter[15]=0;
									RC_Counter[16]=0;
									Sys_Ctrl.Sys_Stop=true;
								}
							}
							else
							{
								if(!Bat_Status.Vbat_L30)
								{
									RC_Counter[16]++;
									if(RC_Counter[16]>2*RC_Time)
									{
										if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
										{
											temp5=true;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if(!PCS_Ready_Status[i]|| MGRID_FAULT)
												{
													temp5=false;
												}
											}
										}
										else
										{
											temp5=false;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if(PCS_Ready_Status[i]&&(!MGRID_FAULT))
												{
													temp5=true;
												}
											}
										}
										if(temp5)
										{
											Switch_to_Island_Status=true;
											Switch_to_Island_Flag=0;
											Switch_to_Grid_Status=false;
											Switch_to_Grid_Flag=0;
										}
										else
										{
											Sys_Ctrl.Sys_Stop=true;
										}
										RC_Counter[15]=0;
										RC_Counter[16]=0;
									}
								}
								else
								{
									RC_Counter[15]=0;
									RC_Counter[16]=0;
									Sys_Ctrl.Sys_Stop=true;
								}
							}
						}
						else
						{
							RC_Counter[15]=0;
							RC_Counter[16]=0;
							Sys_Ctrl.Sys_Stop=true;
							return;
						}
						//RC_Counter[15]=2*RC_Time+1;
						//return;
					}
				}
				else
				{
					RC_Counter[15]=0;
				}
			}
		}
		else //交流母线系统
		{		
            //if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
	        if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
            {
    			if(Island_First==0xAA) //微网优先
    			{
                    if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
                    {
        				if(!Bat_Status.SOC_L30)
        				{
        					RC_Counter[16]++;
        					if(RC_Counter[16]>2*RC_Time)
        					{
                                RC_Counter[16]=0;
                                if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
                                {
            						temp6=true;
            						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            						{
            							if(!PCS_Ready_Status[i]|| MGRID_FAULT)
            							{
            								temp6=false;
            							}
            						}
        						}
        						else
        						{
            						temp6=false;
            						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            						{
            							if(PCS_Ready_Status[i]&&(!MGRID_FAULT))
            							{
            								temp6=true;
            							}
            						}
        						}        						
        						if(temp6)
        						{
        							Switch_to_Island_Status=true;
        							Switch_to_Island_Flag=0;
        							Switch_to_Grid_Status=false;
        							Switch_to_Grid_Flag=0;
        							return;
        						}
        					}
        				}
        				else
        				{
        					RC_Counter[16]=0;
        				}
    				}
                    else
                    {
        				if(!Bat_Status.Vbat_L30)
        				{
        					RC_Counter[16]++;
        					if(RC_Counter[16]>2*RC_Time)
        					{
        						RC_Counter[16]=0;
                                if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
                                {
            						temp6=true;
            						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            						{
            							if(!PCS_Ready_Status[i]|| MGRID_FAULT)
            							{
            								temp6=false;
            							}
            						}
        						}
        						else
        						{
            						temp6=false;
            						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
            						{
            							if(PCS_Ready_Status[i]&&(!MGRID_FAULT))
            							{
            								temp6=true;
            							}
            						}
        						}
        						if(temp6)
        						{
        							Switch_to_Island_Status=true;
        							Switch_to_Island_Flag=0;
        							Switch_to_Grid_Status=false;
        							Switch_to_Grid_Flag=0;
        							return;
        						}
        					}
        				}
        				else
        				{
        					RC_Counter[16]=0;
        				}
    				}
    			}
			}
			//切离网
            if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == ISLAND_MODE) //切离网
            {                
                if(ENABLE==System_Info.Config.Power_Distribution_Enable)
				{
					if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
					{
						if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
						{
							temp7=true;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if((!PCS_Ready_Status[i])||MGRID_FAULT || Bat_Status.SOC_L30)
								{
									temp7=false;
								}
							}
						}
						else
						{
							temp7=false;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(PCS_Ready_Status[i]&&(!MGRID_FAULT)&& (!Bat_Status.SOC_L30))
								{
									temp7=true;
								}
							}
						}
					}
					else
					{
						if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
						{
							temp7=true;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if((!PCS_Ready_Status[i])|| MGRID_FAULT || Bat_Status.Vbat_L30)
								{
									temp7=false;
								}
							}
						}
						else
						{
							temp7=false;
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(PCS_Ready_Status[i]&&(!MGRID_FAULT)&&(!Bat_Status.Vbat_L30))
								{
									temp7=true;
								}
							}
						}
					}
					if(temp7)
					{
						Switch_to_Island_Status=true;
						Switch_to_Island_Flag=0;
						Switch_to_Grid_Status=false;
						Switch_to_Grid_Flag=0;
					}
					else
					{
						Sys_Ctrl.Sys_Stop=true;
					}
				}
                else
                {
                	Sys_Ctrl.Sys_Stop=true;
                }
            }
            
            //if(ENABLE==System_Info.Config.Meter_Comm_Enable) //电表通信使能
            if(System_Info.Config.Meter_Position_Enable[0] == ENABLE)  //电网侧电表使能
			{
				//else if(Grid_First==0xAA)//电网优先
				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
				if(Grid_Missed || GRID_CONTACTOR_FAULT)   //电网没电，接触器故障
				{
					RC_Counter[17]++;
					if(RC_Counter[17]>2*RC_Time)
					{
						RC_Counter[17]=2*RC_Time+1;
						//if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE) //并离网切换
						if((Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode == GRID_ISLAND_MODE)&&(ENABLE==System_Info.Config.Power_Distribution_Enable))  //并离网切换必须使能配电
						{
							if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
							{
								if(!Bat_Status.SOC_L30)
								{
									RC_Counter[18]++;
									if(RC_Counter[18]>2*RC_Time)
									{
										if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
										{
											temp5=true;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if((!PCS_Ready_Status[i])|| MGRID_FAULT)
												{
													temp5=false;
												}
											}
										}
										else
										{
											temp5=false;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if(PCS_Ready_Status[i]&&(!MGRID_FAULT))
												{
													temp5=true;
												}
											}
										}
										if(temp5)
										{
											Switch_to_Island_Status=true;
											Switch_to_Island_Flag=0;
											Switch_to_Grid_Status=false;
											Switch_to_Grid_Flag=0;
										}
										else
										{
											Sys_Ctrl.Sys_Stop=true;
										}
										RC_Counter[17]=0;
										RC_Counter[18]=0;
										return;
									}
								}
								else
								{
									RC_Counter[17]=0;
									RC_Counter[18]=0;
									Sys_Ctrl.Sys_Stop=true;
									return;
								}
							}
							else
							{
								if(!Bat_Status.Vbat_L30)
								{
									RC_Counter[18]++;
									if(RC_Counter[18]>2*RC_Time)
									{
										if(System_Info.Config.PCS_Island_Ctrl_Mode==0)//全部正常
										{
											temp5=true;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if(!PCS_Ready_Status[i]|| MGRID_FAULT)
												{
													temp5=false;
												}
											}
										}
										else
										{
											temp5=false;
											for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
											{
												if(PCS_Ready_Status[i]&& (!MGRID_FAULT))
												{
													temp5=true;
												}
											}
										}
										if(temp5)
										{
											Switch_to_Island_Status=true;
											Switch_to_Island_Flag=0;
											Switch_to_Grid_Status=false;
											Switch_to_Grid_Flag=0;
										}
										else
										{
											Sys_Ctrl.Sys_Stop=true;
										}
										RC_Counter[17]=0;
										RC_Counter[18]=0;
										return;
									}
								}
								else
								{
									RC_Counter[17]=0;
									RC_Counter[18]=0;
									Sys_Ctrl.Sys_Stop=true;
									return;
								}
							}
						}
						else
						{
							RC_Counter[17]=0;
							RC_Counter[18]=0;
							Sys_Ctrl.Sys_Stop=true;
							return;
						}
					}
				}
				else
				{
					RC_Counter[17]=0;
				}
			}

			//交流母线系统，PCS不允许并网时做如下处理。
			if(GRID_TYPE==0)  //非并网
			{
				//电网供电情况，PCS自充电控制
				if(EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Min >= PV_OK_DC_V)
				{
                    if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
                    {
                        if(!Bat_Status.SOC_G90)
                        {
                            //启动PCS，光伏开机充电
                            for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                            {
                                if(!PCS_Com_Err[i])
                                {
                                    if(!PCS_INVERT_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)&&(!MGRID_FAULT))
                                    {
                                        PCS_Ctrl_Data[i].Ind_Inverter_V = EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
                                        PCS_Ctrl_Data[i].Ind_Inverter_F = EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
                                        PCS_Ind_Ctrl[i].Invert = true;
                                    }
                                }
                            }
                        }
                        else if(Bat_Status.SOC_G95)
                        {
                            for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                            {
                                if(!PCS_Com_Err[i])
                                {
                                    if(PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
                                    {
                                        PCS_Ind_Ctrl[i].Stop = true;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
    					if(!Bat_Status.Vbat_G90)
    					{
    					    //启动PCS，光伏开机充电
    						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    						{
    							if(!PCS_Com_Err[i])
    							{
    								if(!PCS_INVERT_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)&&!MGRID_FAULT)
    								{
    									PCS_Ctrl_Data[i].Ind_Inverter_V = EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
    									PCS_Ctrl_Data[i].Ind_Inverter_F = EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
    									PCS_Ind_Ctrl[i].Invert = true;
    								}
    							}
    						}
    					}
    					else if(Bat_Status.Vbat_G95)
    					{
    						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    						{
    							if(!PCS_Com_Err[i])
    							{
    								if(PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
    								{
    									PCS_Ind_Ctrl[i].Stop = true;
    								}
    							}
    						}
    					}
					}					
					if(MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
					{
						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
						{
							if(!PV_Com_Err[i])
							{
								if(PV_Info[i].PV4x.ParaSet.PowerLimit_Set != PV_Ctrl_Data[i].PowerLimit_Set)
								{
 									PV_Ctrl_Data[i].PowerLimit_Set=1;
									PV_Ind_Ctrl[i].Active_Power_Limit = true;
								}
								else
								{
									if(PV_Info[i].PV4x.ParaSet.Power_ONOFF != 0xCF)
									{
										PV_Ind_Ctrl[i].Power_On = true;
									}
								}
							}
						}
					}
				}
				else
				{
					if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
					{
						if(Bat_Status.SOC_L30)
						{
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(!PCS_Com_Err[i])
								{
									if(PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
									{
										PCS_Ind_Ctrl[i].Stop = true;
									}
								}
							}
						}
					}
					else
					{
						if(Bat_Status.Vbat_L30)
						{
							for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
							{
								if(!PCS_Com_Err[i])
								{
									if(PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
									{
										PCS_Ind_Ctrl[i].Stop = true;
									}
								}
							}
						}
					}
				}
			}	
			else //并网
			{
                for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
                {
                    if(!PV_Com_Err[i])
                    {
                        if(PV_Info[i].PV4x.ParaSet.PowerLimit_Set != PV_Ctrl_Data[i].PowerLimit_Set)
                        {
                            //PV_Ctrl_Data[i].PowerLimit_Set=1;
                            PV_Ind_Ctrl[i].Active_Power_Limit=true;
                        }
                        else
                        {
                            if(PV_Info[i].PV4x.ParaSet.Power_ONOFF != 0xCF)
                            {
                                PV_Ind_Ctrl[i].Power_On = true;
                            }
                        }
                    }
                }
            }
		}
	}
}

//电网供电切换
void Switch_to_Grid(void)
{
	static uint16_t StG_time=3;
	bool temp=true;
	switch(Switch_to_Grid_Flag)
	{
		case 0:
			for(uint8_t i=0;i<20;i++)
			{
				StG_Counter[i]=0;
			}
			Switch_to_Grid_Flag=1;
			break;
		case 1:
			(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);	//断微网接触器
			MGrid_Contactor_Control_State = OP_LOW;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
			//if((!DIN(MGrid_Contacter))|| (MGRID_CONTACTOR_FAULT)) // yuan
            if(!DIN(MGrid_Contacter))
			{
                if(StG_Counter[11]++>1*StG_time)
                {
                    StG_Counter[11]=0;
    				Switch_to_Grid_Flag=2;
				}				
				Load_Power_Status=0;
			}
			else
			{
				StG_Counter[0]++;
			}

			if(StG_Counter[0]>2*StG_time)
			{
				StG_Counter[0]=0;
				MGRID_CONTACTOR_FAULT_SET;
				Switch_to_Grid_Status=false;
				Switch_to_Grid_Flag=0;
				Sys_Ctrl.Sys_Stop=true;
			}			
		break;
		case 2:
			(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_HIGH,0);	//合电网接触器
			Grid_Contactor_Control_State = OP_HIGH;		
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
			if(DIN(Grid_Contacter))
			{
				Switch_to_Grid_Flag=3;
				Load_Power_Status=1;
				EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<4;		
			}
			else
			{
				StG_Counter[1]++;
			}

			if(StG_Counter[1]>2*StG_time)
			{
				StG_Counter[1]=0;
				GRID_CONTACTOR_FAULT_SET;
				Switch_to_Grid_Status=false;
				Switch_to_Grid_Flag=0;	
				Sys_Ctrl.Sys_Stop=true;
			}
		break;
		case 3:
			temp=true;
			StG_Counter[2]++;
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				if(!PCS_Com_Err[i])
				{
					PCS_Ind_Ctrl[i].Stop = true;
				}
			}
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				if(!PCS_Com_Err[i])
				{
					if(PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
					{
						temp=false;
					}
				}
				else
				{
                    temp=false;
				}
			}
			if(temp)
			{
				if(GRID_TYPE==0) //非并网
				{
					Switch_to_Grid_Flag=5;
				}
				else 
				{//并网
					StG_Counter[4]++;
					if(StG_Counter[4]>6*StG_time) //6S
					{
						StG_Counter[4]=0;
						Switch_to_Grid_Flag=4;
					}
				}
				StG_Counter[2]=0;
			}

			if(StG_Counter[2]>EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*15*StG_time)
			{
				StG_Counter[2]=0;
				Switch_to_Grid_Status=false;
				Switch_to_Grid_Flag=0;				
			}			
		break;
		case 4:
			if(MGRID_CONTACTOR_FAULT)
			{
				Switch_to_Grid_Flag=5;
			}
			else
			{
				(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_HIGH,0);	//合微网接触器
				MGrid_Contactor_Control_State = OP_HIGH;	
				drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
				if(DIN(MGrid_Contacter))
				{
					Switch_to_Grid_Flag=5;
				}
				else
				{
					StG_Counter[3]++;
				}
			}

			if(StG_Counter[3]>2*StG_time)
			{
				StG_Counter[3]=0;
				MGRID_CONTACTOR_FAULT_SET;
				Switch_to_Grid_Flag=5;				
			}
		break;
		case 5:
		default:
			Switch_to_Grid_Status=false;
			Switch_to_Grid_Flag=0;	
		break;
	}
}


//微网供电切换
void Switch_to_Island(void)
{
	static uint16_t StI_time=3;
	bool temp=true;	
    static uint8_t PCS_tsn1;
	
	drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
	//if(!DIN(5) || GRID_CONTACTOR_FAULT)
	if(Grid_Missed || GRID_CONTACTOR_FAULT)
	{
		EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<1; //微网供电启动
	}
	
	switch(Switch_to_Island_Flag)
	{
		case 0:
			for(uint8_t i=0;i<20;i++)
			{
				StI_Counter[i]=0;
			}
			Switch_to_Island_Flag=1;
			PCS_tsn1=0;
			break;
		case 1:	
            StI_Counter[0]++;
			temp=true;			
            if(GRID_TYPE == 1) //并网型
            {
    			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    			{
    				if(!PCS_Com_Err[i])
    				{
    					if(PCS_RUNNING(PCS_Info[i].PCS3x.RCommunication.Run_Status))
    					{
    						PCS_Ind_Ctrl[i].Stop=true;
    						temp=false;
    					}
    				}
    				else
    				{
                        temp=false;
    				}
    			}
			}	
			if(temp)
			{
				StI_Counter[6]++;
				if(StI_Counter[6]>1*StI_time)
				{
					StI_Counter[6]=0;
					Switch_to_Island_Flag=2;
					StI_Counter[0]=0;
				}
			}

			if(StI_Counter[0]>EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*15*StI_time) //停机超时时间15s
			{
				StI_Counter[0]=0;
				Switch_to_Island_Flag=2;				
			}
		break;
		case 2:
			(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);	//断微网接触器
			MGrid_Contactor_Control_State = OP_LOW;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
			if(!DIN(MGrid_Contacter))
			{
				if(StI_Counter[11]++>1*StI_time)
				{
                    StI_Counter[11]=0;
                    Switch_to_Island_Flag=3;
				}
			}
			else
			{
				StI_Counter[1]++;
			}

			if(StI_Counter[1]>2*StI_time)
			{
				StI_Counter[1]=0;
				MGRID_CONTACTOR_FAULT_SET;
				Switch_to_Island_Status=false;
				Switch_to_Island_Flag=0;
			}
		break;
		case 3:    
            if(System_Info.Config.PCS_Island_Ctrl_Mode==0) //全部正常
            {
    			temp=true;
                if(PCS_tsn1<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
                {
                    temp=false;
                    if(PCS_INVERT_STATUS(PCS_Info[PCS_tsn1].PCS3x.RCommunication.Run_Status)&&PCS_VOLTAGE_OK(PCS_Info[PCS_tsn1].PCS3x.Rmeasure.VAB))
                    {
                        PCS_tsn1++;
                    }
                    else
                    {
                        PCS_Ctrl_Data[PCS_tsn1].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
                        PCS_Ctrl_Data[PCS_tsn1].Ind_Inverter_F=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
                        PCS_Ind_Ctrl[PCS_tsn1].Invert=true;
                    }
                }else{}	
            }
            else //部分正常
            {
    			temp=true;
                if(PCS_tsn1<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
                {
                    temp=false;
                    if((PCS_INVERT_STATUS(PCS_Info[PCS_tsn1].PCS3x.RCommunication.Run_Status)&&PCS_VOLTAGE_OK(PCS_Info[PCS_tsn1].PCS3x.Rmeasure.VAB))
					 ||(!PCS_Ready_Status[PCS_tsn1]))
                    {
                        PCS_tsn1++;
                    }
                    else
                    {
                        PCS_Ctrl_Data[PCS_tsn1].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
                        PCS_Ctrl_Data[PCS_tsn1].Ind_Inverter_F=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
                        PCS_Ind_Ctrl[PCS_tsn1].Invert=true;
                    }
                }else{}	
            }
			if(temp)
			{
				StI_Counter[5]++;
				if(StI_Counter[5]>2*StI_time)
				{
					StI_Counter[5]=0;
					Switch_to_Island_Flag=4;
				}
			}
			else
			{
				StI_Counter[5]=0;
			}
			
			StI_Counter[2]++;
			if(StI_Counter[2]>EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number*60*StI_time)
			{
				StI_Counter[2]=0;
				Switch_to_Island_Status=false;
				Switch_to_Island_Flag=0; //切换离网超时
				
			}			
		break;
		case 4:
			(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_LOW,0);	//断电网接触器
			Grid_Contactor_Control_State = OP_LOW;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
			if(!DIN(Grid_Contacter))
			{
                if(StI_Counter[12]++>1*StI_time)
                {
                    StI_Counter[12]=0;
    				Switch_to_Island_Flag=5;
				}				
				Load_Power_Status=0;
			}
			else
			{
				StI_Counter[3]++;
			}

			if(StI_Counter[3]>2*StI_time)
			{
				GRID_CONTACTOR_FAULT_SET;
				Switch_to_Island_Status=false;
				Switch_to_Island_Flag=0;
				StI_Counter[3]=0;
			}
		break;
		case 5:
			(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_HIGH,0);	//合微网接触器
			MGrid_Contactor_Control_State = OP_HIGH;
			drv_read_gpio(GPIO_INPUT_PATH,dig_input,14);
			if(DIN(MGrid_Contacter))
			{
				Switch_to_Island_Flag=6;
				Load_Power_Status=2;
			}
			else
			{
				StI_Counter[4]++;
			}

			if(StI_Counter[4]>2*StI_time)
			{
				MGRID_CONTACTOR_FAULT_SET;
				Switch_to_Island_Status=false;
				Sys_Ctrl.Sys_Stop=true;
				Switch_to_Island_Flag=0;
				StI_Counter[4]=0;
			}
		break;
		case 6:
		default:
			Switch_to_Island_Status=false;
			Switch_to_Island_Flag=0;
			EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status=1<<3;
		break;
	}

}


/*//if((PCS_INVERT_STATUS(PCS_Info[PCS_tsn].PCS3x.RCommunication.Run_Status)&& ZERO_VOLTAGE(PCS_Info[PCS_tsn].PCS3x.Rmeasure.VAB))
// ||(PCS_Com_Err[PCS_tsn]|| PCS_EMG_STATUS(PCS_Info[PCS_tsn].PCS3x.RCommunication.Run_Status)||!PCS_NO_FATAL_FAULT(PCS_tsn)))

//if((PCS_INVERT_STATUS(PCS_Info[PCS_tsn].PCS3x.RCommunication.Run_Status)&& ZERO_VOLTAGE(PCS_Info[PCS_tsn].PCS3x.Rmeasure.VAB))
// ||(!PCS_Ready_Status[PCS_tsn]))

//修改主PCS输出电压为额定值
//  if(PCS_Soft_Start_OK && !PCS_Master_Modify_OK)
//  {
//      if(!ZERO_VOLTAGE(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
//      {
//          IS_Counter[9]++;
//          if(IS_Counter[9]>2*IS_Time)
//          {
//              for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
//              {
//                  if(PCS_Info[i].PCS3x.Rmeasure.Master_Slave==1)
//                  {
//                      PCS_Ctrl_Data[i].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
//                  }
//                  else
//                  {
//                      PCS_Ctrl_Data[i].Ind_Inverter_V=SOFT_START_V;
//                  }
//              }
//              PCS_Master_Modify_OK=true;
//              for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
//              {
//                  if(!PCS_Com_Err[i])
//                  {
//                      if(PCS_Info[i].PCS4x.RAdjust.Ind_Inverter_V!=PCS_Ctrl_Data[i].Ind_Inverter_V)
//                      {
//                          PCS_Ind_Ctrl[i].Invert=true;
//                          PCS_Master_Modify_OK=false;
//                      }
//                  }
//              }
//          }
//      }
//      else
//      {
//          IS_Counter[9]=0;
//          EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 |= 1<<18;         //微网供电启动失败      
//      }
//  }
//          
//  //修改PCS输出电压为额定值
//  if(PCS_Master_Modify_OK && !PCS_Modify_OK && MG_VOLTAGE_OK(EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage))
//  {
//      IS_Counter[11]++;
//      if(IS_Counter[11]>2*IS_Time)
//      {
//          for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
//          {
//              PCS_Ctrl_Data[i].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
//          }
//          PCS_Modify_OK=true;
//          for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
//          {
//              if(!PCS_Com_Err[i])
//              {
//                  if(PCS_Info[i].PCS4x.RAdjust.Ind_Inverter_V!=PCS_Ctrl_Data[i].Ind_Inverter_V)
//                  {
//                      PCS_Ind_Ctrl[i].Invert=true;
//                      PCS_Modify_OK=false;
//                  }
//              }
//          }
//      }
//  }
//  else
//  {
//      IS_Counter[11]=0;
//  }*/




