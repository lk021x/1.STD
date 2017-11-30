#include "includes.h"

uint8_t doGetRemoteSignal(uint16_t devAddr, uint32_t addr)                  //执行遥信
{
    uint8_t rlt=0;

    switch(addr)
    {
        case 0x01:rlt=(!SYS_NO_FAULT)?SWITCH_ON:SWITCH_OFF;break;//总故障位(闭锁)
        case 0x02:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<< 0))?SWITCH_ON:SWITCH_OFF;break;//储能变流器故障
        case 0x03:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<< 1))?SWITCH_ON:SWITCH_OFF;break;//储能逆变器通讯故障
        case 0x04:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<< 2))?SWITCH_ON:SWITCH_OFF;break;//储能逆变器地址冲突
        case 0x08:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<< 6))?SWITCH_ON:SWITCH_OFF;break;//光伏逆变器地址冲突
        case 0x0A:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<< 8))?SWITCH_ON:SWITCH_OFF;break;//接触器故障
        case 0x0E:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<12))?SWITCH_ON:SWITCH_OFF;break;//负载防雷故障
        case 0x0F:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<13))?SWITCH_ON:SWITCH_OFF;break;//电池电量低
        case 0x11:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<15))?SWITCH_ON:SWITCH_OFF;break;//自检失败
        case 0x12:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<16))?SWITCH_ON:SWITCH_OFF;break;//自检超时
        case 0x13:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<17))?SWITCH_ON:SWITCH_OFF;break;//微网供电启动超时
        case 0x14:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<18))?SWITCH_ON:SWITCH_OFF;break;//微网供电启动失败
        case 0x15:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<20))?SWITCH_ON:SWITCH_OFF;break;//电网供电启动失败
        case 0x19:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1 & ((uint32_t)1<<23))?SWITCH_ON:SWITCH_OFF;break;//停机超时

        case 0x42:rlt=(SYS_ALARM)?SWITCH_ON:SWITCH_OFF;break;//总告警位
        case 0x43:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 0))?SWITCH_ON:SWITCH_OFF;break;//储能逆变器故障告警
        case 0x44:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 1))?SWITCH_ON:SWITCH_OFF;break;//储能逆变器通讯故障告警
        case 0x45:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 2))?SWITCH_ON:SWITCH_OFF;break;//光伏逆变器故障告警
        case 0x46:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 3))?SWITCH_ON:SWITCH_OFF;break;//光伏逆变器通讯故障告警
        case 0x47:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 4))?SWITCH_ON:SWITCH_OFF;break;//电网侧接触器告警
        case 0x48:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 5))?SWITCH_ON:SWITCH_OFF;break;//微网侧接触器告警
		case 0x49:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 6))?SWITCH_ON:SWITCH_OFF;break;//储能逆变器交流未连接
        case 0x4A:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & ((uint32_t)1<< 7))?SWITCH_ON:SWITCH_OFF;break;//电池电量低
        
        case 0x63:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 0))?SWITCH_ON:SWITCH_OFF;break;//自检
        case 0x64:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 1))?SWITCH_ON:SWITCH_OFF;break;//微网供电启动
        case 0x65:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 2))?SWITCH_ON:SWITCH_OFF;break;//电网供电启动
        case 0x66:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 3))?SWITCH_ON:SWITCH_OFF;break;//微网供电运行
        case 0x67:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 4))?SWITCH_ON:SWITCH_OFF;break;//电网供电运行
        case 0x68:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 5))?SWITCH_ON:SWITCH_OFF;break;//故障
        case 0x69:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 6))?SWITCH_ON:SWITCH_OFF;break;//停机中
        case 0x6A:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 7))?SWITCH_ON:SWITCH_OFF;break;//停机
        case 0x6B:rlt=(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status & ((uint32_t)1<< 8))?SWITCH_ON:SWITCH_OFF;break;//紧急停机

		case 0x73:rlt=(System_Info.Config.EMS_Run_Priority == 1)?SWITCH_ON:SWITCH_OFF;break;//微网优先
        case 0x74:rlt=(System_Info.Config.EMS_Run_Priority == 0)?SWITCH_ON:SWITCH_OFF;break;//电网优先
        case 0x75:rlt=(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode==1)?SWITCH_ON:SWITCH_OFF;break; //离网
        case 0x76:rlt=(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode==0)?SWITCH_ON:SWITCH_OFF;break; //并网
        case 0x77:rlt=(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode==2)?SWITCH_ON:SWITCH_OFF;break; //并离网切换
        case 0x78:rlt=(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode==0)?SWITCH_ON:SWITCH_OFF;break; //固定功率
        case 0x79:rlt=(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode==1)?SWITCH_ON:SWITCH_OFF;break; //跟踪负载
        case 0x7A:rlt=(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode==2)?SWITCH_ON:SWITCH_OFF;break; //PCC功率控制
        case 0x7B:rlt=(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode==3)?SWITCH_ON:SWITCH_OFF;break; //防逆流
		case 0x83:rlt=(System_Info.Config.EMS_Start_Type == 1)?SWITCH_ON:SWITCH_OFF;break;//被动模式
        case 0x84:rlt=(System_Info.Config.EMS_Start_Type == 0)?SWITCH_ON:SWITCH_OFF;break;//主动模式
		case 0x93:rlt=(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == QP)?SWITCH_ON:SWITCH_OFF;break;//无功功率调节
		case 0x94:rlt=(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == PF)?SWITCH_ON:SWITCH_OFF;break;//功率因数调节
		case 0x95:rlt=(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == SHUT_OFF)?SWITCH_ON:SWITCH_OFF;break;//关闭调节

        default:rlt=SWITCH_OFF;break;
    }
	return rlt;
}
uint32_t doGetRemoteMesure(uint16_t devAddr, uint32_t addr)                 //执行遥测
{
    float  data = 0.0;
	uint32_t *rdata = (uint32_t *)&data;

	if(addr>=0x4001&&addr<=0x4080)//系统信息
	{
		switch(addr)
		{
			case 0x4001:data=(float)System_Info.Config.System_Power_Set;break; 					        //额定输出功率		  
			case 0x4002:data=(float)((Load_Power_Status<<1)+System_Info.Config.EMS_Run_Priority);break;	//供电状态
			case 0x4003:data=(float)(Grid_Exist?1:0);break;										       //电网状态
            case 0x4004:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max/10.0;break;   //微网电压
            case 0x4005:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency/100.0;break;         //微网频率
            case 0x4006:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max/10.0;break;      //电池最大电压
            case 0x4007:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min/10.0;break;      //电池最小电压
            case 0x4008:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Aver/10.0;break;     //电池平均电压              
            case 0x4009:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Max/10.0;break;          //光伏最大电压
            case 0x400A:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Min/10.0;break;          //光伏最小电压
            case 0x400B:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Aver/10.0;break;          //光伏平均电压
            case 0x400C:data=(float)System_Run_Info.Total_Run_Time;break;                              //总运行时间
            
            case 0x400F:data=(float)P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min/10.0;break;  //微网电压下限
            case 0x4010:data=(float)P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max/10.0;break;  //微网电压上限
            case 0x4011:data=(float)P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min/10.0;break;//微网频率下限
            case 0x4012:data=(float)P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max/10.0;break;//微网频率上限              
            case 0x4013:data=(float)P_Range[System_Info.Config.PCS_Type].Power_Factor.Min/1000.0;break;  //功率因数下限
            case 0x4014:data=(float)P_Range[System_Info.Config.PCS_Type].Power_Factor.Max/1000.0;break;   //功率因数上限
            case 0x4015:data=(float)(-(System_Info.Config.System_Power_Set*0.3));break;                  //无功功率下限
            case 0x4016:data=(float)(System_Info.Config.System_Power_Set*0.3);break;                     //无功功率上限
            case 0x4017:data=(float)P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min/10.0;break; //并网恒压模式电压下限
            case 0x4018:data=(float)P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max/10.0;break; //并网恒压模式电压上限
   			case 0x4019:data=(float)(-(System_Info.Config.System_Power_Set*1.1));break; 			    //EMS并网运行功率设置下限
			case 0x401A:data=(float)(System_Info.Config.System_Power_Set*1.1);break;	                //EMS并网运行功率设置上限
            
            case 0x401D:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num;break;       //储能变流器在线台数
            case 0x401E:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num;break;        //光伏逆变器在线台数
            case 0x401F:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.BMS_Online_Num;break;       //BMS在线台数
            case 0x4020:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Online_Num;break;      //DCDC在线台数
            
            case 0x4022:data=(float)EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Rated_Output_Power/10.0;break; //储能变流器可用容量
            case 0x4023:data=(float)EMS_Info.EMS4x.AGC_Info.SysParameter.PV_Rated_Output_Power/10.0;break;  //光伏逆变器可用容量
            case 0x4024:data=(float)(DIN(Grid_Contacter)?1:0);break; 
            case 0x4025:data=(float)(DIN(MGrid_Contacter)?1:0);break; 
            
            //PCS
            case 0x4029:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power/10.0;break;   //PCS总有功功率
            case 0x402A:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power/10.0;break; //PCS总无功功率           
            case 0x402B:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor/1000.0;break;    //PCS功率因数         
            case 0x402C:data=(float)EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy/10.0;break; //PCS累计充电
            case 0x402D:data=(float)EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy/10.0;break; //PCS累计放电
            case 0x402E:data=(float)System_Run_Info.Day_Charge_Energy/10.0;break;    //PCS电池日充电量
            case 0x402F:data=(float)System_Run_Info.Day_Discharge_Energy/10.0;break; //PCS电池日放电量   
            //PV
            case 0x4033:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power/10.0;break;   //PV总有功功率
            case 0x4034:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power/10.0;break; //PV总无功功率
            case 0x4035:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Powerfactor/1000.0;break;    //PV功率因数
            case 0x4036:data=(float)EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy/10.0;break; //PV累计发电
            case 0x4037:data=(float)System_Run_Info.Day_PV_Energy/10.0;break;                      //PV日发电量
            //BMS
            case 0x403D:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max/10.0;break; //最大电压
            case 0x403E:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min/10.0;break; //最小电压
            case 0x403F:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max/10.0;break;     //最大SOC
            case 0x4040:data=(float)EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min/10.0;break;     //最小SOC
            //DCDC
            case 0x4045:data=(System_Info.Config.DCDC_Type==1)?(float)(EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Active_Power)/10.0:
            		(float)(EMS_Info.EMS3x.AGC_Info.RMeasure.DCDC_Active_Power)/1000.0;break;   //DCDC总有功功率
            case 0x4046:data=(float)EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Charge_Energy/10.0;break;   //DCDC总充
            case 0x4047:data=(float)EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.DCDC_Total_Discharge_Energy/10.0;break;//DCDC总放
            case 0x4048:data=(float)System_Run_Info.Day_DCDC_Charge_Energy/10.0;break;      //DCDC日充
            case 0x4049:data=(float)System_Run_Info.Day_DCDC_Discharge_Energy/10.0;break;   //DCDC日放
            //LOAD
            case 0x404F:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power/10.0;break;   //负荷总有功功率
            case 0x4050:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power/10.0;break; //负荷总无功功率
            case 0x4051:data=(float)EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Powerfactor/1000.0;break;    //负荷功率因数
            case 0x4052:data=(float)System_Run_Info.Total_Load_Energy/10.0;break;            //负荷累计用电
	        
			default:data=(float)0;break;
		}
	}
	else if(addr>=0x4081&&addr<=0x4116) //6台 储能逆变器信息  16513 --- 16662
	{
		uint8_t Pcs_Index = 0;
		Pcs_Index = (addr-0x4081)/25;
		addr = 0x4081+(addr-0x4081)%25;

		if(Pcs_Index>PCS_NUM-1)
		{
			data=(float)0;
			return *rdata;
		}
		
		switch(addr)
		{
			//储能逆变器单台信息
	        case 0x4081:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.DC_Voltage/10.0;break; //储能逆变器直流电压
	        case 0x4082:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.DC_Current/10.0;break; //储能逆变器直流电流
	        case 0x4083:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.DC_Power/10.0;break; //储能逆变器直流功率
	        case 0x4084:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.VAB/10.0;break; //储能逆变器电网电压VAB
	        case 0x4085:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.VBC/10.0;break; //储能逆变器电网电压VBC
	        case 0x4086:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.VCA/10.0;break; //储能逆变器电网电压VCA
	        case 0x4087:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.Grid_Frequency/100.0;break; //储能逆变器电网频率
	        case 0x4088:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.IA/10.0;break; //储能逆变器A相电流
	        case 0x4089:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.IB/10.0;break; //储能逆变器B相电流
	        case 0x408A:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.IC/10.0;break; //储能逆变器C相电流
	        case 0x408B:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.Active_Power/10.0;break; //储能逆变器有功功率
	        case 0x408C:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.Reactive_Power/10.0;break; //储能逆变器无功功率
	        case 0x408D:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.Power_Factor/1000.0;break; //储能逆变器功率因数
	        case 0x408E:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.Total_Charge_Energe/10.0;break; //储能逆变器累计充电
	        case 0x408F:data=(float)PCS_Info[Pcs_Index].PCS3x.Rmeasure.Total_Discharge_Energe/10.0;break;//储能逆变器累计放电
	        case 0x4090:data=(float)PCS_Info[Pcs_Index].PCS3x.RCommunication.Run_Status;break;//储能逆变器设备状态
	        case 0x4091:data=(float)PCS_Info[Pcs_Index].PCS3x.RCommunication.Fault_Status1;break;//储能逆变器故障状态1
	        case 0x4092:data=(float)PCS_Info[Pcs_Index].PCS3x.RCommunication.Fault_Status2;break;//储能逆变器故障状态2
			default:data=(float)0;break;
		}
	}
	else if(addr>=0x4117&&addr<=0x42CE)// 20台 光伏逆变器信息 16663 -- 17102
	{
		uint8_t Pv_Index = 0;
		Pv_Index = (addr-0x4117)/22;
		addr = 0x4117+(addr-0x4117)%22;

		if(Pv_Index>PV_NUM-1)
		{
			data=(float)0;
			return *rdata;
		}
		
		switch(addr)
		{
			//光伏逆变器单台信息
			case 0x4117:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.DC_Voltage1/10.0;break;//光伏逆变器直流电压1
			case 0x4118:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.DC_Current1/10.0;break;//光伏逆变器直流电流1
			case 0x4119:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.DC_Voltage2/10.0;break;//光伏逆变器直流电压2
			case 0x411A:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.DC_Current2/10.0;break;//光伏逆变器直流电流2
			case 0x411B:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.VA/10.0;break;//光伏逆变器电网电压VA
			case 0x411C:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.VB/10.0;break;//光伏逆变器电网电压VB
			case 0x411D:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.VC/10.0;break;//光伏逆变器电网电压VC
			case 0x411E:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.Frequency/10.0;break;//光伏逆变器电网频率
			case 0x411F:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.Total_Active_Power/10.0;break;//光伏逆变器有功功率
			case 0x4120:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.Reactive_Power/10.0;break;//光伏逆变器无功功率
			case 0x4121:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.Power_Factor/1000.0;break;//光伏逆变器功率因数
			case 0x4122:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.Total_Generation/10.0;break;//光伏逆变器累计发电量
			case 0x4123:data=(float)PV_Info[Pv_Index].PV3x.RunParameter.Inverter_Status;break;//光伏逆变器工作状态
			default:data=(float)0;break;
		}
	}
	else if(addr>=0x42CF&&addr<=0x4382)//6台 BMS信息 17103 -- 17282
	{
		uint8_t Bms_Index = 0;
		Bms_Index = (addr-0x42CF)/30;
		addr = 0x42CF+(addr-0x42CF)%30;

		if(Bms_Index>BMS_NUM-1)
		{
			data=(float)0;
			return *rdata;
		}
		
		switch(addr)
        {
        	case 0x42CF:data=(float)BMS_Info[Bms_Index].Sys_Voltage/10.0;break;
        	case 0x42D0:data=(float)BMS_Info[Bms_Index].Sys_Current;break;
        	case 0x42D1:data=(float)BMS_Info[Bms_Index].Sys_SOC/10.0;break;
        	case 0x42D2:data=(float)BMS_Info[Bms_Index].Sys_SOH/10.0;break;
        	case 0x42D3:data=(float)BMS_Info[Bms_Index].Single_V_Min/1000.0;break;
        	case 0x42D4:data=(float)BMS_Info[Bms_Index].Single_V_Max/1000.0;break;
        	case 0x42D5:data=(float)BMS_Info[Bms_Index].Single_T_Min/10.0;break;
        	case 0x42D6:data=(float)BMS_Info[Bms_Index].Single_T_Max/10.0;break;
        	case 0x42D7:data=(float)BMS_Info[Bms_Index].Charge_Limit;break;
        	case 0x42D8:data=(float)BMS_Info[Bms_Index].Discharge_Limit;break;
        	case 0x42D9:data=(float)BMS_Info[Bms_Index].Num_Of_Rack_In_Service;break; //在线rack
        	case 0x42DA:data=(float)BMS_Info[Bms_Index].Num_Of_Rack_In_System;break;  //总rack
        	case 0x42DB:data=(float)BMS_Info[Bms_Index].Rack_Fault_Word;break; //68AH
        	case 0x42DC:data=(float)BMS_Info[Bms_Index].Rack_Alarm_Word;break; //68AH
        	case 0x42DD:data=(float)BMS_Info[Bms_Index].Sys_Alarm_Word;break;  //68AH 	
        	case 0x42DE:data=(float)BMS_Info[Bms_Index].Protection1;break;     //94AH
        	case 0x42DF:data=(float)BMS_Info[Bms_Index].Protection2;break;     //94AH
        	case 0x42E0:data=(float)BMS_Info[Bms_Index].Protection3;break;     //94AH
        	case 0x42E1:data=(float)BMS_Info[Bms_Index].Protection4;break;     //94AH
        	case 0x42E2:data=(float)BMS_Info[Bms_Index].Alarm1;break;          //94AH
        	case 0x42E3:data=(float)BMS_Info[Bms_Index].Alarm2;break;          //94AH
        	case 0x42E4:data=(float)BMS_Info[Bms_Index].Alarm3;break;          //94AH
        	case 0x42E5:data=(float)BMS_Info[Bms_Index].Alarm4;break;          //94AH
        	case 0x42E6:data=(float)BMS_Info[Bms_Index].Sys_Alarm_Status;break;//94AH
        	case 0x42E7:data=(float)BMS_Info[Bms_Index].Sys_Mode;break;        //68ah&94ah				

            default:data=(float)0;break;
        }
	}
	else if(addr>=0x4383&&addr<=0x47BA)// 12台 DCDC单台信息  17283
	{
		uint8_t Dcdc_Index = 0;
		Dcdc_Index = (addr-0x4383)/90;
		addr = 0x4383+(addr-0x4383)%90;

		if(Dcdc_Index>DCDC_NUM-1)
		{
			data=(float)0;
			return *rdata;
		}
		
        if(System_Info.Config.DCDC_Type==1) //单向
        {
    		switch(addr)
    		{
    			//DCDC控制器单台信息
    			case 0x4383:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Array_V/10.0;break;//阵列电压1 17283
    			case 0x4384:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Array_I/10.0;break;//阵列电流1
    			case 0x4385:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Bat_V/10.0;break;//电池电压1
    			case 0x4386:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Charge_I/10.0;break;//充电电流1
    			case 0x4387:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Charge_P/10.0;break;//充电功率1
    			case 0x4388:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_In_Temp/10.0;break;//机内温度1
    			case 0x4389:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Bat_Temp/10.0;break;//电池温度1
    			case 0x438A:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Dev_Status;break;//设备状态1
    			case 0x438B:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Dev_Fault_Status;break;//故障状态1
    			case 0x438C:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Day/10.0;break;//日发电量
    			case 0x438D:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Mon/10.0;break;//月发电量
    			case 0x438E:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Output_E_Total/10.0;break;//发电总量
    			case 0x438F:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_CO2_Reduced/10.0;break;//二氧化碳减排
    			case 0x4390:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Run_Min_Day;break;//日运行分钟
    			case 0x4391:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Run_Hour_Mon;break;//月运行时数
    			case 0x4392:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module_Run_Hour_Total;break;//总运行时数
    			
    			case 0x439C:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Array_V/10.0;break;//阵列电压2 //17308
    			case 0x439D:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Array_I/10.0;break;//阵列电流2
    			case 0x439E:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Bat_V/10.0;break;//电池电压2
    			case 0x439F:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Charge_I/10.0;break;//充电电流2
    			case 0x43A0:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Charge_P/10.0;break;//充电功率2
    			case 0x43A1:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_In_Temp/10.0;break;//机内温度2
    			case 0x43A2:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Bat_Temp/10.0;break;//电池温度2
    			case 0x43A3:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Dev_Status;break;//设备状态2
    			case 0x43A4:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Dev_Fault_Status;break;//故障状态2
    			case 0x43A5:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Day/10.0;break;//日发电量
    			case 0x43A6:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Mon/10.0;break;//月发电量
    			case 0x43A7:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Output_E_Total/10.0;break;//发电总量
    			case 0x43A8:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_CO2_Reduced/10.0;break;//二氧化碳减排
    			case 0x43A9:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Run_Min_Day;break;//日运行分钟
    			case 0x43AA:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Run_Hour_Mon;break;//月运行时数
    			case 0x43AB:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module_Run_Hour_Total;break;//总运行时数

    			case 0x43B5:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Array_V/10.0;break;//阵列电压3 // 17333
    			case 0x43B6:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Array_I/10.0;break;//阵列电流3
    			case 0x43B7:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Bat_V/10.0;break;//电池电压3
    			case 0x43B8:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Charge_I/10.0;break;//充电电流3
    			case 0x43B9:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Charge_P/10.0;break;//充电功率3
    			case 0x43BA:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_In_Temp/10.0;break;//机内温度3
    			case 0x43BB:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Bat_Temp/10.0;break;//电池温度3
    			case 0x43BC:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Dev_Status;break;//设备状态3
    			case 0x43BD:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Dev_Fault_Status;break;//故障状态3
    			case 0x43BE:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Day/10.0;break;//日发电量
    			case 0x43BF:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Mon/10.0;break;//月发电量
    			case 0x43C0:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Output_E_Total/10.0;break;//发电总量
    			case 0x43C1:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_CO2_Reduced/10.0;break;//二氧化碳减排
    			case 0x43C2:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Run_Min_Day;break;//日运行分钟
    			case 0x43C3:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Run_Hour_Mon;break;//月运行时数
    			case 0x43C4:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module_Run_Hour_Total;break;//总运行时数

    			case 0x43CB:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Array_V/10.0;break;//阵列电压4 // 17358
    			case 0x43CC:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Array_I/10.0;break;//阵列电流4
    			case 0x43CD:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Bat_V/10.0;break;//电池电压4
    			case 0x43CE:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Charge_I/10.0;break;//充电电流4
    			case 0x43CF:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Charge_P/10.0;break;//充电功率4
    			case 0x43D0:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_In_Temp/10.0;break;//机内温度4
    			case 0x43D1:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Bat_Temp/10.0;break;//电池温度4
    			case 0x43D2:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Dev_Status;break;//设备状态4
    			case 0x43D3:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Dev_Fault_Status;break;//故障状态4
    			case 0x43D4:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Day/10.0;break;//日发电量
    			case 0x43D5:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Mon/10.0;break;//月发电量
    			case 0x43D6:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Output_E_Total/10.0;break;//发电总量
    			case 0x43D7:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_CO2_Reduced/10.0;break;//二氧化碳减排
    			case 0x43D8:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Run_Min_Day;break;//日运行分钟
    			case 0x43D9:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Run_Hour_Mon;break;//月运行时数
    			case 0x43DA:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module_Run_Hour_Total;break;//总运行时数
    			
    			default:data=(float)0;break;
    		}
		}
        else if (System_Info.Config.DCDC_Type==2) //双向
        {
    		switch(addr)
            {
                case 0x4383:data=(float)DCDC_Info[Dcdc_Index].Module_1.Bus_Bar_V/10.0;break;//母线电压
                case 0x4384:data=(float)DCDC_Info[Dcdc_Index].Module_1.Bus_Bar_I/10.0;break;//母线电流
                case 0x4385:data=(float)DCDC_Info[Dcdc_Index].Module_1.Battery_V/10.0;break;//蓄电池电压
                case 0x4386:data=(float)DCDC_Info[Dcdc_Index].Module_1.Battery_I/10.0;break;//蓄电池电流
                case 0x4387:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module1_T;break;//模组1温度
                case 0x4388:data=(float)DCDC_Info[Dcdc_Index].Module_1.Module2_T;break;//模组2温度
                case 0x4389:data=(float)DCDC_Info[Dcdc_Index].Module_1.Device_Status;break; //设备状态1
                case 0x438A:data=(float)DCDC_Info[Dcdc_Index].Module_1.Fault_Status;break; //故障1

                case 0x439C:data=(float)DCDC_Info[Dcdc_Index].Module_2.Bus_Bar_V/10.0;break;//母线电压
                case 0x439D:data=(float)DCDC_Info[Dcdc_Index].Module_2.Bus_Bar_I/10.0;break;//母线电流
                case 0x439E:data=(float)DCDC_Info[Dcdc_Index].Module_2.Battery_V/10.0;break;//蓄电池电压
                case 0x439F:data=(float)DCDC_Info[Dcdc_Index].Module_2.Battery_I/10.0;break;//蓄电池电流
                case 0x43A0:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module1_T;break;//模组1温度
                case 0x43A1:data=(float)DCDC_Info[Dcdc_Index].Module_2.Module2_T;break;//模组2温度
                case 0x43A2:data=(float)DCDC_Info[Dcdc_Index].Module_2.Device_Status;break; //设备状态2
                case 0x43A3:data=(float)DCDC_Info[Dcdc_Index].Module_2.Fault_Status;break; //故障2

                case 0x43B5:data=(float)DCDC_Info[Dcdc_Index].Module_3.Bus_Bar_V/10.0;break;//母线电压
                case 0x43B6:data=(float)DCDC_Info[Dcdc_Index].Module_3.Bus_Bar_I/10.0;break;//母线电流
                case 0x43B7:data=(float)DCDC_Info[Dcdc_Index].Module_3.Battery_V/10.0;break;//蓄电池电压
                case 0x43B8:data=(float)DCDC_Info[Dcdc_Index].Module_3.Battery_I/10.0;break;//蓄电池电流
                case 0x43B9:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module1_T;break;//模组1温度
                case 0x43BA:data=(float)DCDC_Info[Dcdc_Index].Module_3.Module2_T;break;//模组2温度
                case 0x43BB:data=(float)DCDC_Info[Dcdc_Index].Module_3.Device_Status;break; //设备状态2
                case 0x43BC:data=(float)DCDC_Info[Dcdc_Index].Module_3.Fault_Status;break; //故障2

                case 0x43CE:data=(float)DCDC_Info[Dcdc_Index].Module_4.Bus_Bar_V/10.0;break;//母线电压
                case 0x43CF:data=(float)DCDC_Info[Dcdc_Index].Module_4.Bus_Bar_I/10.0;break;//母线电流
                case 0x43D0:data=(float)DCDC_Info[Dcdc_Index].Module_4.Battery_V/10.0;break;//蓄电池电压
                case 0x43D1:data=(float)DCDC_Info[Dcdc_Index].Module_4.Battery_I/10.0;break;//蓄电池电流
                case 0x43D2:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module1_T;break;//模组1温度
                case 0x43D3:data=(float)DCDC_Info[Dcdc_Index].Module_4.Module2_T;break;//模组2温度
                case 0x43D4:data=(float)DCDC_Info[Dcdc_Index].Module_4.Device_Status;break; //设备状态2
                case 0x43D5:data=(float)DCDC_Info[Dcdc_Index].Module_4.Fault_Status;break;  //故障2

                case 0x43D6:data=(float)DCDC_Info[Dcdc_Index].Module_T.Charge_Discharge_Flag;break;     //充放电标识
                case 0x43D7:data=(float)DCDC_Info[Dcdc_Index].Module_T.Total_Active_Power/1000.0;break; //当前总运行功率
                case 0x43D8:data=(float)DCDC_Info[Dcdc_Index].Module_T.Day_Charge_Energy;break;         //日充放电量
                case 0x43D9:data=(float)DCDC_Info[Dcdc_Index].Module_T.Day_Discharge_Energy;break;      //日放电量
                case 0x43DA:data=(float)DCDC_Info[Dcdc_Index].Module_T.Total_Charge_Energy;break;       //总充电量
                case 0x43DB:data=(float)DCDC_Info[Dcdc_Index].Module_T.Total_Discharge_Energy;break;    //总放电量
    			
				default:data=(float)0;break;
            }
		}
	}
	else if(addr>=0x47BB&&addr<=0x47C0)//时间日期
	{
		switch(addr)
		{
			case 0x47BB:data=(float)((showTime.tm_year)+2000);break;				//系统时间（年）
			case 0x47BC:data=(float)((showTime.tm_mon));break;						//系统时间（月）
			case 0x47BD:data=(float)((showTime.tm_day));break;						//系统时间（日）
			case 0x47BE:data=(float)((showTime.tm_hour));break; 					//系统时间（时）
			case 0x47BF:data=(float)((showTime.tm_min));break;						//系统时间（分）
			case 0x47C0:data=(float)((showTime.tm_sec));break;						//系统时间（秒）
			default:data=(float)0;break;
		}
	}
    return *rdata;
}
uint16_t doRemoteControl(uint16_t devAddr, uint32_t addr, uint8_t action)   //执行遥控
{
    uint16_t rlt=EXE_FAULT;
    //uint16_t INT32_Val=0;

    switch(addr)
    {
		case 0x6001: //电网优先
			if(PC_CAN_SET_DATA)
			{
				if(action==SWITCH_ON)
				{
					System_Info.Config.EMS_Run_Priority=0;
					write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
					rlt = EXE_SUCCESS;
				}
			}
		break;
		case 0x6002: //微网优先
			if(PC_CAN_SET_DATA)
			{
				if(action==SWITCH_ON)
				{
					System_Info.Config.EMS_Run_Priority=1;
					write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);	
					rlt = EXE_SUCCESS;					
				}
			}
		break;
		case 0x6003: //主动模式
			if(PC_CAN_SET_DATA)
			{
				if(action==SWITCH_ON)
				{
					System_Info.Config.EMS_Start_Type= ACTIVE_MODE ;//0;
					write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
					rlt = EXE_SUCCESS;			
				}
			}
		break;
		case 0x6004: //被动模式
			if(PC_CAN_SET_DATA)
			{
				if(action==SWITCH_ON)
				{
					System_Info.Config.EMS_Start_Type=REACTIVE_MODE;//1;
					write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
					rlt = EXE_SUCCESS;			
				}
			}
		break;
		case 0x6005: //系统启动
			if(PC_CAN_SET_DATA)
			{
				if(action==SWITCH_ON)
				{
					Sys_Ctrl.Sys_Start=true;
					rlt = EXE_SUCCESS;
				}
			}
		break;
		case 0x6006: //系统停机
			if(PC_CAN_SET_DATA)
			{
				if(action==SWITCH_ON)
				{
					Sys_Ctrl.Manual_Stop=true;
					rlt = EXE_SUCCESS;
				}
			}
		break;
		case 0x6011: //无功功率调节方式
			if(PC_CAN_SET_DATA)
			{
				if(action==SWITCH_ON)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode = QP;
					write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
					EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=1000;
					write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
					rlt = EXE_SUCCESS;
				}
			}
		break;
        case 0x6012://功率因数调节方式
            if(PC_CAN_SET_DATA)
            {
				if(action==SWITCH_ON)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode = PF;
					write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
					EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0;
					write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
					rlt = EXE_SUCCESS;
				}
            }
        break;
        case 0x6013://关闭调节方式
            if(PC_CAN_SET_DATA)
            {
				if(action==SWITCH_ON)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode = SHUT_OFF;
					write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
					EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=1000;
					write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
					EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0;
					write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
					rlt = EXE_SUCCESS;
				}
            }
        break;
		case 0x601A: //远程&就地
			if(action==SWITCH_ON)
			{
				EMS_RL_Mode=RE_LO_MODE;
				write_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 2);
				rlt=EXE_SUCCESS;
			}
			break;
		case 0x601B: //远程
			if(action==SWITCH_ON)
			{
				EMS_RL_Mode=REMOTE_MODE;
				write_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 2);
				rlt=EXE_SUCCESS;
			}
			break;
		case 0x601C: //就地
			if(action==SWITCH_ON)
			{
				EMS_RL_Mode=LOCAL_MODE;
				write_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 2);
				rlt=EXE_SUCCESS;
			}
			break;
        default:
        break;
    }

    return rlt;
}
uint16_t doRemoteAdjust(uint16_t devAddr, uint32_t addr, uint32_t value)    //执行遥调
{
    float *fp=(float*)&value;
    int32_t INT32_Val=0;
    uint16_t rlt=EXE_FAULT;

    switch(addr)
    {
		case 0x6201: //微网启动电压
			if(PC_CAN_SET_DATA)
			{
				INT32_Val=(int16_t)((*fp)*10);
				if((int16_t)INT32_Val>=P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min && (int16_t)INT32_Val<=P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage=INT32_Val;
					write_eeprom(MG_VOLTAGE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Ctrl_Data[i].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
					rlt=EXE_SUCCESS;
				}
			}
		break;
		case 0x6202: //微网启动频率
			if(PC_CAN_SET_DATA)
			{
				INT32_Val=(int16_t)((*fp)*100);
				if((int16_t)INT32_Val>=P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min && (int16_t)INT32_Val<=P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency=INT32_Val;
					write_eeprom(MG_FREQUENCY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Ctrl_Data[i].Ind_Inverter_F=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
					rlt=EXE_SUCCESS;
				}
			}
		break;
		case 0x6203://并网运行电压下限
			if(PC_CAN_SET_DATA)
			{
				INT32_Val=(int16_t)((*fp)*10);
				if((int16_t)INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min && (int16_t)INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max && INT32_Val<EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min=INT32_Val;
					write_eeprom(CONST_V_MODE_V_MIN_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min, 2);
					rlt=EXE_SUCCESS;
				}
			}
		break;
		case 0x6204://并网运行电压上限
			if(PC_CAN_SET_DATA)
			{
				INT32_Val=(int16_t)((*fp)*10);
				if((int16_t)INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min && (int16_t)INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max && INT32_Val>EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max=INT32_Val;
					write_eeprom(CONST_V_MODE_V_MAX_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max, 2);
					rlt=EXE_SUCCESS;
				}
			}
		break;

        case 0x6206://无功功率
            if(PC_CAN_SET_DATA)
            {
				if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==QP)
				{
                    INT32_Val=(int16_t)(*fp);
                    if(((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*0.3)&&((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*0.3))
                    {
                        EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=(int16_t)INT32_Val;                            
                        write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
                        
                        EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
                        write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
						rlt=EXE_SUCCESS;
					}
				}
            }
        break;
        case 0x6207://功率因数
            if(PC_CAN_SET_DATA)
            {
				if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==PF)
				{
					INT32_Val=(int16_t)((*fp)*1000);
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
						rlt=EXE_SUCCESS;
					}
				}
            }
        break;		

        case 0x6208://运行功率设定0
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[0].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Power,2);                    
                    rlt=EXE_SUCCESS;
                }
    		}
        break;  

        case 0x6209://运行功率设定1
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[1].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[1].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break;  

        case 0x620A://运行功率设定2
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[2].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+2,&Time_Div_Info.Time_Div_Set_Info[2].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break;  

        case 0x620B://运行功率设定3
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[3].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+4,&Time_Div_Info.Time_Div_Set_Info[3].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break; 

        case 0x620C://运行功率设定4
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[4].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+6,&Time_Div_Info.Time_Div_Set_Info[4].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break; 

        case 0x620D://运行功率设定5
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[5].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+8,&Time_Div_Info.Time_Div_Set_Info[5].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break;   

        case 0x620E://运行功率设定6
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[6].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+10,&Time_Div_Info.Time_Div_Set_Info[6].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break;  

        case 0x620F://运行功率设定7
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[7].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+12,&Time_Div_Info.Time_Div_Set_Info[7].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break;  
        case 0x6210://运行功率设定8
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[8].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+14,&Time_Div_Info.Time_Div_Set_Info[8].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break;  

        case 0x6211://运行功率设定9
    		if(PC_CAN_SET_DATA)
    		{
    			INT32_Val=(int16_t)(*fp);
    			if((((int16_t)INT32_Val)>=-(int16_t)(System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)(System_Info.Config.System_Power_Set*1.1)))
                {
                    Time_Div_Info.Time_Div_Set_Info[9].Run_Power = (int16_t)INT32_Val;
                    write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+16,&Time_Div_Info.Time_Div_Set_Info[9].Run_Power,2);
                    rlt=EXE_SUCCESS;
                }
    		}
        break;  

        default:
        break;
    }
    return rlt;
}
uint32_t doGetRemotePulse(uint16_t devAddr, uint32_t addr)                  //执行遥脉
{
    uint32_t data = 0;
    //     
    //     switch(addr)
    //     {
    //         case 0x6401://日充电量
    //             data = System_Info.Com_Info.Day_Input_Energy/10;
    //         break;
    //         case 0x6402://日放电量
    //             data = System_Info.Com_Info.Day_Output_Energy/10;
    //         break;
    //         case 0x6403://月充电量
    //             data = System_Info.Com_Info.Month_Input_Energy/10;
    //         break;
    //         case 0x6404://月放电量
    //             data = System_Info.Com_Info.Month_Output_Energy/10;
    //         break;
    //         case 0x6405://年充电量
    //             data = System_Info.Com_Info.Month_Input_Energy/10;
    //         break;
    //         case 0x6406://年放电量
    //             data = System_Info.Com_Info.Month_Output_Energy/10;
    //         break;
    //         case 0x6407://总充电量
    //             data = System_Info.Com_Info.Total_Input_Energy/10;
    //         break;
    //         case 0x6408://总放电量
    //             data = System_Info.Com_Info.Total_Output_Energy/10;
    //         break;
    //         case 0x6409://日充电时间
    //             data = System_Info.Com_Info.Day_Input_Time;
    //         break;
    //         case 0x640A://日放电时间
    //             data = System_Info.Com_Info.Day_Output_Time;
    //         break;
    //         case 0x640B://月充电时间
    //             data = System_Info.Com_Info.Month_Input_Time/60;
    //         break;
    //         case 0x640C://月放电时间
    //             data = System_Info.Com_Info.Month_Output_Time/60;
    //         break;
    //         case 0x640D://年充电时间
    //             data = System_Info.Com_Info.Month_Input_Time/60;
    //         break;
    //         case 0x640E://年放电时间
    //             data = System_Info.Com_Info.Month_Output_Time/60;
    //         break;
    //         case 0x640F://总充电时间
    //             data = System_Info.Com_Info.Total_Input_Time/60;
    //         break;
    //         case 0x6410://总放电时间
    //             data = System_Info.Com_Info.Total_Output_Time/60;
    //         break;
    //         case 0x6411://总运行时间
    //             data = System_Info.Com_Info.Total_Run_Hour;
    //         break;
    //         default:
    //         break;
    //     }
    return data;
}
uint16_t doSetTime(uint16_t devAddr, const uint8_t *time)                   //执行对时
{
    uint8_t second = ((time[1] << 8) + time[0]) / 1000;

    Touch_Write_Time[0] = time[6]&0x7f;//年
    Touch_Write_Time[1] = time[5]&0x0f;//月
    Touch_Write_Time[2] = time[4]&0x1f;//日
    Touch_Write_Time[3] = time[3]&0x1f;//时
    Touch_Write_Time[4] = time[2]&0x3f;//分
    Touch_Write_Time[5] = second; //秒

    set_sys_time();
	delay10s=true;
	Clear_Data = true;								//判断是否是新的一天，是否要清除前一天的数据

    return EXE_SUCCESS;
}
void doChangeSignalUpload(void)												//遥信变位上传
{
	uint8_t  CS_Num          = 0;
	uint16_t  msec           = 0;
//-----------------------------------------------------------------------------------------------------	
	uint64_t NewFault    	 = (((uint64_t)(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2)) << 32) |
							    ((uint64_t)(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1));
	uint64_t ChangeFaultCode = OldFault ^ NewFault;
	uint64_t Fault_Temp      = NewFault & ChangeFaultCode;
	const uint8_t FaultAddr[64] =
	{//每个比特位记录该故障在遥信中的地址  ccg 2014.5.13
//  BIT    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
		0x02,0x03,0x04,0x00,0x00,0x00,0x08,0x00,0x0A,0x00,0x00,0x00,0x0E,0x0F,0x00,0x11,
		0x12,0x13,0x14,0x15,0x00,0x00,0x00,0x19,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//  BIT    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};
//----------------------------------------------------------------------------------------------------- 
	uint32_t NewAlarm		 = EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word;
	uint32_t ChangeAlarmCode = OldAlarm ^ NewAlarm;
	uint32_t Alarm_Temp 	 = NewAlarm & ChangeAlarmCode;
	const uint8_t AlarmAddr[32] = 
	{
//	BIT    0	1	 2	  3    4	5	 6	  7    8	9	10	 11   12   13	14	 15
		0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};
//-----------------------------------------------------------------------------------------------------	
	uint16_t NewEvent    	 = (uint16_t)EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status;
	uint16_t ChangeEventCode = OldEvent ^ NewEvent;
	uint16_t Event_Temp      = NewEvent & ChangeEventCode;
	const uint8_t EventAddr[16] = 
	{
//  BIT    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
		0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};
//-----------------------------------------------------------------------------------------------------	
	uint64_t NewOther    	 = 0;
//--------------------------------------
	if(System_Info.Config.EMS_Run_Priority == 1)//微网优先
	{
		NewOther=NewOther|(uint64_t)1<<0;
		NewOther=NewOther&~((uint64_t)1<<1);
	}
	else if(System_Info.Config.EMS_Run_Priority == 0)//电网优先
	{
		NewOther=NewOther|(uint64_t)1<<1;
		NewOther=NewOther&~((uint64_t)1<<0);
	}
	
	if(System_Info.Config.EMS_Start_Type == 0)//被动模式
	{
		NewOther=NewOther|(uint64_t)1<<16;
		NewOther=NewOther&~((uint64_t)1<<17);
	}
	else if(System_Info.Config.EMS_Start_Type == 1)//主动模式
	{
		NewOther=NewOther|(uint64_t)1<<17;
		NewOther=NewOther&~((uint64_t)1<<16);
	}
	
	/*if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == SHUT_OFF)//关闭
	{
		NewOther=NewOther&~((uint64_t)1<<32);
		NewOther=NewOther&~((uint64_t)1<<33);
		NewOther=NewOther|(uint64_t)1<<34;
	}
	else if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == QP)
	{
		NewOther=NewOther|(uint64_t)1<<32;
		NewOther=NewOther&~((uint64_t)1<<33);
		NewOther=NewOther&~((uint64_t)1<<34);
	}
	else if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode == PF)
	{
		NewOther=NewOther&~((uint64_t)1<<32);
		NewOther=NewOther|(uint64_t)1<<33;
		NewOther=NewOther&~((uint64_t)1<<34);
	}*/
//--------------------------------------
	uint64_t ChangeOtherCode = OldOther ^ NewOther;
	uint64_t Other_Temp      = NewOther & ChangeOtherCode;
//--------------------------------------
	const uint8_t OtherAddr[64] = 
	{
//  BIT    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
		0x73,0x74,0x00,0x00,0x3A,0x3B,0x3E,0x3F,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x83,0x84,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		
		0x93,0x94,0x95,0x00,0x3A,0x3B,0x3E,0x3F,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};
//-----------------------------------------------------------------------------------------------------
	uint8_t NewYX001 = SYS_NO_FAULT?0:1;
	uint8_t NewYX042 = SYS_ALARM?0:1;
	for(uint8_t i=0;i<64;i++)//故障
	{
		if((ChangeFaultCode>>i) & 1)//故障变位
		{
			CS[CS_Num].data=(Fault_Temp>>i) & 1;	//遥信变位
			CS[CS_Num].addr=FaultAddr[i];			//变位地址
			msec=showTime.tm_sec*1000;
			CS[CS_Num].time[0]=LOBYTE(msec);
			CS[CS_Num].time[1]=HIBYTE(msec);
			CS[CS_Num].time[2]=showTime.tm_min;
			CS[CS_Num].time[3]=showTime.tm_hour;
			CS[CS_Num].time[4]=showTime.tm_day;
			CS[CS_Num].time[5]=showTime.tm_mon;
			CS[CS_Num].time[6]=showTime.tm_year;
			CS_Num++;
		}
	}
	for(uint8_t i=0;i<32;i++)//告警
	{
		if((ChangeAlarmCode>>i) & 1)//故障变位
		{
			CS[CS_Num].data=(Alarm_Temp>>i) & 1;	//遥信变位
			CS[CS_Num].addr=AlarmAddr[i];			//变位地址
			msec=showTime.tm_sec*1000;
			CS[CS_Num].time[0]=LOBYTE(msec);
			CS[CS_Num].time[1]=HIBYTE(msec);
			CS[CS_Num].time[2]=showTime.tm_min;
			CS[CS_Num].time[3]=showTime.tm_hour;
			CS[CS_Num].time[4]=showTime.tm_day;
			CS[CS_Num].time[5]=showTime.tm_mon;
			CS[CS_Num].time[6]=showTime.tm_year;
			CS_Num++;
		}
	}
	if(OldYX001 != NewYX001)//总故障位(闭锁)
	{
		//总故障位(闭锁)
		CS[CS_Num].data=NewYX001;				//遥信变位
		CS[CS_Num].addr=0x01;					//变位地址
		msec=showTime.tm_sec*1000;
		CS[CS_Num].time[0]=LOBYTE(msec);
		CS[CS_Num].time[1]=HIBYTE(msec);
		CS[CS_Num].time[2]=showTime.tm_min;
		CS[CS_Num].time[3]=showTime.tm_hour;
		CS[CS_Num].time[4]=showTime.tm_day;
		CS[CS_Num].time[5]=showTime.tm_mon;
		CS[CS_Num].time[6]=showTime.tm_year;
		CS_Num++;
		//系统状态 故障
		CS[CS_Num].data=NewYX001;				//遥信变位
		CS[CS_Num].addr=0x68;					//变位地址
		msec=showTime.tm_sec*1000;
		CS[CS_Num].time[0]=LOBYTE(msec);
		CS[CS_Num].time[1]=HIBYTE(msec);
		CS[CS_Num].time[2]=showTime.tm_min;
		CS[CS_Num].time[3]=showTime.tm_hour;
		CS[CS_Num].time[4]=showTime.tm_day;
		CS[CS_Num].time[5]=showTime.tm_mon;
		CS[CS_Num].time[6]=showTime.tm_year;
		CS_Num++;
	}
	if(OldYX042 != NewYX042)//总告警位(闭锁)
	{
		//总故障位(闭锁)
		CS[CS_Num].data=NewYX042;				//遥信变位
		CS[CS_Num].addr=0x42;					//变位地址
		msec=showTime.tm_sec*1000;
		CS[CS_Num].time[0]=LOBYTE(msec);
		CS[CS_Num].time[1]=HIBYTE(msec);
		CS[CS_Num].time[2]=showTime.tm_min;
		CS[CS_Num].time[3]=showTime.tm_hour;
		CS[CS_Num].time[4]=showTime.tm_day;
		CS[CS_Num].time[5]=showTime.tm_mon;
		CS[CS_Num].time[6]=showTime.tm_year;
		CS_Num++;
	}
//-----------------------------------------------------------------------------------------------------	
	for(uint8_t i=0;i<16;i++)//事件变位
	{
		if((ChangeEventCode>>i) & 1)
		{
			CS[CS_Num].data=(Event_Temp>>i) & 1;
			CS[CS_Num].addr=EventAddr[i];
			msec=showTime.tm_sec*1000;
			CS[CS_Num].time[0]=LOBYTE(msec);
			CS[CS_Num].time[1]=HIBYTE(msec);
			CS[CS_Num].time[2]=showTime.tm_min;
			CS[CS_Num].time[3]=showTime.tm_hour;
			CS[CS_Num].time[4]=showTime.tm_day;
			CS[CS_Num].time[5]=showTime.tm_mon;
			CS[CS_Num].time[6]=showTime.tm_year;
			CS_Num++;
		}
	}
//-----------------------------------------------------------------------------------------------------	
	for(uint8_t i=0;i<64;i++)//其他变位
	{
		if((ChangeOtherCode>>i) & 1)//其他变位
		{
			CS[CS_Num].data=(Other_Temp>>i) & 1;	//遥信变位
			CS[CS_Num].addr=OtherAddr[i];			//变位地址
			msec=showTime.tm_sec*1000;
			CS[CS_Num].time[0]=LOBYTE(msec);
			CS[CS_Num].time[1]=HIBYTE(msec);
			CS[CS_Num].time[2]=showTime.tm_min;
			CS[CS_Num].time[3]=showTime.tm_hour;
			CS[CS_Num].time[4]=showTime.tm_day;
			CS[CS_Num].time[5]=showTime.tm_mon;
			CS[CS_Num].time[6]=showTime.tm_year;
			CS_Num++;
		}
	}
//-----------------------------------------------------------------------------------------------------	
	OldFault=NewFault;
	OldAlarm=NewAlarm;
	OldEvent=NewEvent;
	OldOther=NewOther;
	OldYX001=NewYX001;
	OldYX042=NewYX042;
//-----------------------------------------------------------------------------------------------------	
	if(CS_Num>0)
	{
		for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++)
		{
			if(g_iec104_ss[i]!=NULL)
			{
				if(g_iec104_ss[i]->Start_Call_All == true)//104总招之后，才主动上传
				{
					IEC104_UploadRemoteSignal(System_Info.IEC104_Address,CS,CS_Num,g_iec104_ss[i]);//上送变位信息  ccg 2013.11.30
				}
			}
		}
	}
}
void doChangeMeasureUpload(void)							//
{
	for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++)
	{
		if(g_iec104_ss[i]!=NULL&&g_iec104_ss[i]->Start_Call_All == true) //
		{
			IEC104_UploadRemoteMesureChanged(g_iec104_ss[i]);//
		}
	}
}

