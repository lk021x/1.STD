#include "includes.h"

//EMS参数计算
void EMS_Calc()
{
	uint16_t temp1_u16;
 	//int16_t  temp1_s16,temp2_s16;
 	//int32_t  temp1_s32;
	uint32_t temp1_u32,temp2_u32;
	float __temp;
	//EMS_Controller
	//Equip_Property
 	//RMeasure


	//PCS设置参数更新
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{

		//PCS_Ctrl_Data_Rec[which].Grid_Island						=
		//PCS_Ctrl_Data_Rec[which].Grid_Mode_Set					=
		PCS_Ctrl_Data[i].Island_Mode_Set   = 	PCS_Ctrl_Data_Rec[i].Island_Mode_Set	;
		//PCS_Ctrl_Data_Rec[which].Island_Mode_ONOFF				=
		PCS_Ctrl_Data[i].Qvar_Adj_Mode	    =	PCS_Ctrl_Data_Rec[i].Qvar_Adj_Mode		;
		PCS_Ctrl_Data[i].Grid_V_Drop_Mode   =    PCS_Ctrl_Data_Rec[i].Grid_V_Drop_Mode	;
		PCS_Ctrl_Data[i].Const_I_Mode_I     =    PCS_Ctrl_Data_Rec[i].Const_I_Mode_I	;
		PCS_Ctrl_Data[i].Const_V_Mode_V     =    PCS_Ctrl_Data_Rec[i].Const_V_Mode_V	;
		PCS_Ctrl_Data[i].Const_V_Mode_I	    =    PCS_Ctrl_Data_Rec[i].Const_V_Mode_I    ;
		PCS_Ctrl_Data[i].Const_P_DC_Mode_P  =	 PCS_Ctrl_Data_Rec[i].Const_P_DC_Mode_P	;
		PCS_Ctrl_Data[i].Const_P_AC_Mode_P  =    PCS_Ctrl_Data_Rec[i].Const_P_AC_Mode_P	;
		PCS_Ctrl_Data[i].Ind_Inverter_V		=    PCS_Ctrl_Data_Rec[i].Ind_Inverter_V	;
		PCS_Ctrl_Data[i].Ind_Inverter_F		=    PCS_Ctrl_Data_Rec[i].Ind_Inverter_F	;
		PCS_Ctrl_Data[i].Power_Factor       = 	 PCS_Ctrl_Data_Rec[i].Power_Factor      ;
		PCS_Ctrl_Data[i].Qvar_Power_Ratio   =	 PCS_Ctrl_Data_Rec[i].Qvar_Power_Ratio  ;
	}

	//PV_Energy
	EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy = 0;
	EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Reactive_Energy = 0;	
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
	{
		if(!PV_Com_Err[i])
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy += PV_Info[i].PV3x.RunParameter.Total_Generation;
		}
	}
	
	//PCS_Energy
	EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy = 0;
	EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy = 0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		if(!PCS_Com_Err[i])
		{
			EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy += PCS_Info[i].PCS3x.Rmeasure.Total_Charge_Energe;
			EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy += PCS_Info[i].PCS3x.Rmeasure.Total_Discharge_Energe;
		}
	}

	//DCDC_Energy
	EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Charge_Energy = 0;
	EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Discharge_Energy = 0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
	{
        if(System_Info.Config.DCDC_Type==1)//单向DCDC
        {
    		if(!DCDC_Com_Err[i])
    		{
    			EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Charge_Energy    +=
    			(DCDC_Info[i].Module_1.Module_Output_E_Total+DCDC_Info[i].Module_2.Module_Output_E_Total
    			+DCDC_Info[i].Module_3.Module_Output_E_Total);
    		}
        }        
        else if(System_Info.Config.DCDC_Type==2)//双向DCDC
        {
    		if(!DCDC_Com_Err[i])
    		{
    			EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Charge_Energy    += DCDC_Info[i].Module_T.Total_Charge_Energy;
    			EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Discharge_Energy += DCDC_Info[i].Module_T.Total_Discharge_Energy;
    		}
		}
	}
	
 	//EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Reactive_Energy;
	//Load_Energy
 	//EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.Load_Total_Active_Energy;
 	//EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.Load_Total_Reactive_Enerty;
	//Diesel_Energy
 	//EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.Diesel_Total_Active_Energy;
 	//EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.Diesel_Total_Reactive_Energy;
	
	//AGC
	//RMeasure
	//PCS_Online_Num
	EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num = 0;
	EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Rated_Output_Power = 0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
        if(PCS_Ready_Status[i])
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num++;
			EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Rated_Output_Power += PCS_Info[i].PCS3x.Rmeasure.Rated_Output_Power;  // 0.1kw
		}
	}

	//PV_Online_Num
	EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num = 0;
	EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power=0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
	{
		if(!PV_Com_Err[i]) // yuan
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num++;
			EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power+=PV_Info[i].PV3x.RunParameter.Rated_Output_Power; // 0.1kw
		}
	}

    //BMS Online Num
	EMS_Info.EMS3x.AGC_Info.RMeasure.BMS_Online_Num = 0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
	{
		if(!BMS_Com_Err[i])
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.BMS_Online_Num++;
		}
	}
	
    //DCDC Online Num
	EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Online_Num = 0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
	{
		if(!DCDC_Com_Err[i])
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Online_Num++;
		}
	}
	
	//Load_Online_Num
 	//EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Online_Num;
 	//EMS_Info.EMS3x.AGC_Info.RMeasure.Diesel_Online_Num;
	//PCS_Useable_Propertion
	if(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number!=0)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Useable_Propertion=
									(uint16_t)(((float)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num)/
													((float)EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)*1000.0);
	}
	else
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Useable_Propertion=0;
	}
	//PV_Useable_Propertion
	if(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number!=0)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Useable_Propertion=
										(uint16_t)(((float)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num)/
														((float)EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number)*1000.0);
	}
	else
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Useable_Propertion=0;
	}
	
    ////iesel_Useable_Propertion
    //EMS_Info.EMS3x.AGC_Info.RMeasure.Diesel_Useable_Propertion;
    //PCC_Power
    //if((METER_POS==1)||(METER_POS==3))
    //{
    //  EMS_Info.EMS3x.AGC_Info.RMeasure.PCC_Active_Power=(uint16_t)((float)gATTPara.dis_Pt/100.0);
    //  EMS_Info.EMS3x.AGC_Info.RMeasure.PCC_Reactive_Power=(int16_t)((float)gATTPara.dis_Qt_PC/100.0);
    // 	EMS_Info.EMS3x.AGC_Info.RMeasure.PCC_Powerfactor=(int16_t)gATTPara.dis_Pft;
    // 	EMS_Info.EMS3x.AGC_Info.RMeasure.PCC_Voltage=(uint16_t)((gATTPara.dis_Va+gATTPara.dis_Vb+gATTPara.dis_Vc)/3);
    // 	EMS_Info.EMS3x.AGC_Info.RMeasure.PCC_Frequency=(uint16_t)((float)gATTPara.dis_Fre/100.0);
    //}
    
	//MG Voltage and Frequency
	EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage=0;
	EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency=0;
	EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max=MINU16;
	EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min=MAXU16;

	temp1_u16=temp1_u32=temp2_u32=0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		if(!PCS_Com_Err[i])
		{
			temp1_u16++;
			temp1_u32+=(PCS_Info[i].PCS3x.Rmeasure.VAB+PCS_Info[i].PCS3x.Rmeasure.VBC+PCS_Info[i].PCS3x.Rmeasure.VCA)/3;
			EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max=MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max,
														(PCS_Info[i].PCS3x.Rmeasure.VAB+PCS_Info[i].PCS3x.Rmeasure.VBC+PCS_Info[i].PCS3x.Rmeasure.VCA)/3);
			EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min=MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min,
														(PCS_Info[i].PCS3x.Rmeasure.VAB+PCS_Info[i].PCS3x.Rmeasure.VBC+PCS_Info[i].PCS3x.Rmeasure.VCA)/3);
			temp2_u32+=PCS_Info[i].PCS3x.Rmeasure.Grid_Frequency;
		}

		//if((!PCS_Com_Err[i]) && PCS_VOLTAGE_OK(PCS_Info[i].PCS3x.Rmeasure.VAB) && PCS_FREQUENCY_OK(PCS_Info[i].PCS3x.Rmeasure.Grid_Frequency))
		//{
		//	temp1_u16++;
		//	temp1_u32+=(PCS_Info[i].PCS3x.Rmeasure.VAB+PCS_Info[i].PCS3x.Rmeasure.VBC+PCS_Info[i].PCS3x.Rmeasure.VCA)/3;
		//	temp2_u32+=PCS_Info[i].PCS3x.Rmeasure.Grid_Frequency;
		//}

	}

	if (temp1_u16 != 0)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage   = (uint16_t) (temp1_u32 / (uint32_t) temp1_u16);
		EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency = (uint16_t) (temp2_u32 / (uint32_t) temp1_u16);
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency > 9999)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency = 9999;
		}
	}
	else
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max = 0;
		EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Min = 0;
		EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage = 0;
		EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency = 0;
	}


	//PCS_Power
	EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power=0;
	EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power=0;
	EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor=1000;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
	{
		if(!PCS_Com_Err[i])
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power+=PCS_Info[i].PCS3x.Rmeasure.Active_Power;
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power+=PCS_Info[i].PCS3x.Rmeasure.Reactive_Power;
		}
	}
	
	__temp=sqrt((uint32_t)pow(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power,2)+
								 (uint32_t)pow(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power,2));
	if(__temp>0)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor=
					(int16_t)(abs((float)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power)/__temp*1000.0);
	}
	else
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor=1000;
	}
	
	if(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power<0)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor*=-1;
	}

	//PV_Power
	EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power = 0;
	EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power = 0;
	EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Powerfactor = 1000;
	//temp1_s32=0;
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
	{
		if (!PV_Com_Err[i])
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power   += PV_Info[i].PV3x.RunParameter.Total_Active_Power;
			EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power += PV_Info[i].PV3x.RunParameter.Reactive_Power;
		}
	}		
	
	__temp=sqrt((uint32_t)pow(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power,2)+
								 (uint32_t)pow(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power,2));
	if(__temp>0)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Powerfactor=
					(int16_t)((float)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power/__temp*1000.0);
	}
	else
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Powerfactor = 1000;
	}

	if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power < 0)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor *= -1;
	}

    //DCDC_Power
	EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Active_Power=0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
	{
		if(System_Info.Config.DCDC_Type==1)//单向DCDC
		{
		    if(!DCDC_Com_Err[i])
		    {
                EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Active_Power+=
                (DCDC_Info[i].Module_1.Module_Charge_P+DCDC_Info[i].Module_2.Module_Charge_P
                 +DCDC_Info[i].Module_3.Module_Charge_P);
		    }
		}
        else if(System_Info.Config.DCDC_Type==2)//双向DCDC
        {
    		if(!DCDC_Com_Err[i])
    		{
    			EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Active_Power+=DCDC_Info[i].Module_T.Total_Active_Power;
    		}
        }
	}		

	//Load_Power
    //EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power=(uint16_t) (AMMETER_Info[0].AMMETER4x.Pt);
    //EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power=(int16_t) (AMMETER_Info[0].AMMETER4x.Qt);
    //EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Powerfactor=(int16_t) AMMETER_Info[0].AMMETER4x.PFt;


	//Grid_Power
	if(System_Info.Config.Meter_Position_Enable[0]==ENABLE)
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power   = (int32_t)Meter_Info[0].Total_active_P;
		EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Reactive_Power = (int32_t)Meter_Info[0].Total_Q;
	}
	else   //没电网电表
	{
		if(ISLAND_RUNNING)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power = 0;
			EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Reactive_Power = 0;
		}
		else if(GRID_RUNNING)
		{
			if(GRID_TYPE==0)  //非并网
			{
				EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power =   -EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power    ;
				EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Reactive_Power =  EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power  ;
			}
			else if(GRID_TYPE == 1) //并网
			{
				EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power =
						(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power + EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power;
				EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Reactive_Power = 0;
			}
		}
		else
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power = 0;
			EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Reactive_Power = 0;
		}
	}

	//load power
	if(System_Info.Config.Meter_Position_Enable[1]==ENABLE)
	{
		//load
		EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage        = (uint16_t)((Meter_Info[1].VAB+Meter_Info[1].VBC+Meter_Info[1].VCA)/3);
		EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Frequency      = (uint16_t)Meter_Info[1].Freq;

		EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power   = (int32_t)Meter_Info[1].Total_active_P;
		EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power = (int32_t)Meter_Info[1].Total_Q;
	}
	else
	{
		//calc
		if(ISLAND_RUNNING)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage        = EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage;
			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Frequency      = EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency;

			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power   =
					(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power + EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power;
		}
		else if(GRID_RUNNING)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage         = (uint16_t)((Meter_Info[0].VAB + Meter_Info[0].VBC + Meter_Info[0].VCA)/3);
			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Frequency       = Meter_Info[0].Freq;

			if(GRID_TYPE==0)  //非并网
			{
				EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power   = -EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power;
			}
			else if(GRID_TYPE == 1) //并网
			{
				EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power   =
						(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power + EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power;
			}
		}
		else
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage         = 0;
			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Frequency       = 0;
			EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power    = 0;
		}
	}

	if(System_Info.Config.Meter_Position_Enable[2]==ENABLE)
	{
		//PV
		EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power   = (uint32_t)Meter_Info[2].Total_active_P;
		EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power = (int32_t)Meter_Info[2].Total_Q;
	}

	//SOC
	//RMeasure
	//SOC Max、Min、Average
	if( System_Info.Config.BMS_Type == BMS_None)
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver=MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max=MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min=MAXU16;
		temp1_u16=temp1_u32=0;
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++) //基数
		{
			if(!PCS_Com_Err[i])      // yuan
			{
				temp1_u16++;
				temp1_u32+=PCS_Info[i].PCS3x.Battery.Bat_SOC;
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max=
								MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max,PCS_Info[i].PCS3x.Battery.Bat_SOC);
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min=
								MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min,PCS_Info[i].PCS3x.Battery.Bat_SOC);
			}
		}
		if (temp1_u16 != 0)
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver = temp1_u32 / temp1_u16;
		}
		else
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min = 0;
		}
	}
	else if ((System_Info.Config.BMS_Type == BMS_68AH) || (System_Info.Config.BMS_Type == BMS_94AH))
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver = MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max = MINU16;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min = MAXU16;
		temp1_u16 = temp1_u32 = 0;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (!BMS_Com_Err[i]) //yuan
			{
				temp1_u16++;
				temp1_u32+=BMS_Info[i].Sys_SOC;
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max=
								MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max,BMS_Info[i].Sys_SOC);
				EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min=
								MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min,BMS_Info[i].Sys_SOC);		
			}
		}
		if (temp1_u16 != 0)
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver = temp1_u32 / temp1_u16;
		}
		else
		{
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Aver = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max = 0;
			EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min = 0;
		}
	}

	//电池电压 Max、Min、Average
	EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver = MINU16;
	EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max = MINU16;
	EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min = MAXU16;
	temp1_u16 = 0;
	temp1_u32 = 0;
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		//if(!PCS_Com_Err[i])//yuan
		if(PCS_Ready_Status[i])		
		{
			temp1_u16++;
			temp1_u32+=PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;			
            EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max=
                    MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max,PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
            EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min=
                    MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min,PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);                    
		}
	}
	if (temp1_u16 != 0)
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver = temp1_u32 / temp1_u16;
	}
	else
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver = 0;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max = 0;
		EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min = 0;
	}

	//PCS Active Power
	EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Aver = MINU16;
	EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Max = MINU16;
	EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Min = MAXU16;
	temp1_u16 = 0;
	temp1_u32 = 0;
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		if(!PCS_Com_Err[i])
		{
			temp1_u16++;
			temp1_u32+=(uint32_t)PCS_Info[i].PCS3x.Rmeasure.Active_Power;
			EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Max=
							MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Max,PCS_Info[i].PCS3x.Rmeasure.Active_Power);
			EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Min=
							MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Min,PCS_Info[i].PCS3x.Rmeasure.Active_Power);			
		}
	}
	if (temp1_u16 != 0)
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Aver = temp1_u32 / temp1_u16;
	}
	else
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Aver = 0;
		EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Max = 0;
		EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_Active_Power_Min = 0;
	}

	//电池板电压Max、Min、Average
	EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Aver = MINU16;
	EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Max = MINU16;
	EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Min = MAXU16;

	temp1_u16 = 0;
	temp1_u32 = 0;
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
	{
		if(!PV_Com_Err[i])
		{
			temp1_u16++;
			temp1_u32+=(uint32_t)PV_Info[i].PV3x.RunParameter.DC_Voltage1;
			EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Max=
							MAX2(EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Max,PV_Info[i].PV3x.RunParameter.DC_Voltage1);
			EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Min=
							MIN2(EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Min,PV_Info[i].PV3x.RunParameter.DC_Voltage1);			
		}
	}
	if(temp1_u16!=0)
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Aver=temp1_u32/temp1_u16;
	}
	else
	{
		EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Aver=0;
		EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Max=0;
		EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Min=0;
	}	
	
	//System_Run_Info.PCS_DC_Voltage=EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver;
	System_Run_Info.PCS_DC_Voltage=EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max;  
	System_Run_Info.PCS_AC_Voltage=EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage;
	System_Run_Info.PV_DC_Voltage=EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Aver;
	System_Run_Info.PV_AC_Voltage=0;
	temp1_u16=0;
	temp1_u32=0;
	for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
	{
		if(!PV_Com_Err[i])
		{
			temp1_u16++;
			temp1_u32+=(uint32_t)(PV_Info[i].PV3x.RunParameter.VA+
								  PV_Info[i].PV3x.RunParameter.VB+PV_Info[i].PV3x.RunParameter.VC)/3;
		}
		if (temp1_u16 != 0)
		{
			System_Run_Info.PV_AC_Voltage = temp1_u32 / temp1_u16;
		}
	}
}


//SOC状态
void SOC_Status(void)
{
	uint16_t SOC_Time=1;
	bool  temp = false;

	if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode == FM)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay_S         = (uint32_t)EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay*1000/EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay;
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay_S = (uint32_t)EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay*1000/EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay;

		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay_S          = (uint32_t)EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay*1000/EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay;
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay_S  = (uint32_t)EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay*1000/EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay;
	}
	else
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay_S         = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay*10/3;
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay_S = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay*10/3;

		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay_S          = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay*10/3;
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay_S  = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay*10/3;
	}

	if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable))
	{
        //G95 有一个大于就置位
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            if(PCS_Ready_Status[i])
            {
                if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2+EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis)
                {
                    SOC_Counter1[i]++;
                    if(SOC_Counter1[i]>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay_S*SOC_Time)
                    {
                        SOC_Counter1[i]=0;
                        Bat_Status.Vbat_G95=true;
                    }
                }  
                else
                {
                    SOC_Counter1[i]=0;
                }           
            }
        } 
        //G95全部小于才退出
        temp = true;
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            if(PCS_Ready_Status[i])
            {
            	if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2+EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis)
            	{
                    temp=false;
            	}
            }
        } 
        if(temp)
        {
            SOC_Counter[0]++;
            if(SOC_Counter[0]>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay_S*SOC_Time)
            {
                SOC_Counter[0]=0;            
                Bat_Status.Vbat_G95=false;
            }        
        }    
        else
        {
            SOC_Counter[0]=0;
        }

        //G90 有一个大于就置位
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            if(PCS_Ready_Status[i])
            {
                if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2)
                {
                    SOC_Counter2[i]++;
                    if(SOC_Counter2[i]>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay_S*SOC_Time)
                    {
                        SOC_Counter2[i]=0;
                        Bat_Status.Vbat_G90=true;
                    }
                }  
                else
                {
                    SOC_Counter2[i]=0;
                }           
            }
        } 
        //G90全部小于退出
        temp = true;
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            if(PCS_Ready_Status[i])        
            {
            	if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2)
            	{
                    temp=false;
            	}        	
            }
        } 
        if(temp)
        {
            SOC_Counter[1]++;
            if(SOC_Counter[1]>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay_S*SOC_Time)
            {
                SOC_Counter[1]=0;            
                Bat_Status.Vbat_G90=false;
            }
        }        	
        else
    	{
            SOC_Counter[1]=0;
    	}  

        //L80 有一个大于就设置为false
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            if(PCS_Ready_Status[i])        
            {
                if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1)
                {
                    SOC_Counter3[i]++;
                    if(SOC_Counter3[i]>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay_S*SOC_Time)
                    {
                        SOC_Counter3[i]=0;
                        Bat_Status.Vbat_L80=false;
                    }
                }  
                else
                {
                    SOC_Counter3[i]=0;
                }           
            }
        }

        //L80 全小于为true
        temp = true;
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            if(PCS_Ready_Status[i])        
            {
            	if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1)
            	{
                    temp=false;
            	}  
            }
        } 
        if(temp)
        {
            SOC_Counter[2]++;
            if(SOC_Counter[2]>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay_S*SOC_Time)
            {
                SOC_Counter[2]=0;            
                Bat_Status.Vbat_L80=true;
            }
        }   
        else
        {
            SOC_Counter[2]=0;
        }           

        //L40 有一个小于就置位
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            if(PCS_Ready_Status[i])        
            {
                if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1)
                {
                    SOC_Counter4[i]++;
                    if(SOC_Counter4[i]>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay_S*SOC_Time)
                    {
                        SOC_Counter4[i]=0;
                        Bat_Status.Vbat_L40=true;
                    }
                }  
                else
                {
                    SOC_Counter4[i]=0;
                }           
            }
        } 

		//L40退出，全部大于才退出
		temp = true;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if (PCS_Ready_Status[i])
			{
				if (PCS_Info[i].PCS3x.Rmeasure.DC_Voltage < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover)
				{
					temp = false;
				}
			}
		}
		if (temp)
		{
			SOC_Counter[3]++;
			if (SOC_Counter[3] > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay_S * SOC_Time)
			{
				SOC_Counter[3] = 0;
				Bat_Status.Vbat_L40 = false;
			}
		}
		else
		{
			SOC_Counter[3] = 0;
		}

		//L30 有一个小于就置位
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if (PCS_Ready_Status[i])
			{
				if (PCS_Info[i].PCS3x.Rmeasure.DC_Voltage < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2)
				{
					SOC_Counter5[i]++;
					if (SOC_Counter5[i] > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay_S * SOC_Time)
					{
						SOC_Counter5[i] = 0;
						Bat_Status.Vbat_L30 = true;
					}
				}
				else
				{
					SOC_Counter5[i] = 0;
				}
			}
		}

		//L30退出，全部大于才退出
		temp = true;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if (PCS_Ready_Status[i])
			{
				if (PCS_Info[i].PCS3x.Rmeasure.DC_Voltage < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover)
				{
					temp = false;
				}
			}
		}
		if (temp)
		{
			SOC_Counter[4]++;
			if (SOC_Counter[4] > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay_S * SOC_Time)
			{
				SOC_Counter[4] = 0;
				Bat_Status.Vbat_L30 = false;
			}
		}
		else
		{
			SOC_Counter[4] = 0;
		}
	}
	else
	{
		Bat_Status.Vbat_G95 = false;
		Bat_Status.Vbat_G90 = false;
		Bat_Status.Vbat_L80 = true;  //反向
		Bat_Status.Vbat_L40 = false;
		Bat_Status.Vbat_L30 = false;
		//如果没有配置BMS，则禁止Vbat控制
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable = DISABLE;
	}

//-----------------------------------------------------------------------------------------------------------------------
	//SOC_Status
	//G95
	if (FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
	{
		//G95 有一个大于就置位
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2 + EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis)
				{
					SOC_Counter1[i]++;
					if (SOC_Counter1[i] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay_S * SOC_Time)
					{
						SOC_Counter1[i] = 0;
						Bat_Status.SOC_G95 = true;
					}
				}
				else
				{
					SOC_Counter1[i] = 0;
				}
			}
		}
		//G95全部小于才退出
		temp = true;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2 + EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis)
				{
					temp = false;
				}
			}
		}
		if (temp)
		{
			SOC_Counter[0]++;
			if (SOC_Counter[0] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay_S * SOC_Time)
			{
				SOC_Counter[0] = 0;
				Bat_Status.SOC_G95 = false;
			}
		}
		else
		{
			SOC_Counter[0] = 0;
		}

		//G90 有一个大于就置位
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2)
				{
					SOC_Counter2[i]++;
					if (SOC_Counter2[i] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay_S * SOC_Time)
					{
						SOC_Counter2[i] = 0;
						Bat_Status.SOC_G90 = true;
					}
				}
				else
				{
					SOC_Counter2[i] = 0;
				}
			}
		}
		//G90全部小于退出
		temp = true;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2)
				{
					temp = false;
				}
			}
		}
		if (temp)
		{
			SOC_Counter[1]++;
			if (SOC_Counter[1] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay_S * SOC_Time)
			{
				SOC_Counter[1] = 0;
				Bat_Status.SOC_G90 = false;
			}
		}
		else
		{
			SOC_Counter[1] = 0;
		}

		//L80 有一个大于就设置为false
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1)
				{
					SOC_Counter3[i]++;
					if (SOC_Counter3[i] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay_S * SOC_Time)
					{
						SOC_Counter3[i] = 0;
						Bat_Status.SOC_L80 = false;
					}
				}
				else
				{
					SOC_Counter3[i] = 0;
				}
			}
		}

		//L80 全小于为true
		temp = true;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1)
				{
					temp = false;
				}
			}
		}
		if (temp)
		{
			SOC_Counter[2]++;
			if (SOC_Counter[2] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay_S * SOC_Time)
			{
				SOC_Counter[2] = 0;
				Bat_Status.SOC_L80 = true;
			}
		}
		else
		{
			SOC_Counter[2] = 0;
		}

		//L40 有一个小于就置位
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC < EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1)
				{
					SOC_Counter4[i]++;
					if (SOC_Counter4[i] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay_S * SOC_Time)
					{
						SOC_Counter4[i] = 0;
						Bat_Status.SOC_L40 = true;
					}
				}
				else
				{
					SOC_Counter4[i] = 0;
				}
			}
		}

		//L40退出，全部大于才退出
		temp = true;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC < EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover)
				{
					temp = false;
				}
			}
		}
		if (temp)
		{
			SOC_Counter[3]++;
			if (SOC_Counter[3] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay_S * SOC_Time)
			{
				SOC_Counter[3] = 0;
				Bat_Status.SOC_L40 = false;
			}
		}
		else
		{
			SOC_Counter[3] = 0;
		}

		//L30 有一个小于就置位
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC < EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2)
				{
					SOC_Counter5[i]++;
					if (SOC_Counter5[i] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay_S * SOC_Time)
					{
						SOC_Counter5[i] = 0;
						Bat_Status.SOC_L30 = true;
					}
				}
				else
				{
					SOC_Counter5[i] = 0;
				}
			}
		}

		//L30退出，全部大于才退出
		temp = true;
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			if (PCS_Ready_Status[i] && (!BMS_Com_Err[i]) && BMS_Com_OK[i])
			{
				if (BMS_Info[i].Sys_SOC < EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover)
				{
					temp = false;
				}
			}
		}
		if (temp)
		{
			SOC_Counter[4]++;
			if (SOC_Counter[4] > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay_S * SOC_Time)
			{
				SOC_Counter[4] = 0;
				Bat_Status.SOC_L30 = false;
			}
		}
		else
		{
			SOC_Counter[4] = 0;
		}
	}
	else
	{
		Bat_Status.SOC_G95 = false;
		Bat_Status.SOC_G90 = false;
		Bat_Status.SOC_L80 = true;  //反向
		Bat_Status.SOC_L40 = false;
		Bat_Status.SOC_L30 = false;
		//如果没有配置BMS，则禁止SOC控制
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable = DISABLE;
	}
}



#if 0

//0-充电 1-放电
//float power[8];
float power[PCS_NUM];
void Power_Balancing(uint8_t PCS_Num ,int16_t P_Aver,int16_t P_Max,uint8_t Status)
{
    uint8_t i,k,j=0;    
    static float p_total[PCS_NUM],soc_total[PCS_NUM],soc_t=0.0;
    static float soc_aver[PCS_NUM]={0};
    static float p_aver[PCS_NUM]={0};
    static float  kp[PCS_NUM] ={0};
    static uint8_t ok[PCS_NUM]={0};
    static uint8_t loop=0;
    static uint8_t online_num=0;
	
	soc_t=0;online_num=0;	
	//for(i=0;i<8;i++)
	for(i=0;i<PCS_NUM;i++)
	{
		power[i] = 0.0;
		ok[i] = 0;
		p_total[i] = 0.0;
		soc_total[i] = 0.0;
		soc_aver[i] = 0.0;
		p_aver[i] = 0.0;
		kp[i] = Balancing_Kp / (float) 10.0;
	}
	for (i = 0; i < PCS_Num; i++)
	{
		if ((PCS_Ready_Status[i]) && (!BMS_Com_Err[i]))
		{
			soc_t += BMS_Info[i].Sys_SOC;
			online_num++;
		}
		else
		{
			ok[i] = 1; //故障的pcs ,bms 不参与分配
		}
	}
	if (online_num != 0)
	{
		soc_aver[0] = soc_t / (float) online_num;
	}
	p_aver[0] = P_Aver;
	loop = 0;
	j = 0;
	//for (k = 0; k < 8; k++)
	for(k=0;k<PCS_NUM;k++)
	{
		for (i = 0; i < PCS_Num; i++)
		{
			if (!ok[i])
			{
				if (Status)
				{
					power[i] = power[i] + p_aver[j] - (soc_aver[j] - BMS_Info[i].Sys_SOC) * (float) kp[j];  //放电
				}
				else
				{
					power[i] = power[i] + p_aver[j] + (soc_aver[j] - BMS_Info[i].Sys_SOC) * (float) kp[j];  //充电
				}
				if (power[i] >= P_Max)
				{
					p_total[j] += (power[i] - P_Max);
					power[i] = P_Max;
					ok[i] = 1;
				}
				else if (power[i] <= 0)
				{
					p_total[j] += (power[i] - 0);
					power[i] = 0;
					ok[i] = 1;
				}
				else
				{
					loop++;
					ok[i] = 0;
					soc_total[j] += BMS_Info[i].Sys_SOC;
				}
			}
		}
		if (loop == PCS_Num)
		{
			for (i = 0; i < PCS_Num; i++)
			{
				ok[i] = 1;
			}
			//p_total[j] = power[0] + power[1] + power[2] + power[3] + power[4] + power[5] + power[6] + power[7];
			break;
		}
		else if (loop != 0)
		{
			if (p_total[j] == 0)
			{
				//p_total[j] = power[0] + power[1] + power[2] + power[3] + power[4] + power[5] + power[6] + power[7];
				break;
			}
			j++;
			p_aver[j] = p_total[j - 1] / (float) loop;
			soc_aver[j] = soc_total[j - 1] / (float) loop;
			loop = 0;
		}
		else
		{
			break;
		}
	}
}

#else

//0-充电 1-放电
//float power[8];
float power[PCS_NUM];
void Power_Balancing(uint8_t PCS_Num ,int16_t P_Aver,int16_t P_Max,uint8_t Status)
{
    uint8_t i,j;

    static uint8_t fault[PCS_NUM]={0};
    static float   soc_t=0.0,soc_aver=0.0,p_left=0.0,kp=0.0;
    static uint8_t pmax_num=0,pmin_num=0,pmid_num=0,online_num=0;


	soc_t      = 0.0;
    soc_aver   = 0.0;
    p_left     = 0.0;
    kp         = (float)Balancing_Kp / 10.0;

    pmax_num   = 0 ;
	pmin_num   = 0;
	pmid_num   = 0;
	online_num = 0;

    for(i=0;i<PCS_NUM;i++)
	{
		power[i] = 0.0;
		fault[i] = 0;
	}

	for (i = 0; i < PCS_Num; i++)
	{
		if ((PCS_Ready_Status[i]) && (!BMS_Com_Err[i]))
		{
			soc_t += BMS_Info[i].Sys_SOC;
			online_num++;
		}
		else
		{
			fault[i] = 1; //故障的pcs ,bms 不参与分配
		}
	}
	if (online_num != 0)
	{
		soc_aver = soc_t / (float)online_num;
	}


	for (i = 0; i < PCS_Num; i++)
	{
		if (Status)
		{
			power[i] = P_Aver - (soc_aver - (float)BMS_Info[i].Sys_SOC) * (float) kp;  //放电
		}
		else
		{
			power[i] = P_Aver + (soc_aver - (float)BMS_Info[i].Sys_SOC) * (float) kp;  //充电
		}
	}

	for(j=0;j< PCS_Num; j++)
	{
	    p_left     = 0.0;
	    pmax_num   = 0 ;
		pmin_num   = 0;
		pmid_num   = 0;

		for (i = 0; i < PCS_Num; i++)
		{
			if (!fault[i])
			{
				if (power[i] >= P_Max)
				{
					p_left += (power[i] - P_Max);
					power[i] = P_Max ;
					pmax_num++;
				}
				else if (power[i] <= 0)
				{
					p_left += (power[i] - 0);
					power[i] = 0 ;
					pmin_num++;
				}
				else
				{
					pmid_num++;
				}
			}
		}

		if(p_left>=0)
		{
			for (i = 0; i < PCS_Num; i++)
			{
				if ((!(power[i]>=P_Max))&&(!fault[i]))
				{
					power[i] += (p_left/(float)(pmin_num+pmid_num));
				}
			}
		}
		else
		{
			for (i = 0; i < PCS_Num; i++)
			{
				if((!(power[i]<=0))&&(!fault[i]))
				{
					power[i] += (p_left/(float)(pmax_num+pmid_num)) ;
				}
			}
		}
		//printf("p_left %f\n", p_left );
	}
}

#endif


static void Grid_Power_Control_Proc(int32_t Power_Value)
{
	int32_t Charge_Power_Max, Discharge_Power_Max = 0;
	uint16_t temp_u16;
	int16_t temp_s16;
	int32_t balancing_charge_power = 0;
	int32_t balancing_discharge_power = 0;
	int32_t charge_power = 0;
	int32_t dis_charge_power = 0;

	//充放电限定值
	if(ENABLE==System_Info.Config.Power_Distribution_Enable)
	{
		if (DC_BUS_SYS == 0) //交流母线
		{
			if(System_Info.Config.Meter_Position_Enable[1] == ENABLE)
			{
				Charge_Power_Max   = (int32_t)System_Info.Config.System_Power_Set * 11 - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power + (int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power;
			}
			else
			{
				Charge_Power_Max   = (int32_t)System_Info.Config.System_Power_Set * 11 ;
			}

			Discharge_Power_Max =(int32_t)System_Info.Config.System_Power_Set * 11 - (int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power;
		}
		else //直流母线
		{
			if(System_Info.Config.Meter_Position_Enable[1] == ENABLE)
			{
				Charge_Power_Max    = (int32_t)System_Info.Config.System_Power_Set * 11 - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power;
			}
			else
			{
				Charge_Power_Max    = (int32_t)System_Info.Config.System_Power_Set * 11 ;
			}

			Discharge_Power_Max = (int32_t)System_Info.Config.System_Power_Set * 11;
		}
	}
	else
	{
		Charge_Power_Max    = (int32_t)System_Info.Config.System_Power_Set * 11;
		Discharge_Power_Max = (int32_t)System_Info.Config.System_Power_Set * 11;
	}

	if ((Charge_Power_Max > 0) && (Power_Value < 0))
	{
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num > 0)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = Charge_Power_Max / EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num;
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power = abs(Power_Value) / EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num;
		}
		else
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = 0;
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power = 0;
		}
	}
	else
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power = 0;
	}

	if ((Discharge_Power_Max > 0) && (Power_Value > 0))
	{
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num > 0)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = Discharge_Power_Max / EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num;
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power = abs(Power_Value) / EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num;
		}
		else
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = 0;
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power = 0;
		}
	}
	else
	{
		EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power = 0;
	}

	if (System_Info.Config.PCS_Type == PCS_Power_SC30_50)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			if (PCS_Info[i].PCS3x.Rmeasure.Type_Code == 0x0b08) //sc30
			{
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max > 330)
				{
					EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = 330;
				}
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max > 330)
				{
					EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = 330;
				}
			}
			else if (PCS_Info[i].PCS3x.Rmeasure.Type_Code == 0x0b04) //sc50
			{
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max > 550)
				{
					EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = 550;
				}
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max > 550)
				{
					EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = 550;
				}
			}
		}
	}
	else if (System_Info.Config.PCS_Type == PCS_Power_SC100)
	{
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max > 1100)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = 1100;
		}
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max > 1100)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = 1100;
		}
	}
	else if (System_Info.Config.PCS_Type == PCS_Power_SC250)
	{
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max > 2750)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = 2750;
		}
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max > 2750)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = 2750;
		}
	}
	else if ((System_Info.Config.PCS_Type == PCS_Power_SC500)||(System_Info.Config.PCS_Type == PCS_Power_SC500TL))
	{
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max > 5500)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = 5500;
		}
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max > 5500)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = 5500;
		}
	}
	else if (System_Info.Config.PCS_Type == PCS_Power_SC1000)
	{
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max > 11000)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max = 11000;
		}
		if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max > 11000)
		{
			EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max = 11000;
		}
	}
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		if(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage!=0)
		{
			//分配功率
			if(Power_Value<0)
			{//充电
                if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable == ENABLE)
                {                
                    charge_power = 
                    Pid_VoltCtrl(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1+3, PCS_Info[i].PCS3x.Rmeasure.DC_Voltage, \
                    EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power, 0,i);     
                    //恒压
    				PCS_Ctrl_Data[i].Const_V_Mode_V    =  EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max;
    				PCS_Ctrl_Data[i].Const_V_Mode_I    =  charge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
    				//恒流，恒直流交流功率
    				PCS_Ctrl_Data[i].Const_I_Mode_I    = -(charge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
    				PCS_Ctrl_Data[i].Const_P_DC_Mode_P = -charge_power;
    				PCS_Ctrl_Data[i].Const_P_AC_Mode_P = -charge_power;

    				//printf("Const_P_AC_Mode_P %d\n", PCS_Ctrl_Data[0].Const_P_AC_Mode_P );

				}
                else if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable == ENABLE)
                {
                    if(System_Info.Config.Balancing_Enable == ENABLE)
                    {
                        //分配功率
                        Power_Balancing(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number ,EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power,
                                        EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max,0);
                        balancing_charge_power = power[i];                        
                        //恒压
                        PCS_Ctrl_Data[i].Const_V_Mode_V    =  EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max;
                        PCS_Ctrl_Data[i].Const_V_Mode_I    =  balancing_charge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                        //恒流，恒直流交流功率
                        PCS_Ctrl_Data[i].Const_I_Mode_I    = -(balancing_charge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
                        PCS_Ctrl_Data[i].Const_P_DC_Mode_P = -balancing_charge_power;
                        PCS_Ctrl_Data[i].Const_P_AC_Mode_P = -balancing_charge_power;
                    }
                    else
                    {
                        //恒压
                        PCS_Ctrl_Data[i].Const_V_Mode_V    =  EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max;
                        PCS_Ctrl_Data[i].Const_V_Mode_I    =  EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                        //恒流，恒直流交流功率
                        PCS_Ctrl_Data[i].Const_I_Mode_I    = -(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
                        PCS_Ctrl_Data[i].Const_P_DC_Mode_P = -EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power;
                        PCS_Ctrl_Data[i].Const_P_AC_Mode_P = -EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power;
                    }
                    if(BMS_Info[i].Sys_SOC > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1)
                    {
                        PCS_Ctrl_Data[i].Const_V_Mode_V    =  EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max;
                        PCS_Ctrl_Data[i].Const_V_Mode_I    =  0;
                        //恒流，恒直流交流功率
                        PCS_Ctrl_Data[i].Const_I_Mode_I    =  0;
                        PCS_Ctrl_Data[i].Const_P_DC_Mode_P =  0;
                        PCS_Ctrl_Data[i].Const_P_AC_Mode_P =  0;
                    }
                }				
			}
			else
			{//放电
                if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable == ENABLE)
                {
                    dis_charge_power = 
                    -Pid_VoltCtrl(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1-3,PCS_Info[i].PCS3x.Rmeasure.DC_Voltage, \
                    0,-EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power,i);                    
                    //恒压
    				PCS_Ctrl_Data[i].Const_V_Mode_V    = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min;
    				PCS_Ctrl_Data[i].Const_V_Mode_I    = dis_charge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                    //恒流，恒直流，交流功率
    				PCS_Ctrl_Data[i].Const_I_Mode_I    = dis_charge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
    				PCS_Ctrl_Data[i].Const_P_DC_Mode_P = dis_charge_power;
    				PCS_Ctrl_Data[i].Const_P_AC_Mode_P = dis_charge_power;
                }                
                else if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable == ENABLE)
                {
                    if(System_Info.Config.Balancing_Enable == ENABLE)
                    {
                        //分配功率
                        Power_Balancing(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number ,EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power,
                                        EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max,1);
                        balancing_discharge_power = power[i];         
                        PCS_Ctrl_Data[i].Const_V_Mode_V    = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min;
                        PCS_Ctrl_Data[i].Const_V_Mode_I    = balancing_discharge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                        //恒流，恒直流，交流功率
                        PCS_Ctrl_Data[i].Const_I_Mode_I    = balancing_discharge_power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                        PCS_Ctrl_Data[i].Const_P_DC_Mode_P = balancing_discharge_power;
                        PCS_Ctrl_Data[i].Const_P_AC_Mode_P = balancing_discharge_power;
                    }
                    else
                    {
                        //恒压
                        PCS_Ctrl_Data[i].Const_V_Mode_V    = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min;
                        PCS_Ctrl_Data[i].Const_V_Mode_I    = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                        //恒流，恒直流，交流功率
                        PCS_Ctrl_Data[i].Const_I_Mode_I    = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                        PCS_Ctrl_Data[i].Const_P_DC_Mode_P = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power;
                        PCS_Ctrl_Data[i].Const_P_AC_Mode_P = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power;
                    }
                    if(BMS_Info[i].Sys_SOC < EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1)
                    {
                        PCS_Ctrl_Data[i].Const_V_Mode_V    =  EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min;
                        PCS_Ctrl_Data[i].Const_V_Mode_I    =  0;
                        //恒流，恒直流交流功率
                        PCS_Ctrl_Data[i].Const_I_Mode_I    =  0;
                        PCS_Ctrl_Data[i].Const_P_DC_Mode_P =  0;
                        PCS_Ctrl_Data[i].Const_P_AC_Mode_P =  0;
                    }                    
				}
			}
			
			//限幅
			if(Power_Value<0)//充电
			{
				if(DC_BUS_SYS == 1) //直流母线
				{
					if(System_Info.Config.DC_Bus_Charge_Permit == 1) //直流母线允许从电网充电
					{                                 
						temp_u16=EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
						if(temp_u16<PCS_Ctrl_Data[i].Const_V_Mode_I)
						{
							PCS_Ctrl_Data[i].Const_V_Mode_I=temp_u16;
						}				
                  
						temp_s16 = -(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
						if(temp_s16 > PCS_Ctrl_Data[i].Const_I_Mode_I)
						{
							PCS_Ctrl_Data[i].Const_I_Mode_I=temp_s16;
						}
                    
                        temp_s16 = -(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max);
                        if(temp_s16 > PCS_Ctrl_Data[i].Const_P_DC_Mode_P)
                        {
                            PCS_Ctrl_Data[i].Const_P_DC_Mode_P=temp_s16;
                        }
                    
                        temp_s16 = -(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max);
                        if(temp_s16 > PCS_Ctrl_Data[i].Const_P_AC_Mode_P)
                        {
                            PCS_Ctrl_Data[i].Const_P_AC_Mode_P=temp_s16;
                        }                 					
					}
					else
					{
						PCS_Ctrl_Data[i].Const_V_Mode_I=0;
						PCS_Ctrl_Data[i].Const_I_Mode_I=0;
						PCS_Ctrl_Data[i].Const_P_DC_Mode_P=0;
						PCS_Ctrl_Data[i].Const_P_AC_Mode_P=0;
					}
				} 
				else //交流母线
				{
                    temp_u16=EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                    if(temp_u16<PCS_Ctrl_Data[i].Const_V_Mode_I)
                    {
                        PCS_Ctrl_Data[i].Const_V_Mode_I=temp_u16;
                    }
                    temp_s16 = -(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage);
                    if(temp_s16 > PCS_Ctrl_Data[i].Const_I_Mode_I)
                    {
                        PCS_Ctrl_Data[i].Const_I_Mode_I=temp_s16;
                    }
                    temp_s16 = -(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max);
                    if(temp_s16 > PCS_Ctrl_Data[i].Const_P_DC_Mode_P)
                    {
                        PCS_Ctrl_Data[i].Const_P_DC_Mode_P=temp_s16;
                    }
                    temp_s16 = -(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_charge_Power_Max);
                    if(temp_s16 > PCS_Ctrl_Data[i].Const_P_AC_Mode_P)
                    {
                        PCS_Ctrl_Data[i].Const_P_AC_Mode_P=temp_s16;
                    }
				}
			}
			else//放电
            {
                temp_u16 = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                if(temp_u16<PCS_Ctrl_Data[i].Const_V_Mode_I)
                {
                    PCS_Ctrl_Data[i].Const_V_Mode_I=temp_u16;
                }
         
                temp_s16 = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max*10000/PCS_Info[i].PCS3x.Rmeasure.DC_Voltage;
                if(temp_s16<PCS_Ctrl_Data[i].Const_I_Mode_I)
                {
                    PCS_Ctrl_Data[i].Const_I_Mode_I=temp_s16;
                }
        
                temp_s16 = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max;
                if(temp_s16<PCS_Ctrl_Data[i].Const_P_DC_Mode_P)
                {
                    PCS_Ctrl_Data[i].Const_P_DC_Mode_P=temp_s16;
                }
         
                temp_s16 = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_discharge_Power_Max;
                if(temp_s16<PCS_Ctrl_Data[i].Const_P_AC_Mode_P)
                {
                    PCS_Ctrl_Data[i].Const_P_AC_Mode_P=temp_s16;
                }
			}
		}
		else
		{
			if (Power_Value < 0)
			{
				PCS_Ctrl_Data[i].Const_V_Mode_V = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max;
			}
			else
			{
				PCS_Ctrl_Data[i].Const_V_Mode_V = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min;
			}
			PCS_Ctrl_Data[i].Const_V_Mode_I = 0;
			PCS_Ctrl_Data[i].Const_I_Mode_I = 0;
			PCS_Ctrl_Data[i].Const_P_DC_Mode_P = 0;
			PCS_Ctrl_Data[i].Const_P_AC_Mode_P = 0;
		}
	}
}

int32_t EMS_Run_Power_Active = 0;
static  int32_t EMS_Run_Power_Active_Last = 0;

//SOC控制
void SOC_Control(void)
{
    static int32_t EMS_Run_Power_Reactive = 0;
    static int16_t EMS_Run_Qvar_Ratio = 0;
    static int32_t temp_power=0;
    
    static uint16_t back_delay_time;
    static float ratio,ratio_limit;
    static float ratio_limit_last=1000.0;
    static float ratio_max;

	//无功功率调节
	if (ISLAND_RUNNING || GRID_RUNNING)
	{
		if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == QP) //无功功率
		{
			if (EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set      > (int16_t)((float)System_Info.Config.System_Power_Set * 0.3))
			{
				EMS_Run_Power_Reactive = System_Info.Config.System_Power_Set * 3; // x10
			}
			else if (EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set < (-(int16_t)(float)System_Info.Config.System_Power_Set * 0.3))
			{
				EMS_Run_Power_Reactive = -System_Info.Config.System_Power_Set * 3; // x10
			}
			else
			{
				EMS_Run_Power_Reactive = EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set*10; // x10
			}

			if (System_Info.Config.PCS_Type == PCS_Power_SC30_50)
			{
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num > 0)
				{
					EMS_Run_Qvar_Ratio = (int32_t) EMS_Run_Power_Reactive * 1000 / (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num * 50 * 3); // 根据实际在线台数，为总功率的30%
				}
				else
				{
					EMS_Run_Power_Reactive = 0;
					EMS_Run_Qvar_Ratio = 0;
				}
			}
			else if (System_Info.Config.PCS_Type == PCS_Power_SC100)
			{
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num > 0)
				{
					EMS_Run_Qvar_Ratio = (int32_t) EMS_Run_Power_Reactive * 1000 / (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num * 100 * 3); // 根据实际在线台数，为总功率的30%
				}
				else
				{
					EMS_Run_Power_Reactive = 0;
					EMS_Run_Qvar_Ratio = 0;
				}
			}
			else if (System_Info.Config.PCS_Type == PCS_Power_SC250)
			{
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num > 0)
				{
					EMS_Run_Qvar_Ratio = (int32_t) EMS_Run_Power_Reactive * 1000 / (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num * 250 * 3); // 根据实际在线台数，为总功率的30%
				}
				else
				{
					EMS_Run_Power_Reactive = 0;
					EMS_Run_Qvar_Ratio = 0;
				}
			}
			else if ((System_Info.Config.PCS_Type == PCS_Power_SC500)||(System_Info.Config.PCS_Type == PCS_Power_SC500TL))
			{
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num > 0)
				{
					EMS_Run_Qvar_Ratio = (int32_t) EMS_Run_Power_Reactive * 1000 / (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num * 500 * 3); // 根据实际在线台数，为总功率的30%
				}
				else
				{
					EMS_Run_Power_Reactive = 0;
					EMS_Run_Qvar_Ratio = 0;
				}
			}
			else if (System_Info.Config.PCS_Type == PCS_Power_SC1000)
			{
				if (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num > 0)
				{
					EMS_Run_Qvar_Ratio = (int32_t) EMS_Run_Power_Reactive * 1000 / (EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num * 1000 * 3); // 根据实际在线台数，为总功率的30%
				}
				else
				{
					EMS_Run_Power_Reactive = 0;
					EMS_Run_Qvar_Ratio = 0;
				}
			}
			if (EMS_Run_Qvar_Ratio > 1000)
			{
				EMS_Run_Qvar_Ratio = 1000;
			}
			if (EMS_Run_Qvar_Ratio < -1000)
			{
				EMS_Run_Qvar_Ratio = -1000;
			}
			for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
			{
				if (PCS_Ctrl_Data[i].Qvar_Power_Ratio != EMS_Run_Qvar_Ratio)
				{
					PCS_Ctrl_Data[i].Qvar_Power_Ratio = EMS_Run_Qvar_Ratio;
					PCS_Ctrl_Data[i].Power_Factor = P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft;
					Send_Data_To_PCS_0x10_02[i] = true;
				}
			}
		}
		else if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == PF) //功率因数
		{
			for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
			{
				if (PCS_Ctrl_Data[i].Power_Factor != EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor)
				{
					PCS_Ctrl_Data[i].Power_Factor = EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
					PCS_Ctrl_Data[i].Qvar_Power_Ratio = 0;
					Send_Data_To_PCS_0x10_02[i] = true;
				}
			}
		}
		else if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == SHUT_OFF) //关闭无功调节
		{
			for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
			{
				PCS_Ctrl_Data[i].Qvar_Power_Ratio = 0;
				PCS_Ctrl_Data[i].Power_Factor = P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft;
				if (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode != PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode)
				{
					PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode = EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
					Send_Data_To_PCS_0x10_02[i] = true;
				}
			}
		}
	}

	//孤岛运行SOC控制
	if(ISLAND_RUNNING)
	{
		if(DC_BUS_SYS == 0)   //交流母线
		{
            if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
            {
    			if(Bat_Status.SOC_G95)
    			{
    				ratio = 0; //应该关机最好
    			}
    			else if(Bat_Status.SOC_G90)
    			{
    				ratio = 1;
    			}    	
    			else //PID控制在L80
    			{ 
                    if(-EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power>abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10)) // x10
                    {
                        ratio_max = (float)(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)*1000.0
                        / EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power;
                    }
                    else
                    {
                        ratio_max=1000.0;
                    }
                    //功率
                    ratio_limit = 
                    -Pid_PowerCtrl(-((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))),
                                                            EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power,
                                                            0,-ratio_limit_last,PCS_NUM);    // x10 61
                        
                    if(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power > 
                            -((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))*0.95))
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
                    //SOC
                    ratio = 
                    Pid_VoltCtrl(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1+3, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max,
                                                                              ratio_limit,0,1);     
      			}   
                //单台光伏出力计算
                for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
                {
                    if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num>0)
                    {
                        PV_Ctrl_Data[i].PowerLimit_Set = ratio;                                                           
                        PV_Ctrl_Data[i].PowerLimit_Set = LIMIT(1,PV_Ctrl_Data[i].PowerLimit_Set,1000);
                    }
                    else
                    {
                        PV_Ctrl_Data[i].PowerLimit_Set=0;
                    }
                }           
			}
            else
            {
    			if(Bat_Status.Vbat_G95)
    			{
    				ratio = 0; //应该关机最好
    			}
    			else if(Bat_Status.Vbat_G90)
    			{
    				ratio = 1;
    			}    	
    			else //PID控制在L80
    			{ 
                    if(-EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power>abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))
                    {
                        ratio_max = (float)(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)*1000.0
                        / EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power;
                    }
                    else
                    {
                        ratio_max=1000.0;
                    }                    
                    //功率
                    ratio_limit = 
                    -Pid_PowerCtrl(-((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))),
                                                            EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power,
                                                            0,-ratio_limit_last,PCS_NUM); // 61
                        
                    if(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power > 
                            -((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))*0.95))
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
                    //电压
                    ratio = 
                    Pid_VoltCtrl(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1+3, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max,
                                                                              ratio_limit,0,1);     
      			}   
                //单台光伏出力计算
                for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
                {
                    if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num>0)
                    {
                        PV_Ctrl_Data[i].PowerLimit_Set = ratio;                                                           
                        PV_Ctrl_Data[i].PowerLimit_Set = LIMIT(1,PV_Ctrl_Data[i].PowerLimit_Set,1000);
                    }
                    else
                    {
                        PV_Ctrl_Data[i].PowerLimit_Set=0;
                    }
                }           
			}			
		}
	}

	//并网运行SOC控制
	if(GRID_RUNNING)
	{
		if(DC_BUS_SYS != 1) //交流母线
		{ 
			if(GRID_TYPE==0) //非并网
			{
				if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
				{
					if(Bat_Status.SOC_G95)
					{
						ratio = 0; //应该关机最好
					}
					else if(Bat_Status.SOC_G90)
					{
						ratio = 1;
					}
					else //PID控制在L80
					{
						if(-EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power>abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10)) // x10
						{
							ratio_max = (float)(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)*1000.0
							/ EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power;
						}
						else
						{
							ratio_max=1000.0;
						}
						//功率
						ratio_limit =
						-Pid_PowerCtrl(-((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))),
																EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power,
																0,-ratio_limit_last,PCS_NUM);    // x10 61

						if(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power >
								-((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))*0.95))
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
						//SOC
						ratio =
						Pid_VoltCtrl(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1+3, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max,
																				  ratio_limit,0,1);
					}
					//单台光伏出力计算
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
					{
						if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num>0)
						{
							PV_Ctrl_Data[i].PowerLimit_Set = ratio;
							PV_Ctrl_Data[i].PowerLimit_Set = LIMIT(1,PV_Ctrl_Data[i].PowerLimit_Set,1000);
						}
						else
						{
							PV_Ctrl_Data[i].PowerLimit_Set=0;
						}
					}
				}
				else
				{
					if(Bat_Status.Vbat_G95)
					{
						ratio = 0; //应该关机最好
					}
					else if(Bat_Status.Vbat_G90)
					{
						ratio = 1;
					}
					else //PID控制在L80
					{
						if(-EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power>abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))
						{
							ratio_max = (float)(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)*1000.0
							/ EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power;
						}
						else
						{
							ratio_max=1000.0;
						}
						//功率
						ratio_limit =
						-Pid_PowerCtrl(-((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))),
																EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power,
																0,-ratio_limit_last,PCS_NUM); // 61

						if(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power >
								-((float)(abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Mgrid_Run_Power*10))*0.95))
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
						//电压
						ratio =
						Pid_VoltCtrl(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1+3, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max,
																				  ratio_limit,0,1);
					}
					//单台光伏出力计算
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
					{
						if(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num>0)
						{
							PV_Ctrl_Data[i].PowerLimit_Set = ratio;
							PV_Ctrl_Data[i].PowerLimit_Set = LIMIT(1,PV_Ctrl_Data[i].PowerLimit_Set,1000);
						}
						else
						{
							PV_Ctrl_Data[i].PowerLimit_Set=0;
						}
					}
				}
			}
			else // 并网
			{
				for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
				{
					PV_Ctrl_Data[i].PowerLimit_Set=1000;
				}
			}
		}
		if (GRID_TYPE == 1) // 并网
		{
			if (DC_BUS_SYS == 0) // 交流母线
			{
				//充电限定值
			}
			else //共直流母线
			{
				//充电限定值
			}
			if (TIME_IN_PERIOD)
			{
				//选择不同的功率控制方式
				switch (Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode)
				{
					case CONST_POWER:
						temp_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Power*10;
						break;

                    case FOLLOW_LOAD_POWER:                    
                        Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Power = (int16_t)(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power/10);
                        temp_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Power*10;
                        break;

                    case PCC_POWER_CTRL:   
                        temp_power = (int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Power*10
                         -((int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power - EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power) ;
                        if(DC_BUS_SYS==0) //交流母线
                        {                  
                            if(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power>abs((int32_t)Max_anti_reflux_power*10))
                            {
                                ratio_max = (float)(EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)*1000.0
                                / EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power;
                            }
                            else
                            {
                                ratio_max=1000.0;
                            }
                            //功率
                            ratio_limit = 
                            Pid_PowerCtrl(abs((int32_t)Max_anti_reflux_power*10),EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power, ratio_limit_last,0,PCS_NUM);  // 61
                                
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
                        break;
                        
                    case ANTI_REFLUX:                 
                        temp_power = -abs((int32_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Power*10); //设置充电功率
                        if(DC_BUS_SYS==0) //交流母线
                        {                  
                            if(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power>abs((int32_t)Max_anti_reflux_power*10))
                            {
                                ratio_max = (float)(EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power+abs((int32_t)Max_anti_reflux_power*10)
                                +abs(EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power))*1000.0
                                / EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power;
                            }
                            else
                            {
                                ratio_max=1000.0;
                            }
                            //功率
                            ratio_limit = 
                            Pid_PowerCtrl(abs((int32_t)Max_anti_reflux_power*10),EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power, ratio_limit_last,0,PCS_NUM); // 61
                                
                            if(EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power < -50)
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
                        break;
                    case PEAK_LOAD_SHIFTING:
                    	temp_power = Peak_Valley_Shift();
                    	break;
                    case ACSIDE_PVLIMIT:
                    	temp_power = ACSide_PVLimit();
                    	break;
                    case RAMP_RATE_CONTROL:
                    	//temp_power = Ramp_rate_Control();
                    	temp_power = Ramp_Rate_Power;
                    	break;
                    case FM:
                    	temp_power = FM_Control();
                    	break;
                }
                
				if(System_Info.Config.System_Power_Set*11<abs(temp_power))
				{
					if (temp_power > 0)
					{
						EMS_Run_Power_Active = (int32_t)System_Info.Config.System_Power_Set * 11;
					}
					else
					{
						EMS_Run_Power_Active = -(int32_t)System_Info.Config.System_Power_Set * 11;
					}
				}
				else
				{
					EMS_Run_Power_Active = temp_power;					
				}
				//printf("EMS_Run_Power_Active %d\n", EMS_Run_Power_Active);
                //根据电池电压或者SOC功率控制
    			if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
    			{
    				//放电且没放空，充电且未充满    		
					Grid_Power_Control_Proc(EMS_Run_Power_Active);                                				    				
    			}
    			else
    			{
                   if(EMS_Run_Power_Active<0) // 控制在L40和L80之间
                    {
                        if(EMS_Run_Power_Active_Last>0)
                        {
                            for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                            {
                                memset(&sPid[i],0,sizeof(sPid[i]));
                            }
                        }                            
                        EMS_Run_Power_Active_Last = EMS_Run_Power_Active;
                    }
                    else if(EMS_Run_Power_Active>0)  
                    {
                        if(EMS_Run_Power_Active_Last<0)
                        {
                            for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                            {
                                memset(&sPid[i],0,sizeof(sPid[i]));
                            }
                        }                            
                        EMS_Run_Power_Active_Last = EMS_Run_Power_Active;
                    }  
                    else if(EMS_Run_Power_Active==0)
                    {
                        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
                        {
                            memset(&sPid[i],0,sizeof(sPid[i]));
                        }
                        
                        EMS_Run_Power_Active_Last = EMS_Run_Power_Active;
                    }                       
					Grid_Power_Control_Proc(EMS_Run_Power_Active);

					//printf("Const_P_AC_Mode_P %d\n", PCS_Ctrl_Data[0].Const_P_AC_Mode_P );
				}
			}
		}
	}
}

//AGC控制
void AGC_Control(void)
{
	uint16_t AGC_Time=0;	
	if(GRID_RUNNING)
	{
		if(GRID_TYPE==1)
		{
			AGC_Counter[7]++;
			if(AGC_Counter[7]>1*AGC_Time)
			{
				AGC_Counter[7]=0;    				
                if(FUNCTION_ENABLE(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable))
                {//SOC控制                
                    for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)                    
                    {
                        //指定时间且放电未放空或充电未充满              
                        if((Time_Div_Info.Period<10) && 
                           (((EMS_Run_Power_Active> 0)&&(BMS_Info[i].Sys_SOC>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2)&&(!BMS_Com_Err[i]))||
                            ((EMS_Run_Power_Active<=0)&&(BMS_Info[i].Sys_SOC<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2) &&(!BMS_Com_Err[i]))))
                        {
                            //恒流
                            if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_I_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_I_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_I_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_I_Mode_I-PCS_Ctrl_Data[i].Const_I_Mode_I)>1))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_I_Mode=true;
                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_I_Mode=true;
                                }
                            }
                            //恒压
                            else if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_V_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_V_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_V_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_V_Mode_I-PCS_Ctrl_Data[i].Const_V_Mode_I)>1)||
                                       ((abs(PCS_Ctrl_Data[i].Const_V_Mode_V-PCS_Info[i].PCS3x.Rmeasure.DC_Voltage)>20)&&
                                        (abs(PCS_Ctrl_Data[i].Const_V_Mode_I-PCS_Info[i].PCS3x.Rmeasure.DC_Current)>10))||
                                        (PCS_Info[i].PCS4x.RAdjust.Const_V_Mode_V!=PCS_Ctrl_Data[i].Const_V_Mode_V))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_V_Mode=true;
                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_V_Mode=true;
                                }
                            }
                            
                            //恒直流功率
                            else if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_P_DC_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_P_DC_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_P_DC_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_P_DC_Mode_P-PCS_Ctrl_Data[i].Const_P_DC_Mode_P)>1))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode=true;
                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode=true;
                                }
                            }
                            //恒交流功率
                            else if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_P_AC_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_P_AC_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_P_AC_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_P_AC_Mode_P-PCS_Ctrl_Data[i].Const_P_AC_Mode_P)>1))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_P_AC_Mode=true;

                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_P_AC_Mode=true;
                                }
                            }                                
                        }                    
                        else
                        {
                            if((!PCS_E_STOP(i)) && PCS_NO_FATAL_FAULT(i) && (!PCS_Com_Err[i]))
                            {
                                if(!Sys_Stop_Flag)
                                {
                                    if(!PCS_STANDBY_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))
                                    {
                                        PCS_Ind_Ctrl[i].Standby=true;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {//电压控制 
                    //PCS二级过压待机
                    for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
    				{
        				//指定时间且放电未放空或充电未充满				
                        if((Time_Div_Info.Period<10)&&
                           (((EMS_Run_Power_Active> 0)&&(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2))||
                            ((EMS_Run_Power_Active<=0)&&(PCS_Info[i].PCS3x.Rmeasure.DC_Voltage<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2))))
    					{
                            //恒流
                            if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_I_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_I_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_I_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_I_Mode_I-PCS_Ctrl_Data[i].Const_I_Mode_I)>1))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_I_Mode=true;
                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_I_Mode=true;
                                }
                            }
                            //恒压
                            else if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_V_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_V_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_V_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_V_Mode_I-PCS_Ctrl_Data[i].Const_V_Mode_I)>1)||
                                       ((abs(PCS_Ctrl_Data[i].Const_V_Mode_V-PCS_Info[i].PCS3x.Rmeasure.DC_Voltage)>20)&&
                                        (abs(PCS_Ctrl_Data[i].Const_V_Mode_I-PCS_Info[i].PCS3x.Rmeasure.DC_Current)>10))||
                                        (PCS_Info[i].PCS4x.RAdjust.Const_V_Mode_V!=PCS_Ctrl_Data[i].Const_V_Mode_V))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_V_Mode=true;
                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_V_Mode=true;
                                }
                            }
                            
                            //恒直流功率
                            else if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_P_DC_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_P_DC_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_P_DC_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_P_DC_Mode_P-PCS_Ctrl_Data[i].Const_P_DC_Mode_P)>1))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode=true;
                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_P_DC_Mode=true;
                                }
                            }
                            //恒交流功率
                           else if(System_Info.Config.PCS_Grid_Run_Mode == GRID_CONST_P_AC_MODE)
                            {
                                if(((EMS_Run_Power_Active>=0)&&PCS_CONST_P_AC_DISCHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))||\
                                   ((EMS_Run_Power_Active<=0)&&PCS_CONST_P_AC_CHARGE_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status)))
                                {
                                    if((abs(PCS_Info[i].PCS4x.RAdjust.Const_P_AC_Mode_P-PCS_Ctrl_Data[i].Const_P_AC_Mode_P)>1))
                                    {
                                        PCS_Ind_Ctrl[i].Grid_Const_P_AC_Mode=true;

                                    }
                                }
                                else
                                {
                                    PCS_Ind_Ctrl[i].Grid_Const_P_AC_Mode=true;
                                }
                            }                                
                        }                        
                        else
                        {
                            //PCS控制
                            if((!PCS_E_STOP(i)) && PCS_NO_FATAL_FAULT(i) && (!PCS_Com_Err[i]))
                            {
                                if(!Sys_Stop_Flag)
                                {
                                    if(!PCS_STANDBY_STATUS(PCS_Info[i].PCS3x.RCommunication.Run_Status))
                                    {
                                        PCS_Ind_Ctrl[i].Standby=true;
                                    }
                                }
                            }
                        }
    				}
				}
			}					
		}
	}
}

void RunMode_Control(void)
{
    if(System_Info.Config.Time_Divide_Function == TIME_DIV_BY_HOUR)
    {
        //分时电价功能逻辑
        uint8_t j=0;
        for(uint8_t i=0;i<10;i++)
        {
        	if(((Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour*60+Time_Div_Info.Time_Div_Set_Info[i].Start_time.min) == 0)&&
        	   ((Time_Div_Info.Time_Div_Set_Info[i].End_time.hour*60+Time_Div_Info.Time_Div_Set_Info[i].End_time.min) == 0))
        	{

        	}
        	else
        	{
				if((((showTime.tm_hour)*60+(showTime.tm_min)) >= (Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour*60+Time_Div_Info.Time_Div_Set_Info[i].Start_time.min))&&\
				   (((showTime.tm_hour)*60+(showTime.tm_min)) <= (Time_Div_Info.Time_Div_Set_Info[i].End_time.hour*60+Time_Div_Info.Time_Div_Set_Info[i].End_time.min)))
				{
					break;
				}
        	}
            j++;
        }       
        if(j < 10)
        {
            Time_Div_Info.Period = j;
        }
        else
        {
            Time_Div_Info.Period = 10;
            if(!STOP)
            {
    			Sys_Ctrl.Sys_Stop=true;
			}
        }
    }
    else if(System_Info.Config.Time_Divide_Function == TIME_DIV_BY_WEEK)
    {
        //分天功能逻辑
        uint8_t j=0;
        for(uint8_t i=0;i<7;i++)
        {
            if(Time_Div_Info.Time_Div_Set_Info[i].Week 
               == CaculateWeekDay(((showTime.tm_year)+2000),(showTime.tm_mon),(showTime.tm_day)))
            {
                 break;
            }             
            j++;
        }       
        if(j < 7)  
        {
            Time_Div_Info.Period = j;
        }
        else
        {
            Time_Div_Info.Period = 10;
            if(!STOP)
            {
    			Sys_Ctrl.Sys_Stop=true;
			}
		}
	}
	else if (System_Info.Config.Time_Divide_Function == TIME_DIV_DISABLE)
	{
		Time_Div_Info.Period = 0;
	}
}

//根据年月日计算星期几
//y－年，m－月，d－日期
uint8_t CaculateWeekDay(uint16_t y,uint8_t m, uint8_t d)
{
    if((m==1)||(m==2))
    {
        m+=12;
        y--;
    }
    uint8_t temp=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7; 
    uint8_t week=0;
    switch(temp)
    {
        case 0: week=1; break;
        case 1: week=2; break;
        case 2: week=3; break;
        case 3: week=4; break;
        case 4: week=5; break;
        case 5: week=6; break;
        case 6: week=7; break;
    }
    return week; 
}

//电池电压控制PID算法
//算法
void PidReg3Calc(uint8_t p)
{   
    sPid[p].RegErr = sPid[p].RegRef - sPid[p].RegFdb;
    sPid[p].RegUp  = sPid[p].RegKp*sPid[p].RegErr;  
    sPid[p].RegUi  = sPid[p].RegUi + sPid[p].RegKi*sPid[p].RegUp + sPid[p].RegKc*sPid[p].RegSatErr; 
    sPid[p].RegOutPreSat = sPid[p].RegUp + sPid[p].RegUi;
    sPid[p].RegOut = sPid[p].RegOutPreSat;
    if(sPid[p].RegOutPreSat >= sPid[p].RegOutMax)
    {
        sPid[p].RegOut = sPid[p].RegOutMax;
    }
    if(sPid[p].RegOutPreSat <= sPid[p].RegOutMin)
    {
        sPid[p].RegOut = sPid[p].RegOutMin;
    }
    sPid[p].RegSatErr = sPid[p].RegOut - sPid[p].RegOutPreSat;
}
//Ref-设置;Fdb-反馈
//uplimit-上限;lowlimit-下限
float Pid_VoltCtrl(float Ref, float Fdb, float Uplimit , float Lowlimit,uint8_t p)
{    
    sPid[p].RegRef = Ref;              
    sPid[p].RegFdb = Fdb;    
    sPid[p].RegUi  = sPid[p].VacIdUi;
    sPid[p].RegSatErr = sPid[p].UdSatErr;              
    sPid[p].RegKp = 0.01* wInverterAcVoltDQKp;   // yuan 0.675      
    sPid[p].RegKi = 0.01* wInverterAcVoltDQKi;   // yuan 0.06
    sPid[p].RegKc = 0.03;      
    sPid[p].RegOutMax = Uplimit;
    sPid[p].RegOutMin = Lowlimit;
    PidReg3Calc(p);
    sPid[p].VacIdUi = sPid[p].RegUi;
    sPid[p].VacIdOut = sPid[p].RegOut;
    sPid[p].UdSatErr = sPid[p].RegSatErr;
    return sPid[p].VacIdOut;
}

//float testtt;

//Ref-设置;Fdb-反馈
//uplimit-上限;lowlimit-下限
float Pid_PowerCtrl(float Ref, float Fdb, float Uplimit , float Lowlimit,uint8_t p)
{
    sPid[p].RegRef = Ref;                
    sPid[p].RegFdb = Fdb;    
    sPid[p].RegUi  = sPid[p].VacIdUi;
    sPid[p].RegSatErr = sPid[p].UdSatErr;               
    sPid[p].RegKp = 0.01* wInverterAcVoltDQKp_PVLimit;   // yuan 0.675     
    sPid[p].RegKi = 0.01* wInverterAcVoltDQKi_PVLimit;   // yuan 0.06
    sPid[p].RegKc = 0.03;   // 0.03   
    sPid[p].RegOutMax = Uplimit;
    sPid[p].RegOutMin = Lowlimit;
    PidReg3Calc(p);
    sPid[p].VacIdUi = sPid[p].RegUi;
    sPid[p].VacIdOut = sPid[p].RegOut;
    sPid[p].UdSatErr = sPid[p].RegSatErr;
    return sPid[p].VacIdOut;
}

//---------------------------------end of file---------------------------------------//

