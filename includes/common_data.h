#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

//DEFINE------------------------------------------------------------------------------------------------------------------------
//定义LCD软件版本
#define LCD_SOFTWARE_VERSION        "_V1_A_M"   //(V1~V100)_(A~Z)
//-------------------------------------------------------------------------------

//定义使能Web调试
#define Web_RunTime_Debug			1

#define GENERAL_SERIAL_NUM			5		//通用串口个数

//判断MAC地址是否合法
#define Mac_Valid(mac) 				(mac[0] == 0xAC && mac[1] == 0x19 && mac[2] == 0x9f)
//----------------------------------------------------------------------------------------------------------
#define TCP_MODBUS_LINK_NUM         5
#define TCP_IEC104_LINK_NUM         5
//----------------------------------------------------------------------------------------------------------
#define HIBYTE(v1)                  ((uint8_t)((v1)>>8))
#define LOBYTE(v1)                  ((uint8_t)((v1)&0xff))
#define HIWORD(v1)                  ((uint16_t)((v1)>>16))
#define LOWORD(v1)                  ((uint16_t)((v1)&0xffff))
//----------------------------------------------------------------------------------------------------------
#define RE_LO_MODE                  0X00
#define REMOTE_MODE                 0X01
#define LOCAL_MODE                  0X02
//----------------------------------------------------------------------------------------------------------
#define NORMAL_OPEN                 0xAA
#define NORMAL_CLOSE                0x55 
//----------------------------------------------------------------------------------------------------------
#define SN_ENABLE                   0xAA
#define SN_DISABLE                  0x55
//----------------------------------------------------------------------------------------------------------
#define ENABLE                      0xAA
#define DISABLE                     0x55
//----------------------------------------------------------------------------------------------------------
#define START_COMMAND               1
#define STOP_COMMAND                2
//----------------------------------------------------------------------------------------------------------
#define NEW_PROTOCOL                0
#define OLD_PROTOCOL                1
//----------------------------------------------------------------------------------------------------------
#define SHUT_OFF                    0x55
#define PF                          0xA1
#define QP                          0xA2
//----------------------------------------------------------------------------------------------------------
#define ISLAND                      0x00
#define LVRT                        0x01
#define SEAMLESS                    0x02
//----------------------------------------------------------------------------------------------------------
#define SWITCH_OFF                  0
#define SWITCH_ON                   1
#define SWITCH_ON_OPEN              1
#define SWITCH_ON_CLOSE             2
//----------------------------------------------------------------------------------------------------------
#define SIGNAL_FULL                 40			//PCS、PV 20秒无通讯，视为通讯故障
#define BMS_SIGNAL_FULL             40			//BMS 10秒无通讯，视为故障，此时机器停机
#define METER_SIGNAL_FULL           24          //12秒判断电网掉电
#define PVS_SIGNAL_FULL				40 			//PVS最大50组轮询超时时间
#define ENV_SIGNAL_FULL				6

//----------------------------------------------------------------------------------------------------------
//#define TIME_900_SECONDS          900        //间隔的秒数15min刷一次
//----------------------------------------------------------------------------------------------------------
//#define SOFT_START_V				3707
#define SOFT_START_315_V			2807
#define SOFT_START_400_V			3707  // 3707
//----------------------------------------------------------------------------------------------------------
#define  MODBUS_BUF_LEN             255
#define  CAN_BUF_LEN                255
//----------------------------------------------------------------------------------------------------------
#define COM_DSP_MODE_UART485        0
#define COM_DSP_MODE_CAN            1
//----------------------------------------------------------------------------------------------------------
#define COM_BMS_MODE_NONE           0
#define COM_BMS_MODE_CAN            1
#define COM_BMS_MODE_UART485        2
//----------------------------------------------------------------------------------------------------------
#define PICTURE_CHARGE              0
#define PICTURE_DISCHARGE           1
#define PICTURE_STOP                2
//----------------------------------------------------------------------------------------------------------
#define D_DHCP_ON                   1
#define D_DHCP_OFF                  0
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
#define CHINESE           			0x55
#define ENGLISH          			0xAA
//----------------------------------------------------------------------------------------------------------
#define IO_CHECK_FAULT_NO           0x55
#define IO_CHECK_FAULT_YES          0xAA
//----------------------------------------------------------------------------------------------------------
//DSP调试使用  ccg 2013.10.28
#define SLOW_UP_ENABLED_OFF         0
#define SLOW_UP_ENABLED_DFT         1
#define SLOW_UP_ENABLED_ON          1

#define DEVICE_PARALLEL_ADDRESS_MIN 1
#define DEVICE_PARALLEL_ADDRESS_DFT 1
#define DEVICE_PARALLEL_ADDRESS_MAX 10

#define DC_V_ADJ_VAL_MIN            -200
#define DC_V_ADJ_VAL_DFT            0
#define DC_V_ADJ_VAL_MAX            200
//----------------------------------------------------------------------------------------------------------
//通讯参数默认值
#define PC_RS485_BAUD_DEFAULT       2//9600
#define PCS_RS485_BAUD_DEFAULT      2
#define PV_RS485_BAUD_DEFAULT       2
#define PC_RS485_COM_ADDR_DEFAULT   1
#define BMS_RS485_COM_ADDR_DEFAULT  1
//----------------------------------------------------------------------------------------------------------
//历史复位参赛
#define HISRESETRECORD_NB_MAX       100			//最大记录数
#define EVERY_RESETRECORD_NB        8  			//每条记录的字节长度
#define EVERY_RESETRECORD_PAGE_NB   5  			//每页显示历史事件记录数以前是7,现在改为5条
//历史事件参数
#define HISEVENTRECORD_NB_MAX       10000		//最大记录数
#define EVERY_EVENTRECORD_NB        8  			//每条记录的字节长度
#define EVERY_EVENTRECORD_PAGE_NB   5  			//每页显示历史事件记录数以前是7,现在改为5条
//历史故障参数
#define HISFAULTRECORD_NB_MAX       10000		//最大记录数
#define EVERY_FAULTRECORD_NB        12 			//每条记录的字节长度 10   每条记录预留16个字节地址空间
#define EVERY_FAULTRECORD_PAGE_NB   5  			//每页显示历史故障记录数
//历史告警参数
#define HISALARMRECORD_NB_MAX       10000		//最大记录数
#define EVERY_ALARMRECORD_NB        12 			//每条记录的字节长度 10   每条记录预留16个字节地址空间
#define EVERY_ALARMRECORD_PAGE_NB   5  			//每页显示历史故障记录数
//接触器控制状态
#define OP_LOW						0
#define OP_HIGH						1

//系统配置参数
#define SYS_POWER_SET_MIN			0
#define SYS_POWER_SET_MAX			29000
#define SYS_POWER_SET_DFT			100

//----------------------------------------------------------------------------------------------------------
#define PC_CAN_SET_DATA    ((EMS_RL_Mode == REMOTE_MODE)	|| (EMS_RL_Mode == RE_LO_MODE))
#define TOUCH_CAN_SET_DATA ((EMS_RL_Mode == LOCAL_MODE)|| (EMS_RL_Mode == RE_LO_MODE))
#define SOC_CTRL_ENABLE			(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable==0xAA)
#define BAT_CTRL_ENABLE			(EMS_Info.EMS4x.SOC_Info.RunParameter.BatVoltage_Control_Enable==0xAA)  
//----------------------------------------------------------------------------------------------------------
#define SYS_NO_FAULT    ((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1==0)&& \
								 (EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2==0))

#define SYS_ALARM		(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word != 0)

#define GRID_CONTACTOR_FAULT_SET	(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= (uint32_t)1<<4)		//电网侧接触器故障置位
#define MGRID_CONTACTOR_FAULT_SET	(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word |= (uint32_t)1<<5)		//微网侧接触器故障置位
#define GRID_CONTACTOR_FAULT_CLR	(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<4))		//电网侧接触器故障清位
#define MGRID_CONTACTOR_FAULT_CLR	(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word &= ~((uint32_t)1<<5))		//微网侧接触器故障清位



#define GRID_CONTACTOR_FAULT	((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & (uint32_t)1<<4)==1<<4)		//电网侧接触器故障标志

#define MGRID_CONTACTOR_FAULT	((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & (uint32_t)1<<5)==1<<5)	    //微网侧接触器故障标志

#define PCS_CONTACTOR_FAULT     ((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word & (uint32_t)1<<6)==1<<6)     //PCS交流未连接
#define MGRID_FAULT    			(MGRID_CONTACTOR_FAULT || PCS_CONTACTOR_FAULT)	

#define ALL_CONTACTOR_FAULT		(GRID_CONTACTOR_FAULT  && MGRID_CONTACTOR_FAULT)  //微网及电网接触器均故障
#define PCS_NO_FAULT(a)		        ((PCS_Info[a].PCS3x.RCommunication.Fault_Status1==0)&& \
									 (PCS_Info[a].PCS3x.RCommunication.Fault_Status2==0))

#define PCS_NO_FATAL_FAULT(a)       (((PCS_Info[a].PCS3x.RCommunication.Fault_Status1 & 0xFFFFFD87)==0)&& \
									 (PCS_Info[a].PCS3x.RCommunication.Fault_Status2==0))

#define PV_FAULT(a)			        ((PV_Info[a].PV3x.RunParameter.Fault_Status1!=0)|| \
									 (PV_Info[a].PV3x.RunParameter.Fault_Status2!=0))

#define CURRENT_STATUS_CHARGE(a)          (((a) == (uint32_t)1<<3) || \
                                           ((a) == (uint32_t)1<<4) || \
                                           ((a) == (uint32_t)1<<5) || \
                                           ((a) == (uint32_t)1<<14))

#define CURRENT_STATUS_DISCHARGE(a)       (((a) == (uint32_t)1<<8) || \
                                           ((a) == (uint32_t)1<<9) || \
                                           ((a) == (uint32_t)1<<10)|| \
                                           ((a) == (uint32_t)1<<15))

#define CURRENT_STATUS_IND_INVERTER(a)     ((a) == (uint32_t)1<<13)

#define CURRENT_STATUS_NOT_IND_INVERTER(a) ((a) != ((uint32_t)1<<13))

#define CURRENT_STATUS_NOT_C_D_TIME(a)    (((a) != ((uint32_t)1<<14)) && \
                                           ((a) != ((uint32_t)1<<15)))

#define CURRENT_STATUS_C_D_TIME(a)        (((a) == ((uint32_t)1<<14)) || \
                                           ((a) == ((uint32_t)1<<15)))

//ccg 2013.5.8
#define TOUCH_SET_STATUS_CHARGE(a)        (((a) == 3) || \
                                           ((a) == 4) || \
                                           ((a) == 5) || \
                                           ((a) == 6))//ccg 2013.5.17
#define PC_SET_STATUS_CHARGE(a)           (((a) == (uint32_t)1<<3) || \
                                           ((a) == (uint32_t)1<<4) || \
                                           ((a) == (uint32_t)1<<5) || \
                                           ((a) == (uint32_t)1<<6))//ccg 2013.5.17
#define PC_SET_STATUS_NOT_C_D_TIME(a)     (((a) != (uint32_t)1<<14) && \
                                           ((a) != (uint32_t)1<<15))//ccg 2013.5.18
#define TOUCH_SET_STATUS_DISCHARGE(a)     (((a) == 8) || \
                                           ((a) == 9) || \
                                           ((a) == 10)|| \
                                           ((a) == 11))//ccg 2013.5.17
#define PC_SET_STATUS_DISCHARGE(a)        (((a) == (uint32_t)1<<8) || \
                                           ((a) == (uint32_t)1<<9) || \
                                           ((a) == (uint32_t)1<<10)|| \
                                           ((a) == (uint32_t)1<<11))//ccg 2013.5.17
#define TOUCH_SET_STATUS_IND_INVERTER(a)   ((a) == 13)

#define PC_SET_STATUS_IND_INVERTER(a)      ((a) == (uint32_t)1<<13)

#define PCS_E_STOP(a)           (PCS_Info[a].PCS3x.RCommunication.Run_Status == (uint32_t)1<<1)

#define CURRENT_STATUS_FAULT_STOP(a)       ((a) == (uint32_t)1<<16)//lixia;2013.5.7;

#define DIN(a)							   (!dig_input[a])

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------
//bank服务器域名
#define SOLARBANK_DOMAIN_NAME "data1.solarinfobank.com"
//----------------------------------------------------------------------------------------------------------
//http协议端口号
#define WEBSERVER_PORT   	  80
//modbus协议端口号
#define MODBUS_PORT 		  502
//modbus广播地址
#define MODBUS_BOARDCAST_ADDR 0
//----------------------------------------------------------------------------------------------------------
//webserver
#define PARAMETER_NUM 		  100                	//用于配置的参数个数
//----------------------------------------------------------------------------------------------------------
//微网中各设备数量
#define PCS_NUM					    60							//PCS总台数   最大60
#define PV_NUM						60							//PV总台数    最大60
#define DCDC_NUM					30							//DCDC总台数  最大30
#define PVS_NUM                     50							//汇流箱总台数 最大50
#define BMS_NUM                     60                          //BMS总台数   最大60
#define Meter_NUM					12							//METER总台数 最大12
#define Load_NUM 					1
#define DSP_NUM                     1
#define ENV_NUM						1							//环境检测仪

//硬件配置设置
#define METER_POS					2					             //1-电网侧；2-负载侧；3-电网侧和负载侧；其它-未定义
#define DC_BUS_SYS					(System_Info.Config.Bus_Type==0)  //0-交流母线；1-直流母线
#define GRID_TYPE					(Grid_Connect_Type==1)     	      //0-非并网型；1-并网型
#define PARA_ONLY					1					              //1-仅并联启动。
#define DC_CONTACTOR				0					              //0-交流接触器；1-交直流两用接触器
#define PROTOCOL_VER				0					              //0-新协议；1-旧协议
//PCS工作状态
//分时功率控制最大数组
#define TIME_IN_PERIOD        (Time_Div_Info.Period<10)

//运行模式
#define GRID_MODE              0
#define ISLAND_MODE            1
#define GRID_ISLAND_MODE       2

//并网功率控制模式
#define CONST_POWER           0
#define FOLLOW_LOAD_POWER     1
#define PCC_POWER_CTRL        2
#define ANTI_REFLUX           3    //防逆流
#define PEAK_LOAD_SHIFTING	  10   //削峰填谷
#define ACSIDE_PVLIMIT        11   //交流侧限发
#define RAMP_RATE_CONTROL     12   //斜率控制,输出平滑
#define FM     				  13   //调频

//并网模式设置
#define GRID_CONST_I_MODE      0
#define GRID_CONST_V_MODE      1
#define GRID_CONST_P_DC_MODE   2
#define GRID_CONST_P_AC_MODE   3
#define GRID_STANDBY_MODE      4
#define GRID_STOP_MODE         5
//离网主动被动模式设置
#define ACTIVE_MODE            0
#define REACTIVE_MODE          1
//离网模式设置
#define ISLAND_START           0
#define ISLAND_STOP            1

//EMS工作状态
#define INIT                    ( EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status      ==(0   ))
#define CHECKING				((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<0)==(1<<0))
#define ISLAND_STARTING			((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<1)==(1<<1))
#define GRID_STARTING	        ((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<2)==(1<<2))
#define ISLAND_RUNNING			((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<3)==(1<<3))
#define GRID_RUNNING 	        ((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<4)==(1<<4))
#define FAULT					((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<5)==(1<<5))
#define STOPPING				((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<6)==(1<<6))
#define STOP					((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<7)==(1<<7))
#define EMGSTOP					((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<8)==(1<<8))
#define SYS_RUNNING				((GRID_RUNNING)||(ISLAND_RUNNING))

// #define GRID_SUPPLY			((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<15)==(1<<15))
// #define MGRID_SUPPLY			((EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status&1<<16)==(1<<16))

//function
#define MAXU16						0xFFFF
#define MAXINT16					0x7FFF
#define MAXU32						0xFFFFFFFF
#define MAXINT32					0x7FFFFFFF
#define MINU16						0
#define MININT16					0x8000
#define MINU32						0
#define MININT32					0x80000000

#define MAX2(a,b)				(((a)>(b))?(a):(b))			//取最大值
#define MIN2(a,b)				(((a)>(b))?(b):(a))			//取最小值
#define LIMIT(a,c,b)			(((((c)>(a))?(c):(a))>b)?(b):(((c)>(a))?(c):(a)))
#define TIME_UP(a)			    (a==0)

//----------------------------------------------------------------------------------

#define ZERO_FREQUENCY(a)	    ((a)<=100)						 //频率小于1HZ
#define ZERO_VOLTAGE(a)		    ((a)<= (System_Info.Zero_V_Max)) //电压小于10V

#define PCS_SOFT_VOLTAGE_OK(a)  (((a)>=100 )&&((a)<=500 ))    //PCS缓启电压正常 30

#define PCS_VOLTAGE_OK(a)	    (((a)>=2800)&&((a)<=4500))  //PCS电压正常
#define PCS_FREQUENCY_OK(a)	    (((a)>=4700)&&((a)<=5150))  //PCS频率正常

#define MG_VOLTAGE_OK(a)	    (((a)>=2800)&&((a)<=4500))  //微网电压正常
#define MG_FREQUENCY_OK(a)	    (((a)>=4700)&&((a)<=5150))  //微网频率正常

#define GRID_VOLTAGE_OK(a)	    (((a)>=2800)&&((a)<=4500))  //电网电压正常
#define GRID_FREQUENCY_OK(a)	(((a)>=4700)&&((a)<=5150))  //电网频率正常

#define FUNCTION_ENABLE(a)	                ((a)==0xAA)	   //使能
#define PCS_INVERT_STATUS(a)		        (((a)&(1<<13))==(uint32_t)1<<13)
#define PCS_EMG_STATUS(a)                   (((a)&(1<<1 ))==(uint32_t)1<<1 )

#define PCS_CONST_I_CHARGE_STATUS(a)	    (((a)&(1<<3))==(uint32_t)1<<3)
#define PCS_CONST_I_DISCHARGE_STATUS(a)	    (((a)&(1<<8))==(uint32_t)1<<8)

#define PCS_CONST_V_CHARGE_STATUS(a)	    (((a)&(1<<4))==(uint32_t)1<<4)
#define PCS_CONST_V_DISCHARGE_STATUS(a)	    (((a)&(1<<9))==(uint32_t)1<<9)

#define PCS_CONST_P_DC_CHARGE_STATUS(a)	    (((a)&(1<<5 ))==(uint32_t)1<<5 )
#define PCS_CONST_P_DC_DISCHARGE_STATUS(a)	(((a)&(1<<10))==(uint32_t)1<<10)

#define PCS_CONST_P_AC_CHARGE_STATUS(a)	    (((a)&(1<<6 ))==(uint32_t)1<<6 )
#define PCS_CONST_P_AC_DISCHARGE_STATUS(a)	(((a)&(1<<11))==(uint32_t)1<<11)

#define PCS_STANDBY_STATUS(a)		        (((a)&(1<<2))==(uint32_t)1<<2)

#define PCS_GRID_CONNECTED_STATUS(a)		((((a)&(1<<3))==(uint32_t)1<<3)   ||    \
											 (((a)&(1<<4))==(uint32_t)1<<4)   ||    \
											 (((a)&(1<<5))==(uint32_t)1<<5)   ||    \
											 (((a)&(1<<6))==(uint32_t)1<<6)   ||    \
											 (((a)&(1<<8))==(uint32_t)1<<8)   ||    \
											 (((a)&(1<<9))==(uint32_t)1<<9)   ||    \
											 (((a)&(1<<10))==(uint32_t)1<<10) ||    \
											 (((a)&(1<<11))==(uint32_t)1<<11) ||    \
											 (((a)&(1<<2))==(uint32_t)1<<2))

#define PCS_RUNNING(a)			       (((a&(1<<2))==(uint32_t)1<<2)   || \
                                        ((a&(1<<3))==(uint32_t)1<<3)   || \
										((a&(1<<4))==(uint32_t)1<<4)   || \
										((a&(1<<5))==(uint32_t)1<<5)   || \
										((a&(1<<6))==(uint32_t)1<<6)   || \
										((a&(1<<8))==(uint32_t)1<<8)   || \
										((a&(1<<9))==(uint32_t)1<<9)   || \
										((a&(1<<10))==(uint32_t)1<<10) || \
										((a&(1<<11))==(uint32_t)1<<11) || \
										((a&(1<<13))==(uint32_t)1<<13))

#define PCS_RUNNING_NOT_STANDBY(a)	   (((a&(1<<3))==(uint32_t)1<<3)   || \
										((a&(1<<4))==(uint32_t)1<<4)   || \
										((a&(1<<5))==(uint32_t)1<<5)   || \
										((a&(1<<6))==(uint32_t)1<<6)   || \
										((a&(1<<8))==(uint32_t)1<<8)   || \
										((a&(1<<9))==(uint32_t)1<<9)   || \
										((a&(1<<10))==(uint32_t)1<<10) || \
										((a&(1<<11))==(uint32_t)1<<11) || \
										((a&(1<<13))==(uint32_t)1<<13))

//#define PCS_FAULT_STATUS(a) 	((a == (uint32_t)1<<16))

//#define PCS_FAULT(a)		(((PCS_Info[a].PCS3x.RCommunication.Fault_Status1&0xFFFFFD87)!=0)&&(PCS_Info[a].PCS3x.RCommunication.Fault_Status2!=0))

#define PV_POWER_ON(a)			((a)==0xCF)	

#define BMS_STATUS_RIGHT(a)		((a) != 0xAAAA) 

#define BMS_FAULT_68AH(a)       ((BMS_Info[a].Rack_Fault_Word != 0)) 

#define BMS_ALARM_68AH(a)       ((BMS_Info[a].Rack_Alarm_Word!= 0)||(BMS_Info[a].Sys_Alarm_Word != 0))

#define BMS_FAULT_94AH(a)       ((BMS_Info[a].Protection1!=0)||(BMS_Info[a].Protection2!=0)||(BMS_Info[a].Protection3!=0)||(BMS_Info[a].Protection4!=0))

#define BMS_ALARM_94AH(a)       ((BMS_Info[a].Alarm1!=0)||(BMS_Info[a].Alarm2!=0)||(BMS_Info[a].Alarm3!=0)||(BMS_Info[a].Alarm4!=0)||(BMS_Info[a].Sys_Alarm_Status!=0))

//ENUM--------------------------------------------------------------------------------------------------------------------------

enum INPUT_PIN_FUNC
{
	NODEF_PIN=0,
	ALARM_PIN,
	FAULT_PIN,
};

enum TIME_DIV_Type
{
    TIME_DIV_DISABLE=0,
	TIME_DIV_BY_HOUR,	
	TIME_DIV_BY_WEEK,	
};

enum BMS_PV_SWITCH_Type
{
	BMS_TURN=0,	
	PV_TURN,
	DCDC_TURN,	
    PC_TURN,
};
enum BMS_Type
{
	BMS_None=0,
	BMS_68AH,
	BMS_94AH
};
enum PCS_COMM_Type
{
	PCS_COMM_SERIAL=0,
	PCS_COMM_LAN
};
enum PV_COMM_Type
{
	PV_COMM_SERIAL=0,
	PV_COMM_LAN
};
enum DEVICE_T
{
    SC30 = 0, 
    SC50, 
    SC100,
    SC250,
    SC500,
    SC500TL, 
	SC1000TL,
    PMD_AI100k,
    DEVICE_NUM
};

enum PCS_Power_
{
	PCS_Power_SC30_50=0,
	PCS_Power_SC100,
    PCS_Power_SC250,    
	PCS_Power_SC500,
	PCS_Power_SC500TL,
	PCS_Power_SC1000,
	PCS_Power_Max
};

enum DEVICE_INFO
{
    DEVICE_NAME=0,
    DEVICE_VERSION,
    FIRMWARE_VEB1,   //LCD
    FIRMWARE_VEB2,   //TOUCH
    FIRMWARE_VEB3,   //DSP
    WEB_VER
};

enum FEQ_T
{
     F_50Hz = 0,
     F_60Hz, 
     FEQ_NUM
};

enum E_CRC_TYPE_T
{
    E_CRC8  = 1,
    E_CRC16 = 2
};

enum svr_states
{
    SS_NONE = 0,
    SS_ACCEPTED,
    SS_RECEIVED,
    SS_CLOSING
};

enum S_BMS_WORK_STATUS_T //BMS工作状态
{
	BMS_WORK_OK = 0xbbbb,   //工作正常
	BMS_CHARGE_FULL = 0x1111,   //充电满
	BMS_DISCHARGE_EMPTY = 0x2222,   //放电空
	BMS_STANDBY = 0x5555,   //待机
	BMS_STOP = 0xaaaa    //停机
};

//tcp client
enum tcp_client_state
{
    CLIENT_CONNECTED = 0x00U,
    CLIENT_WAITING_FOR_CMD,
    CLIENT_BUSY,
    CLIENT_SENT,
    CLIENT_ERROR,
    CLIENT_CLOSE,
    CLIENT_WAITING_FOR_CONNECTION
};

struct tcp_client_app_arg
{
	uint8_t app_state;
	uint8_t textlen;
	uint8_t* dataptr;
	uint8_t retries;
};

//TYPEDEF-----------------------------------------------------------------------------------------------------------------------
//Add by Lihd.15-11-17
//分时电价
enum E_PEAK_LOW_STATUS_T    //峰谷状态
{
    ELEC_PEAK_STATE     = 0x55,
	ELEC_NORMAL_STATE	= 0xAA,
    ELEC_LOW_STATE     	= 0xFF,
    ELEC_INVALID        = 0x00
};

typedef struct
{
	uint8_t hour;
	uint8_t min;
} S_Time;

typedef struct
{
	S_Time Start_time;
	S_Time End_time;
	uint8_t Week;                //星期
	//uint8_t Elec_state;
	//uint16_t Price;
	uint8_t Run_Mode;            //PCS运行模式 0-并网 1-离网 2-并离网切换
	uint8_t Power_Ctrl_Mode;     //PCS并网运行模式 0-固定功率 1-跟踪负载 2-PCC点功率控制 4-防逆流 10-消峰填谷
	int16_t Run_Power;
	//int16_t PC_Set_Power;
	int16_t Mgrid_Run_Power;

	//峰谷电价
	int16_t Start_Charge_Power;     //谷值
	int16_t Start_Discharge_Power;  //峰值
	uint16_t Update_Cycle;    //削锋填谷更新周期  ms

	//新能源平滑
	uint32_t PV_Rated_Power;
	uint16_t Ramp_ratio;
	uint16_t Ramp_time;
	uint16_t SOC_Uplimit;
	uint16_t SOC_Lowlimit;
	uint16_t Vbat_Uplimit;
	uint16_t Vbat_Lowlimit;
	int16_t  PCS_Autocharge_Power;

	//调频
	//模拟量数字量选择
	uint16_t FM_Analog_Digital;
	uint16_t FM_Analog_Vmax;
	uint16_t FM_Analog_Vmin;

} S_Time_Div_Set_Info_T;

typedef struct
{
	double Charge_Energy_Acc;
	double Discharge_Energy_Acc;
	double Day_Charge_Energy_Acc;
	double Day_Discharge_Energy_Acc;
	double Mon_Charge_Energy_Acc;
	double Mon_Discharge_Energy_Acc;
	double Year_Charge_Energy_Acc;
	double Year_Discharge_Energy_Acc;

	double Charge_Money_Acc;
	double Discharge_Money_Acc;
	double Day_Charge_Money_Acc;
	double Day_Discharge_Money_Acc;
	double Mon_Charge_Money_Acc;
	double Mon_Discharge_Money_Acc;
	double Year_Charge_Money_Acc;
	double Year_Discharge_Money_Acc;

	uint32_t Day_Save_Money;
	uint32_t Mon_Save_Money;
	uint32_t Year_Save_Money;
	uint32_t Total_Save_Money;

	S_Time_Div_Set_Info_T Time_Div_Set_Info[11];
	uint16_t Period;     //当前所处的时间段位置

} S_Time_Div_Info_T;
//End.

//IAT协议 ccg 2013.5.15
typedef struct     //S_IAT_Set_Info_T
{
	uint16_t SN_Enable; 			//SN设备序列号使能
	char SN_Number[20];			//SN设备序列号
	uint16_t MAC[3];				//MAC地址
	uint16_t MAC2[3];				//MAC地址
	uint16_t MAC_Lock;				//MAC地址锁定

} S_IAT_Set_Info_T;
typedef struct				//S_IAT_Info_T
{
	S_IAT_Set_Info_T Set_Info;
} S_IAT_Info_T;
typedef struct				//S_Touch_Data_T
{
	//MT8000触摸屏相关
	uint8_t Touch_Charge_Power_Curve[148];             			//主界面充电功率曲线    74*(2字节)
	uint8_t Touch_Discharge_Power_Curve[148];          			//主界面放电功率曲线    74*(2字节)
	uint16_t Touch_Charge_Power_Curve_Temp[74];  	    			//充电曲线
	uint16_t Touch_Discharge_Power_Curve_Temp[74]; 	    			//放电曲线

	uint8_t Touch_Elec_Column[100];		            			//电量柱           (24+24)*2  +4(保留)

	uint32_t Touch_Day_Charge_Elec_Column[VALID_HOUR_NUM];		    //1天内每小时充电量  4*24=96
	uint32_t Touch_Day_Discharge_Elec_Column[VALID_HOUR_NUM];       //1天内每小时放电量  4*24=96

	uint32_t Touch_Month_Charge_Elec_Column[VALID_DAY_NUM];		    //1月内每日充电量    4*31=124
	uint32_t Touch_Month_Discharge_Elec_Column[VALID_DAY_NUM];      //1月内每日放电量    4*31=124

	uint32_t Touch_Year_Charge_Elec_Column[VALID_MONTH_NUM];		//1年内每月充电量    4*12=48
	uint32_t Touch_Year_Discharge_Elec_Column[VALID_MONTH_NUM];     //1年内每月放电量    4*12=48

	uint32_t Touch_Total_Charge_Elec_Column[VALID_YEAR_NUM];		//20年内每年充电量   4*20=80
	uint32_t Touch_Total_Discharge_Elec_Column[VALID_YEAR_NUM];     //20年内每年放电量   4*20=80

	uint8_t Touch_History_Event[86];                   			//历史事件  (1+1+1+8*5) 43*2
	uint8_t Touch_History_Fault[96];			        			//历史故障  (1+1+1+9*5) 48*2  故障记录保留了一个字节，不知当初触摸屏怎么想的~~
} S_Touch_Data_T;
typedef struct			        			//gt_time
{
	uint8_t tm_year;
	uint8_t tm_mon;
	uint8_t tm_day;
	uint8_t tm_hour;
	uint8_t tm_min;
	uint8_t tm_sec;
} gt_time;
typedef struct			        			//Range
{
	int16_t Min;
	int16_t Max;
	int16_t Dft;
} Range;
typedef struct			        			//Param_Range
{
	Range PCS_Total_Num;
	Range PV_Total_Num;
	Range BMS_Total_Num;
	Range DCDC_Total_Num;
	Range AMMETER_Total_Num;
	Range PVS_Total_Num;

	Range SOC_Uplimit2;
	Range SOC_Uplimit1;
	Range SOC_Lowlimit1;
	Range SOC_Lowlimit2;

	Range SOC_Lowlimit1_Recover;
	Range SOC_Lowlimit2_Recover;
	Range SOC_Control_Delay;
	Range SOC_Control_Recover_Delay;

	Range VBAT_Uplimit2;
	Range VBAT_Uplimit1;
	Range VBAT_Lowlimit1;
	Range VBAT_Lowlimit2;

	Range VBAT_Lowlimit1_Recover;
	Range VBAT_Lowlimit2_Recover;
	Range VBAT_Control_Delay;
	Range VBAT_Control_Recover_Delay;

	Range MGrid_Voltage;
	Range MGrid_Frequency;
	Range Const_V_Mode_V;
	Range Const_V_Mode_I;

	Range Power_Factor;
	Range Qvar_Power_Ratio;

} Param_Range;
//com_IO_DSP.h

//S_Com_Info_T;
typedef struct
{
	uint8_t PCS_Protocol_Type;           //0--新协议；1--旧协议
	uint8_t Bus_Type;                    //母线类型:0-直流母线；1-交流母线
	uint8_t EMS_Run_Priority;            //运行模式 0-电网优先；1-微网优先
	uint8_t EMS_Start_Type;              //启动类型 0-主动模式；1-被动模式
	uint8_t DC_Bus_Charge_Permit;        //直流母线PCS是否允许从电网充电
	uint16_t System_Power_Set;
	uint8_t PCS_Type;                  	 //PCS类型选择
	uint8_t DCDC_Type;                   //DCDC类型选择

	uint16_t Time_Divide_Function;        //分时功率控制，0-关闭 1- 按照小时控制 2-按照周几控制
	uint8_t BMS_Type;                     // 0-无 1-68ah 2-94ah
	uint8_t PCS_Comm_Type;                //PCS通讯方式选择 0-串口 ； 1-网口
	uint8_t PCS_Grid_Run_Mode;            //PCS并网工作模式选择

	uint8_t Power_Distribution_Enable;    //配电功能使能禁止
	uint8_t Meter_Comm_Enable;            //电表通讯使能禁止
	uint8_t Balancing_Enable;             //均衡使能控制
	uint8_t PCS_Island_Ctrl_Mode;         // PCS离网启动运行策略，0:全部正常，1:部分正常

	uint8_t Ammeter_Protocol_Type;        //电表协议类型  0 -- MODBUS 1 -- DL/T645
	uint8_t PV_Comm_Type;                 //PV通讯方式选择 0-串口 ； 1-网口

	uint8_t InputPin_Function[8];         //输入引脚功能定义，0:无定义 1：告警 2：故障
	uint8_t InputPin_NC_NO[8];

	uint8_t Meter_Position_Enable[8];     //电表位置使能设置

} S_CONFIG_T;

typedef struct
{
	uint8_t ip[4];           // IP地址
	uint8_t submask[4];      // IP子网掩码
	uint8_t gateway[4];      // 网关
	uint8_t dns1[4];         // DNS1
	uint8_t dns2[4];         // DNS2
	uint8_t mac[6];          // MAC地址
	uint8_t dhcp_flag; /* DHCP标志 */
} NetParam;

typedef struct
{
	uint8_t ip[4];           // 服务IP地址
	uint16_t port;			 // 服务端口
	uint8_t enable;			 // ntp服务启动
}NTP_Param;

typedef struct
{
	uint8_t First_Start;                   //IO板程序第一次启动的标识，0xAA表示已经不是第一次启动
	uint8_t Baud_Rate;                     //与DSP通信所使用的波特率
	uint8_t Modbus_Address;                //上位机的modbus地址
	//uint8_t  BMS_Address;                	//BMS的modbus地址

	uint8_t EMS_BMS_CAN_Com_Count[BMS_NUM];        //BMS通讯正常计数器
	uint8_t EMS_MONITOR_Web_Http_Com_Count;      	//webserver通讯正常计数器(HTTP)
	uint8_t EMS_MONITOR_Modbus_TCP_Com_Count[TCP_MODBUS_LINK_NUM];      	//MODBUS通讯正常计数器
	uint8_t EMS_MONITOR_IEC104_TCP_Com_Count[TCP_IEC104_LINK_NUM];      	//IEC104通讯正常计数器
	uint8_t EMS_PCS_Com_Count[PCS_NUM];      	//PCS通讯正常计数器
	uint8_t EMS_PV_Com_Count[PV_NUM];       	//PV通讯正常计数器
	uint8_t EMS_PVS_Com_Count[PVS_NUM];       	//PVS通讯正常计数器  2016.9.17 lk
	uint8_t EMS_DCDC_Com_Count[DCDC_NUM];      //DCDC通讯正常计数器
	uint8_t EMS_Meter_Com_Count[Meter_NUM];    //DCDC通讯正常计数器
	uint8_t EMS_DSP_Com_Count[DSP_NUM];        //DSP通讯正常计数器 // caoxh
	uint8_t EMS_ENV_Com_Count[ENV_NUM];			//环境检测仪通讯正常计数器  2017.7.19 lk

	//uint8_t  Device_Type;                     //设备类型
	uint8_t IO_To_BMS_Mode;

	NetParam Net;                               //EMS网络参数
	NetParam Net2;
	uint16_t Modbus_Port;
	uint16_t Webserver_Port;
	NTP_Param	NTP;

	NetParam Server[PCS_NUM];                   //PCS网络参数
	uint16_t Server_Modbus_Port[PCS_NUM];
	NetParam PVServer[PV_NUM];                  //PV网络参数
	uint16_t PVServer_Modbus_Port[PV_NUM];

	uint16_t IEC104_Port;
	uint16_t IEC104_Address;				//104参数
	uint16_t IEC104_T1;
	uint16_t IEC104_T3;
	uint16_t IEC104_K;
	uint16_t IEC104_W;

	uint8_t Reset_Time_Flag;				//请求触摸屏向计量板对时
	//uint8_t  BMS_Type;                    // 0-无 1-68ah 2-94ah

    S_CONFIG_T Config;
    uint16_t Zero_V_Max;                   //零电压最大值
	
}S_System_Info_T;

//EMS.h
//S_EMS_3x_Info_T
//EMS_Controler_T
typedef struct    //EMS_Equip_Property_T
{
	uint32_t Protocol_Number;
	uint32_t Protocol_Version;
	uint16_t Machine_Type[8];
	uint16_t Machine_Version[10];
	uint16_t Serial_Number[10];
} EMS_Equip_Property_T;

typedef struct    //EMS_RMeasure_T
{
	uint32_t Equip_TypeCode;
	uint32_t PCC_Pos_Active_Energy;
	uint32_t PCC_Pos_Reactive_Energy;
	uint32_t PCC_Neg_Active_Energy;
	uint32_t PCC_Neg_Reactive_Energy;
	uint32_t PV_Total_Active_Energy;
	uint32_t PV_Total_Reactive_Energy;
	uint32_t PCS_Total_Charge_Energy;
	uint32_t PCS_Total_Discharge_Energy;
	uint32_t PCS_Total_Reactive_Energy;
	uint32_t Load_Total_Active_Energy;
	uint32_t Load_Total_Reactive_Enerty;
	uint32_t Diesel_Total_Active_Energy;
	uint32_t Diesel_Total_Reactive_Energy;	
    uint32_t DCDC_Total_Charge_Energy;
    uint32_t DCDC_Total_Discharge_Energy;
	
}EMS_RMeasure_T;

typedef struct    //EMS_RCommunication_T
{
	uint32_t Total_Status;
	//uint32_t Run_Mode;
	uint32_t Fault_Word1;
	uint32_t Fault_Word2;
	uint32_t Alarm_Word;
	uint32_t Equip_Fault;
	uint32_t Contactor_Fault;
	uint32_t PCS_Comm_Fault;
	uint32_t PV_Comm_Fault;
	uint16_t InputPin_Status;
} EMS_RCommunication_T;

typedef struct    //EMS_Controller_3x_Info_T
{
	EMS_Equip_Property_T Equip_Property;
	EMS_RMeasure_T RMeasure;
	EMS_RCommunication_T RCommunication;
} EMS_Controller_3x_Info_T;

//AGC_3x_Info_T
typedef struct    //AGC_RMeasure_T
{
	uint16_t PCS_Online_Num;
	uint16_t PV_Online_Num;
	uint16_t DCDC_Online_Num;
	uint16_t BMS_Online_Num;
	uint16_t Load_Online_Num;
	uint16_t Diesel_Online_Num;
	uint16_t PCS_Useable_Propertion;
	uint16_t PV_Useable_Propertion;
	uint16_t Diesel_Useable_Propertion;
	int16_t  PCC_Active_Power;
	int16_t  PCC_Reactive_Power;
	int16_t  PCC_Powerfactor;
	uint16_t PCC_Voltage;
	uint16_t PCC_Frequency;
	uint16_t MG_Voltage;
	uint16_t MG_Frequency;
	uint16_t Load_Voltage;
	uint16_t Load_Frequency;
	uint16_t PCS_charge_Power;
	uint16_t PCS_discharge_Power;
	int16_t  PCS_charge_Power_Max;
	int16_t  PCS_discharge_Power_Max;

	//----------------------------------------------//
	int32_t  PCS_Active_Power;
	int32_t  PCS_Reactive_Power;
	int16_t  PCS_Powerfactor;

	uint32_t  PV_Active_Power;
	int32_t  PV_Reactive_Power;
	int16_t  PV_Powerfactor;

	int32_t  Load_Active_Power;
	int32_t  Load_Reactive_Power;
	int16_t  Load_Powerfactor;

	uint32_t Diesel_Active_Power;
	int32_t  Diesel_Reactive_Power;
	int16_t  Diesel_Powerfactor;

	int32_t  Grid_Active_Power;
	int32_t  Grid_Reactive_Power;
	int16_t  Grid_Powerfactor;

	uint32_t  DCDC_Active_Power;
	
}AGC_RMeasure_T;

typedef struct    //AGC_RCommunication_T
{
	uint32_t Total_Status;
	uint32_t Total_Switch_Status;
	uint32_t Total_Equip_ONOFF_Status;
	uint32_t PCS_Individual_ONOFF_Status;
	uint32_t PV_Individual_ONOFF_Status;
	uint32_t Load_Individual_ONOFF_Status;
	uint32_t Diesel_Indidual_ONOFF_Status;
} AGC_RCommunication_T;
typedef struct    //AGC_3x_Info_T
{
	AGC_RMeasure_T RMeasure;
	AGC_RCommunication_T RCommunication;
} AGC_3x_Info_T;

//SOC_3x_Info_T
typedef struct    //SOC_RMeasure_T
{
	uint16_t Bat_SOC_Aver;
	uint16_t Bat_SOC_Max;
	uint16_t Bat_SOC_Min;
	uint16_t Bat_Voltage_Aver;
	uint16_t Bat_Voltage_Max;
	uint16_t Bat_Voltage_Min;
	uint16_t Bat_ChargeI_Aver;

	uint16_t Bat_ChargeI_Max;
	uint16_t Bat_ChargeI_Min;
	uint16_t Bat_DischargeI_Aver;
	uint16_t Bat_DischargeI_Max;
	uint16_t Bat_DischargeI_Min;

	uint16_t PCS_Active_Power_Aver;
	uint16_t PCS_Active_Power_Max;
	uint16_t PCS_Active_Power_Min;

	uint16_t PCS_AC_Voltage_Aver;
	uint16_t PCS_AC_Voltage_Max;
	uint16_t PCS_AC_Voltage_Min;
	uint16_t PV_DC_V_Aver;
	uint16_t PV_DC_V_Max;
	uint16_t PV_DC_V_Min;

	uint16_t Bat_SOC_Aver_Rrc;
	uint16_t Bat_SOC_Max_Rrc;
	uint16_t Bat_SOC_Min_Rrc;
	uint16_t Bat_Voltage_Aver_Rrc;
	uint16_t Bat_Voltage_Max_Rrc;
	uint16_t Bat_Voltage_Min_Rrc;

} SOC_RMeasure_T;
typedef struct    //SOC_RCommunication_T
{
	uint32_t Total_Status;
} SOC_RCommunication_T;
typedef struct    //SOC_Info_T
{
	SOC_RMeasure_T RMeasure;
	SOC_RCommunication_T RCommunication;
} SOC_3x_Info_T;

//PV_Power_Forecast_3x_Info_T
typedef struct    //PV_Power_Forecast_RMeasure_T
{
	uint16_t Light_Intensity;
	int16_t Envi_Temperatrue;
	int16_t Solar_Panel_Temperature;
	uint16_t Wind_Speed;
	uint16_t Wind_Direction;
	uint16_t Forcast_Active_Power;
} PV_Power_Forcast_RMeasure_T;
typedef struct    //PV_Power_Forcast_RCommunication_T
{
	uint16_t Total_Status;
} PV_Power_Forcast_RCommunication_T;
typedef struct    //PV_Power_Forcast_3x_Info_T
{
	PV_Power_Forcast_RMeasure_T RMeasure;
	PV_Power_Forcast_RCommunication_T RCommunication;
} PV_Power_Forcast_3x_Info_T;

//Load_Forcast_3x_Info_T
typedef struct    //S_Load_Forcast_RMeasure_T
{
	uint16_t Load_Forcast_Active_Power;
} Load_Forcast_RMeasure_T;
typedef struct    //Load_Forcast_RCommunication_T
{
	uint32_t Total_Status;
} Load_Forcast_RCommunication_T;
typedef struct    //Load_Forcast_Info_T
{
	Load_Forcast_RMeasure_T RMeasure;
	Load_Forcast_RCommunication_T RCommunication;
} Load_Forcast_3x_Info_T;

//Grid_Connected_Start_3x_Info_T
typedef struct    //Grid_Connected_Start_RCommunication_T
{
	uint32_t Status;
	uint32_t Fault_Status;
	uint32_t Alarm_Status;
} Grid_Connected_Start_RCommunication_T;
typedef struct    //Grid_Connected_Start_3x_Info_T
{
	Grid_Connected_Start_RCommunication_T RCommunication;
} Grid_Connected_Start_3x_Info_T;

//Island_Start_Info_T
typedef struct    //Island_Start_RCommunication_T
{
	uint32_t Status;
	uint32_t Fault_Status;
	uint32_t Alarm_Status;
} Island_Start_RCommunication_T;
typedef struct    //Island_Start_3x_Info_T
{
	Island_Start_RCommunication_T RCommunication;
} Island_Start_3x_Info_T;

//Load_Control_3x_Info_T
typedef struct    //Load_Control_RMeasure_T
{
	uint16_t AC_Voltage;
	uint16_t AC_Current;
	uint16_t AC_Frequency;
	int16_t Active_Power;
	int16_t Reactive_Power;
	int16_t Powerfactor;
	uint32_t Day_Active_Energy;
	uint32_t Day_Reactive_Energy;
	uint32_t Total_Active_Energy;
	uint32_t Total_Reactive_Energy;
} Load_Control_RMeasure_T;
typedef struct    //Load_Control_RCommunication_T
{
	uint32_t Status;
	uint32_t Switch_Status;
} Load_Control_RCommunication_T;
typedef struct    //Load_Control_Info_T
{
	Load_Control_RMeasure_T RMeasure;
	Load_Control_RCommunication_T RCommunication;
} Load_Control_3x_Info_T;

typedef struct    //EMS_3x_T
{
	EMS_Controller_3x_Info_T EMS_Controller_Info;
	AGC_3x_Info_T AGC_Info;
	SOC_3x_Info_T SOC_Info;
	PV_Power_Forcast_3x_Info_T PV_Power_Forcast_Info;
	Load_Forcast_3x_Info_T Load_Forcast_Info;
	Grid_Connected_Start_3x_Info_T Grid_Connected_Start_Info;
	Island_Start_3x_Info_T Island_Start_Info;
	Load_Control_3x_Info_T Load_Control_Info[Load_NUM];
} EMS_3x_T;
//EMS_4x_Info_T
//EMS_Controller_4x_Info_T
typedef struct    //IP_Address_T
{
	uint8_t IP[4];
	uint8_t Submask[4];
	uint8_t Gateway[4];
	uint8_t DHCP_Switch;
	uint8_t DNS1[4];
	uint8_t DNS2[4];
} IP_Address_T;
typedef struct    //IP_Type_T
{
	IP_Address_T IP_Address;
	uint8_t Modubus_Address;
	uint16_t Port;
} IP_Type_T;
typedef struct    //EMS_Equip_Address_T
{
	IP_Type_T PCS_Address[PCS_NUM];
	IP_Type_T PV_Address[PV_NUM];
	IP_Type_T BMS_Address[BMS_NUM];
	IP_Type_T DCDC_Address[DCDC_NUM];
	IP_Type_T Meter_Address[Meter_NUM];
	IP_Type_T PVS_Address[PVS_NUM];
	IP_Type_T ENV_Address[ENV_NUM];
	IP_Type_T EM_Address;
} EMS_Equip_Address_T;
typedef struct    //EMS_Equip_Baudrate_T
{
	uint8_t PCS_Baud;
	uint8_t PV_Baud;
	uint8_t DCDC_Baud;
	uint8_t BMS_Baud;
	uint8_t AMMETER_Baud;
} EMS_Equip_Baudrate_T;
typedef struct //IEC104_Parameter_T
{
	uint16_t Address;
	uint16_t T1;
	uint16_t T3;
	uint16_t k;
	uint16_t w;
} IEC104_Parameter_T;
typedef struct //EMS_RControl_T
{
	uint32_t Work_Mode;
	uint32_t Run_Mode;
} EMS_RControl_T;
typedef struct //EMS_RAdjust_T
{
	uint16_t SysTime_Year;
	uint16_t SysTime_Month;
	uint16_t SysTime_Date;
	uint16_t SysTime_Hour;
	uint16_t SysTime_Min;
	uint16_t SysTime_Sec;
} EMS_RAdjust_T;
typedef struct //EMS_EM_T;
{
	int16_t PCC_Active_Power_Giving;
	int16_t PCC_Reactive_Power_Giving;

	uint16_t Ind_Bat_Float_V;
	uint16_t Ind_Bat_Protect_V;
	uint16_t Ind_Bat_Cap;
	uint16_t Bat_Parallel_Num;
	uint16_t Bat_Ind_Seri_Num;
	uint16_t Const_V_Mode_I;		//恒压充电限制电流值
} EMS_EM_T;
typedef struct		//EMS_Controller_4x_Info_T
{
	EMS_Equip_Address_T Equip_Address;
	EMS_Equip_Baudrate_T Baudrate;
	IEC104_Parameter_T IEC104_Para;
	EMS_RControl_T RControl;
	EMS_RAdjust_T RAdjust;
	EMS_EM_T EM;
} EMS_Controller_4x_Info_T;

//AGC_4x_Info_T
typedef struct		//AGC_RControl_T
{
	uint32_t Total_Switch_Control;
	uint32_t Equip_Total_ONOFF_Control;
	uint32_t PCS_Individual_ONOFF_Control;
	uint32_t PV_Individual_ONOFF_Control;
	uint32_t Load_Individual_ONOFF_Control;
	uint32_t Diesel_Individual_ONOFF_Control;
} AGC_RControl_T;

typedef struct		//AGC_SysParameter_T
{
	uint16_t MG_Voltage;
	uint16_t MG_Frequency;
	uint16_t Const_V_Mode_V;
	uint16_t Const_V_Mode_I;

	uint16_t Const_V_Mode_V_Min;
	uint16_t Const_V_Mode_V_Max;

	uint16_t Const_V_Mode_I_Max;
	uint16_t Total_PV_Number;
	uint16_t Total_PCS_Number;
	uint16_t Total_BMS_Number;
	uint16_t Total_DCDC_Number;
	uint16_t Total_Meter_Number;
	uint16_t Total_PVS_Number;
	uint16_t Total_LoadController_Number;
	uint16_t Total_Diesel_Number;
	uint16_t Total_ENV_Number;

	uint32_t PCS_Rated_Output_Power;
	uint32_t PV_Rated_Output_Power;
	uint32_t Total_Solar_Panel_Power;

	uint16_t Qvar_Adj_Mode;
	int16_t Power_Factor;
	int16_t EMS_Run_Q_Set;
	int16_t EMS_Run_Power_Set;

	uint16_t PCS_Net_SetPara_Delay;
	uint16_t PCS_Net_Set_Cycle;

} AGC_SysParameter_T;

typedef struct		//AGC_ProParameter_T
{
	uint16_t MG_Voltage_Uplimit;
	uint16_t MG_Voltage_Lowlimit;
	uint16_t MG_Frequency_Uplimit;
	uint16_t MG_Frequency_Lowlimit;
} AGC_ProParameter_T;
typedef struct		//AGC_4x_Info_T
{
	AGC_RControl_T RControl;
	AGC_SysParameter_T SysParameter;
	AGC_ProParameter_T ProParameter;
} AGC_4x_Info_T;

//SOC_4x_Info_T
typedef struct		//S_SOC_SysParameter_T
{
	uint16_t BMS_Configered;
} SOC_SysParameter_T;
typedef struct		//SOC_RunParameter_T
{
	uint16_t SOC_Control_Enable;
	uint16_t SOC_Uplimit2;
	uint16_t SOC_Uplimit1;
	uint16_t SOC_Lowlimit1;
	uint16_t SOC_Lowlimit2;
	uint16_t SOC_Lowlimit1_Recover;
	uint16_t SOC_Lowlimit2_Recover;
	uint16_t SOC_Float_Hysteresis;
	uint16_t SOC_Control_Delay;
	uint16_t SOC_Control_Recover_Delay;

	uint16_t SOC_Control_Delay_S;
	uint16_t SOC_Control_Recover_Delay_S;

	uint16_t VBAT_Control_Enable;
	uint16_t VBAT_Uplimit2;
	uint16_t VBAT_Uplimit1;
	uint16_t VBAT_Lowlimit1;
	uint16_t VBAT_Lowlimit2;
	uint16_t VBAT_Lowlimit1_Recover;
	uint16_t VBAT_Lowlimit2_Recover;
	uint16_t VBAT_Float_Hysteresis;
	uint16_t VBAT_Control_Delay;
	uint16_t VBAT_Control_Recover_Delay;

	uint16_t VBAT_Control_Delay_S;
	uint16_t VBAT_Control_Recover_Delay_S;

    uint8_t  Low_Battery_Output1_Mode;
    uint8_t  Low_Battery_Output2_Mode;
	
}SOC_RunParameter_T;
typedef struct//SOC_ProParameter_T
{
	uint16_t SOC_Uplimit;
	uint16_t SOC_Lowlimit;
	uint16_t Bat_OverVoltage_Value;
	uint16_t Bat_UnderVoltage_Value;
	uint16_t Bat_OverTemper_Value;
	uint16_t Bat_UnderTemper_Value;
} SOC_ProParameter_T;
typedef struct		//SOC_4x_Info_T
{
	SOC_SysParameter_T SysParameter;
	SOC_RunParameter_T RunParameter;
	SOC_ProParameter_T ProParameter;
} SOC_4x_Info_T;

//PV_Power_Forcast_4x_Info_T
typedef struct		//PV_Power_Forcast_SysParameter_T
{
	uint16_t Light_Coefficient;
	uint16_t SelfLearn_Light_Coefficient;
} PV_Power_Forcast_SysParameter_T;
typedef struct		//PV_Power_Forcast_RunParameter_T;
{
	uint16_t SelfLearn_Enable;
	uint16_t Start_Light_Intensity;
	uint16_t Max_Light_Intensity;
	uint16_t Light_Intensity_Set;
	uint16_t Forcast_PV_Power_Lowlimit_Set;
} PV_Power_Forcast_RunParameter_T;
typedef struct		//PV_Power_Forcast_4x_Info_T
{
	PV_Power_Forcast_SysParameter_T SysParameter;
	PV_Power_Forcast_RunParameter_T RunParameter;
} PV_Power_Forcast_4x_Info_T;

//Grid_Connected_Start_4x_Info_T
typedef struct		//Grid_Connected_Start_ProParameter_T
{
	uint16_t BatSOC_Start_Lowlimit;
	uint16_t BatSOC_Start_Delay;
	uint16_t BatVoltage_Start_Lowlimit;
	uint16_t BatVoltage_Start_Delay;
} Grid_Connected_Start_ProParameter_T;
typedef struct		//Grid_Connected_Start_4x_Info_T
{
	Grid_Connected_Start_ProParameter_T ProParameter;

} Grid_Connected_Start_4x_Info_T;

//Island_Start_4x_Info_T
typedef struct		//Island_Start_ProParameter_T
{
	uint16_t BatSOC_Start_Lowlimit;
	uint16_t BatSOC_Start_Delay;
	uint16_t BatVoltage_Start_Lowlimit;
	uint16_t BatVoltage_Start_Delay;
} Island_Start_ProParameter_T;
typedef struct		//Island_Start_4x_Info_T
{
	Island_Start_ProParameter_T ProParameter;
} Island_Start_4x_Info_T;

//Bat_Balance_4x_Info_T
typedef struct		//Bat_Balance_RunParameter_T
{
	uint16_t Bat_Balance_Start_SOC_Deviation;
	uint16_t Bat_Balance_Stop_SOC_Deviation;
	uint16_t Bat_Balance_Start_Voltage_Deviation;
	uint16_t Bat_Balance_Stop_Voltage_Deviation;
} Bat_Balance_RunParameter_T;
typedef struct		//Bat_Banlace_4x_Info_T
{
	Bat_Balance_RunParameter_T RunParameter;
} Bat_Banlace_4x_Info_T;

//Load_Control_4x_Info_T
typedef struct		//Load_Control_RContol_T
{
	uint32_t Switch_Control;
} Load_Control_RContol_T;
typedef struct		//Load_Control_RunParameter_T
{
	uint16_t Run_Priority;
	uint16_t Dispatchable;
	uint16_t SwitchON_Delay;
	uint16_t SwitchOFF_Delay;
} Load_Control_RunParameter_T;
typedef struct		//Load_Control_4x_Info_T
{
	Load_Control_RContol_T RContol;
	Load_Control_RunParameter_T RunParameter;
} Load_Control_4x_Info_T;

typedef struct		//EMS_4x_T
{
	EMS_Controller_4x_Info_T EMS_Controller_Info;
	AGC_4x_Info_T AGC_Info;
	SOC_4x_Info_T SOC_Info;
	PV_Power_Forcast_4x_Info_T PV_Power_Forcast_Info;
	Grid_Connected_Start_4x_Info_T Grid_Connected_Start_Info;
	Island_Start_4x_Info_T Island_Start_Info;
	Bat_Banlace_4x_Info_T Bat_Banlace_Info;
	Load_Control_4x_Info_T Load_Control_Info[2];
} EMS_4x_T;

//S_EMS_Info_T
typedef struct		//S_EMS_Info_T
{
	EMS_3x_T EMS3x;
	EMS_4x_T EMS4x;
} S_EMS_Info_T;

//S_PCS_Info_T
//S_PCS_3x_Info_T
typedef struct		//PCS_Property_T
{
	uint32_t Protocol_Number;
	uint32_t Protocol_Version;
	uint16_t Mechine_Type[8];
	uint16_t Mechine_Version[10];
	uint16_t Serial_Number[10];
} PCS_Property_T;
typedef struct		//PCS_RRmeasure_T
{
	uint16_t Type_Code;
	uint16_t Rated_Output_Power;
	uint16_t Output_Type;
	uint16_t Charge_Status;		//0-其它；1-充电；2-放电。
	uint16_t Efficiency;
	uint16_t DC_Voltage;
	uint16_t DC_Current;
	uint32_t u32_DC_Power;
	uint16_t DC_Power;
	uint16_t VAB;
	uint16_t VBC;
	uint16_t VCA;
	uint16_t IA;
	uint16_t IB;
	uint16_t IC;
	uint32_t u32_Apparent_Power;
	uint16_t Apparent_Power;
	uint32_t u32_Active_Power;
	int16_t Active_Power;
	int32_t s32_Reactive_Power;
	int16_t Reactive_Power;
	int16_t Power_Factor;
	uint16_t Grid_Frequency;
	int16_t Inside_Temper;
	int16_t ModuleA1_Temper;
	int16_t ModuleB1_Temper;
	int16_t ModuleC1_Temper;
	int16_t ModuleA2_Temper;
	int16_t ModuleB2_Temper;
	int16_t ModuleC2_Temper;
	uint32_t Total_Run_Time;
	uint16_t Day_Charge_Energe;
	uint16_t Day_Discharge_Energe;
	uint16_t Day_Charge_Time;
	uint16_t Day_Discharge_Time;
	uint32_t Month_Charge_Energe;
	uint32_t Month_Discharge_Energe;
	uint32_t Month_Charge_Time;
	uint32_t Month_Discharge_Time;
	uint32_t Year_Charge_Energe;
	uint32_t Year_Discharge_Energe;
	uint32_t Year_Charge_Time;
	uint32_t Year_Discharge_Time;
	uint32_t Total_Charge_Energe;
	uint32_t Total_Discharge_Energe;
	uint32_t Total_Charge_Time;
	uint32_t Total_Discharge_Time;

	int16_t Const_I_Mode_I_Uplimit;
	int16_t Const_I_Mode_I_Lowlimit;
	uint16_t Const_V_Mode_V_Uplimit;
	uint16_t Const_V_Mode_V_Lowlimit;
	uint16_t Const_V_Mode_I_Uplimit;
	uint16_t Const_V_Mode_I_Lowlimit;
	int16_t Const_P_DC_Mode_P_Uplimit;
	int16_t Const_P_DC_Mode_P_Lowlimit;
	int16_t Const_P_AC_Mode_P_Uplimit;
	int16_t Const_P_AC_Mode_P_Lowlimit;

	uint16_t Ind_Inverter_F_Uplimit;
	uint16_t Ind_Inverter_F_Lowlimit;
	uint16_t Ind_Inverter_V_Uplimit;
	uint16_t Ind_Inverter_V_Lowlimit;

	int16_t Powerfactor_Uplimit;
	int16_t Powerfactor_Lowlimit;
	int16_t Reactive_Power_Uplimit;
	int16_t Reactive_Power_Lowlimit;

	uint16_t Charge_Times;
	uint16_t Discharge_Times;

	uint16_t Island_Mode;
	uint16_t Ractive_Mode;
	uint16_t Master_Slave;
} PCS_Rmeasure_T;
typedef struct		//PCS_RCommunication_T
{
	uint32_t Run_Status;
	uint32_t Fault_Status1;
	uint32_t Fault_Status2;
} PCS_RCommunication_T;
typedef struct //PCS_Battery_T
{
	uint16_t Bat_Status;
	uint16_t Bat_Fault;
	uint16_t Bat_Alarm;
	uint16_t Bat_SOC;
	uint16_t Bat_Chargeable_SOC;
	uint16_t Bat_Dischargeable_SOC;
	uint16_t Bat_Total_Voltage;
	uint16_t Bat_Total_Current;
	uint16_t Bat_Single_SOC_Max;
	uint16_t Bat_Single_SOC_Min;
	uint16_t Bat_Single_V_Max;
	uint16_t Bat_Single_V_Min;
	uint16_t Bat_T;
} PCS_Battery_T;
typedef struct //PCS_3x_T
{
	PCS_Property_T Property;
	PCS_Rmeasure_T Rmeasure;
	PCS_RCommunication_T RCommunication;
	PCS_Battery_T Battery;
} PCS_3x_T;

//PCS_4x_T
typedef struct //PCS_Time_T
{
	uint16_t Year;
	uint16_t Month;
	uint16_t Day;
	uint16_t Hour;
	uint16_t Minute;
	uint16_t Second;
} PCS_Time_T;
typedef struct //PCS_RControl_T
{
	//V21 新协议
	uint16_t Grid_Island;
	uint16_t Grid_Mode_Set;
	uint16_t Island_Mode_Set;
	uint16_t Island_Mode_ONOFF;
	//V12老协议
	uint16_t Work_Mode;

	////////////////////////////////////////
	uint16_t Qvar_Adj_Mode;
	uint16_t Grid_V_Drop_Mode;

} PCS_RControl_T;
typedef struct //PCS_RAdjust_T
{
	//新协议V21遥调
	int16_t Const_I_Mode_I;
	uint16_t Const_V_Mode_V;
	uint16_t Const_V_Mode_I;
	int16_t Const_P_DC_Mode_P;
	int16_t Const_P_AC_Mode_P;

	//老协议V12遥调
	uint16_t Const_I_Charge_I;
	uint16_t Const_V_Charge_V;
	uint16_t Const_V_Charge_I;
	uint16_t Const_P_Charge_P;

	uint16_t Const_I_Discharge_I;
	uint16_t Const_V_Discharge_V;
	uint16_t Const_V_Discharge_I;
	uint16_t Const_P_Discharge_P;

	//////////////////////////////////
	uint16_t Ind_Inverter_V;
	uint16_t Ind_Inverter_F;

	int16_t Power_Factor;                  //功率因数
	int16_t Qvar_Power_Ratio;              //无功比例

} PCS_RAdjust_T;

typedef struct              //PCS_4x_T
{
	PCS_Time_T Time;
	PCS_RControl_T RControl;
	PCS_RAdjust_T RAdjust;
} PCS_4x_T;

typedef struct              //S_PCS_Info_T
{
	PCS_3x_T PCS3x;
	PCS_4x_T PCS4x;
} S_PCS_Info_T;

//S_PV_Info_T
//S_PV_3x_Info_T
typedef struct              //PV_RunParameter_T
{
	uint16_t Type_Code;
	uint16_t Rated_Output_Power;
	uint16_t Output_Type;
	uint16_t Day_Generation;
	uint32_t Total_Generation;
	uint32_t Total_Run_Time;
	int16_t Inside_Temper;
	uint16_t DC_Voltage1;
	uint16_t DC_Current1;
	uint16_t DC_Voltage2;
	uint16_t DC_Current2;
	uint32_t u32_Total_DC_Power;
	uint32_t Total_DC_Power;
	uint16_t VA;
	uint16_t VB;
	uint16_t VC;
	uint16_t IA;
	uint16_t IB;
	uint16_t IC;
	uint32_t u32_Total_Active_Power;
	uint32_t Total_Active_Power;
	int32_t s32_Reactive_Power;
	int32_t Reactive_Power;
	int16_t Power_Factor;
	uint16_t Frequency;
	uint16_t Inverter_Efficiecy;
	uint16_t Inverter_Status;
	uint16_t StatusTime_Year;
	uint16_t StatusTime_Month;
	uint16_t StatusTime_Day;
	uint16_t StatusTime_Hour;
	uint16_t StatusTime_Minute;
	uint16_t StatusTime_Second;
	uint16_t Status_Data1;
	uint16_t Rated_Reactive_Power_Output;
	uint32_t Fault_Status1;
	uint32_t Fault_Status2;
	int16_t Reactor_Temper;
	int16_t Module_Temper1;
	int16_t Module_Temper2;
	int16_t Module_Temper3;
	int16_t Module_Temper4;
	int16_t Module_Temper5;
	int16_t Module_Temper6;
	uint32_t Limit_Power_Value;
	int32_t Reactive_Power_Adjust_Value;
	uint32_t Work_Status;
} PV_RunParameter_T;
typedef struct              //PV_3x_T
{
	PV_RunParameter_T RunParameter;
} PV_3x_T;

//PV_4x_T
typedef struct              //PV_ParaSet_T
{
	uint16_t Year;
	uint16_t Month;
	uint16_t Day;
	uint16_t Hour;
	uint16_t Minute;
	uint16_t Second;
	uint16_t Power_ONOFF;
	uint16_t PowerLimit_Switch;
	uint16_t PowerLimit_Set;
} PV_ParaSet_T;
typedef struct              //PV_4x_T
{
	PV_ParaSet_T ParaSet;
} PV_4x_T;

typedef struct              //S_PV_Info_T
{
	PV_3x_T PV3x;
	PV_4x_T PV4x;
} S_PV_Info_T;

typedef struct              //S_DCDC_Module_Info_T
{
	//单向DCDC
	uint16_t Module_Array_V;
	uint16_t Module_Array_I;
	uint16_t Module_Bat_V;
	uint16_t Module_Charge_I;
	uint32_t Module_Charge_P;
	int16_t Module_In_Temp;
	int16_t Module_Bat_Temp;
	uint16_t Module_Dev_Status;
	uint16_t Module_Dev_Fault_Status;
	uint16_t Module_Output_E_Day;
	uint32_t Module_Output_E_Mon;
	uint32_t Module_Output_E_Total;
	uint32_t Module_CO2_Reduced;
	uint16_t Module_Run_Min_Day;
	uint16_t Module_Run_Hour_Mon;
	uint32_t Module_Run_Hour_Total;

	//双向DCDC
	uint16_t Bus_Bar_V;
	uint16_t Battery_V;
	uint16_t Bus_Bar_I;
	uint16_t Battery_I;
	int16_t Module1_T;
	int16_t Module2_T;
	uint16_t Device_Status;
	uint16_t Fault_Status;
	uint32_t Total_Active_Power;
	uint16_t Charge_Discharge_Flag;
	uint16_t Day_Charge_Energy;
	uint16_t Day_Discharge_Energy;
	uint32_t Total_Charge_Energy;
	uint32_t Total_Discharge_Energy;
	
}S_DCDC_Module_Info_T;

typedef struct              //S_DCDC_Info_T
{
	S_DCDC_Module_Info_T Module_1;
	S_DCDC_Module_Info_T Module_2;
	S_DCDC_Module_Info_T Module_3;
	S_DCDC_Module_Info_T Module_4;
	S_DCDC_Module_Info_T Module_T; //双向DCDC总的信息
} S_DCDC_Info_T;

typedef struct//S_PVS_Info_T  2016.9.19  lk
{
	uint16_t DC_Vol;
	int16_t  Room_T;
	uint16_t Max_I;
	uint16_t Ave_I;
	uint16_t Path_I[16];
	uint32_t Total_I;
	uint32_t Total_P_DC;
	uint32_t Output_Pt_Day;
	uint32_t Output_Pt_Total;
	uint32_t Work_Status;
	uint32_t Short_Data;
	uint32_t Reverse_I;
	uint32_t OI_Data;
	uint32_t Fuse_Off;
	uint32_t UI_Data;
	uint32_t Open_Data;
	uint16_t Path_P[16];
}S_PVS_Info_T;

typedef struct//S_ENV_Info_T  2017.7.19.lk
{
	uint16_t Sunshine_Radiation_Intensity1;  	//日照辐射强度1
	uint32_t Day_Radiation1;					//日辐射量1
	uint32_t All_Radiation1;					//总辐射量1
	uint16_t Sunshine_Radiation_Intensity2;  	//日照辐射强度2
	uint32_t Day_Radiation2;					//日辐射量2
	uint32_t All_Radiation2;					//总辐射量2
	uint16_t Wind_Speed;						//风速
	uint16_t Wind_Level;						//风速等级
	uint16_t Wind_Direction_Degree;				//风向度数
	uint16_t Wind_Position;						//风向位置
	uint16_t Atmospheric_Pressure;				//大气压力
	int16_t Battery_Board_Temperature;			//电池板温度
	int16_t Env_Temperature;					//环境温度
	uint16_t Env_Humidity;						//环境湿度
}S_ENV_Info_T;

typedef struct              //AMMETER_4x_T
{
	uint16_t UIpoint;
	uint16_t PSpoint;   //高字节 DPQ 4~10
	uint16_t Ua;
	uint16_t Ub;
	uint16_t Uc;
	uint16_t Uab;
	uint16_t Ubc;
	uint16_t Uac;
	uint16_t Ia;
	uint16_t Ib;
	uint16_t Ic;

	int32_t Pa;
	int32_t Pb;
	int32_t Pc;
	int32_t Pt;

	int32_t Qa;
	int32_t Qb;
	int32_t Qc;
	int32_t Qt;

	int16_t PFa;
	int16_t PFb;
	int16_t PFc;
	int16_t PFt;
	uint16_t hz;
} AMMETER_4x_T;

typedef struct   //S_AMMETER_Info_T
{
	AMMETER_4x_T AMMETER4x;
} S_AMMETER_Info_T;

//PCS_Ctrl_Data_T
typedef struct   //PCS_Ctrl_Data_T
{
	uint16_t Grid_Island;
	uint16_t Grid_Mode_Set;
	uint16_t Island_Mode_Set;
	uint16_t Island_Mode_ONOFF;
	//旧协议
	uint16_t Work_Mode;

	uint16_t Qvar_Adj_Mode;
	uint16_t Grid_V_Drop_Mode;

	int16_t Const_I_Mode_I;
	uint16_t Const_V_Mode_V;
	uint16_t Const_V_Mode_I;
	int16_t Const_P_DC_Mode_P;
	int16_t Const_P_AC_Mode_P;

	//旧协议
	uint16_t Const_I_Charge_I;
	uint16_t Const_V_Charge_V;
	uint16_t Const_V_Charge_I;
	uint16_t Const_P_Charge_P;

	uint16_t Const_I_Discharge_I;
	uint16_t Const_V_Discharge_V;
	uint16_t Const_V_Discharge_I;
	uint16_t Const_P_Discharge_P;

	///////////////////////////////
	uint16_t Ind_Inverter_V;
	uint16_t Ind_Inverter_F;

	int16_t Power_Factor;
	int16_t Qvar_Power_Ratio;

} PCS_Ctrl_Data_T;

//PCS_Ind_Ctrl_T
typedef struct   //PCS_Ind_Ctrl_T
{

	bool Grid_Const_I_Mode;
	bool Grid_Const_V_Mode;
	bool Grid_Const_P_DC_Mode;
	bool Grid_Const_P_AC_Mode;

	bool Standby;
	bool Invert;
	bool Stop;

	bool Reactive_Power_Set;
	bool Parameter_Modify;
} PCS_Ind_Ctrl_T;

//PV_Ind_Ctrl_T
typedef struct   //PV_Ind_Ctrl_T
{
	bool Power_On;
	bool Power_Off;
	bool Active_Power_Limit;
	bool Reactive_Power_Limit;
} PV_Ind_Ctrl_T;

//PV_Ctrl_Data_T
typedef struct   //PV_Ctrl_Data_T
{
	uint16_t Power_ONOFF;
	uint16_t PowerLimit_Switch;
	uint16_t PowerLimit_Set;
	uint16_t Powerfactor_Set;
	uint16_t LVRT_Switch;
	uint16_t Reactive_Power_Adjust_SelSwitch;
	uint16_t Reactive_Power_Propority_Set;
	uint16_t Leak_Current_Protect_Value;
	uint16_t PowerLimit_Value_Set;
	uint16_t Ractive_Power_Value_Set;
}PV_Ctrl_Data_T;

//DCDC_Ind_Ctrl_T
typedef struct//dcdc_Ind_Ctrl_T
{
	bool Power_On;
	bool Power_Off;
}DCDC_Ind_Ctrl_T;

//DCDC_Ctrl_Data_T
typedef struct//DCDC_Ctrl_Data_T
{
	uint16_t Power_ONOFF;
	
}DCDC_Ctrl_Data_T;

//Sys_Ctrl_T
typedef struct   //Sys_Ctrl_T
{
	bool Sys_Stop;
	bool Flt_Stop;
	bool Manual_Stop;
	bool Active_Flt_Stop;
	bool Sys_Start;
	bool Batlow_Stop;
	bool Autorun_Stop;
	bool Grid_Connected_Start;
	bool Island_Start;
	bool Auto_Run;
	//PCS
	bool Grid_Const_I_Mode;
	bool Grid_Const_V_Mode;
	bool Grid_Const_P_DC_Mode;
	bool Grid_Const_P_AC_Mode;
	bool Standby;
	bool Invert;
	bool Stop;
	bool Reactive_Power_Set;
	bool Parameter_Modify;
	//SG
	bool Power_On;
	bool Power_Off;
	bool Active_Power_Limit;
	bool Reactive_Power_Limit;
} Sys_Ctrl_T;

//Bat_Status_T
typedef struct   //Bat_Status_T
{
	bool Vbat_G95;
	bool Vbat_G90;
	bool Vbat_L80;
	bool Vbat_L40;
	bool Vbat_L30;

	bool SOC_G95;
	bool SOC_G90;
	bool SOC_L80;
	bool SOC_L40;
	bool SOC_L30;
} Bat_Status_T;

//sPid_t
typedef struct
{
	float RegErr;
	float RegRef;
	float RegFdb;
	float RegUp;
	float RegKp;
	float RegUi;
	float RegKi;
	float RegKc;
	float RegSatErr;
	float RegOutPreSat;
	float RegOutMax;
	float RegOutMin;
	float RegOut;
	float VacIdUi;
	float VacIdOut;
	float UdSatErr;

} _SPID_T;

//Bat_Status_Delay_T
typedef struct   //Bat_Status_Delay_T
{
	uint16_t Vbat_G95;
	uint16_t Vbat_G90;
	uint16_t Vbat_L80;
	uint16_t Vbat_L40;
	uint16_t Vbat_L30;

	uint16_t SOC_G95;
	uint16_t SOC_G90;
	uint16_t SOC_L80;
	uint16_t SOC_L40;
	uint16_t SOC_L30;
} Bat_Status_Delay_T;

//System_Run_Info_T
typedef struct
{
	uint32_t Total_Run_Time;
	uint32_t Total_MG_Run_Time;
	uint32_t Total_Grid_Run_Time;
	uint32_t Total_Charge_Time;
	uint32_t Total_Discharge_Time;
	uint32_t Total_PV_Run_Time;
	uint32_t Total_Load_Supply_Time;
	uint32_t Total_Fault_Time;

	uint16_t Day_Run_Time;
	uint16_t Day_MG_Run_Time;
	uint16_t Day_Grid_Run_Time;
	uint16_t Day_Charge_Time;
	uint16_t Day_Discharge_Time;
	uint16_t Day_PV_Run_Time;
	uint16_t Day_Load_Supply_Time;
	uint16_t Day_Fault_Time;

	uint32_t Run_min;
	uint32_t MG_Run_min;
	uint32_t Grid_Run_min;
	uint32_t Charge_min;
	uint32_t Discharge_min;
	uint32_t PV_Run_min;
	uint32_t Load_Supply_min;
	uint32_t Fault_min;

	uint32_t Total_Charge_Energy;
	uint32_t Total_Discharge_Energy;
	uint32_t Total_PV_Energy;
	uint32_t Total_Load_Energy;

	uint16_t Day_Charge_Energy;
	uint16_t Day_Discharge_Energy;
	uint16_t Day_DCDC_Charge_Energy;
	uint16_t Day_DCDC_Discharge_Energy;
	uint16_t Day_PV_Energy;
	uint16_t Day_Load_Energy;

	uint64_t Charge_Energy_Acc;
	uint64_t Discharge_Energy_Acc;
	uint64_t PV_Energy_Acc;
	uint64_t Load_Energy_Acc;

	uint64_t Day_Load_Energy_Acc;

	int16_t E_Load_Adjust;

	uint16_t PCS_DC_Voltage;
	uint16_t PCS_AC_Voltage;

	uint16_t PV_DC_Voltage;
	uint16_t PV_AC_Voltage;

} S_System_Run_Info_T;

//com_IO_BMS.h
typedef struct   //S_BMS_Info_T
{
	uint16_t BMS_Fault_Info;                //干接点故障信息
	uint16_t BMS_Warning_Info;              //干接点告警信息

	uint16_t BMS_Work_Status;               //工作状态
	uint16_t BMS_Fault;                		//电池故障信息
	uint16_t BMS_Warn;                 		//电池报警信息
	uint16_t BMS_SOC;                       //电池组SOC
	uint16_t BMS_Rest_Input;                //电池组可充电量
	uint16_t BMS_Rest_Output;               //电池组可发电量
	uint16_t BMS_Total_V;                   //电池组总电压
	uint16_t BMS_Total_I;                   //电池组总电流
	uint16_t BMS_Single_SOC_Max;            //电池单体最高SOC
	uint16_t BMS_Single_SOC_Min;            //电池单体最低SOC
	uint16_t BMS_Single_V_Max;              //电池单体最高电压
	uint16_t BMS_Single_V_Min;              //电池单体最低电压
	uint16_t BMS_Input_V_Max;               //最高允许充电电压
	uint16_t BMS_Input_I_Max;               //最高充电允许电流
	int16_t BMS_Battery_Temp;              //柜体温度

	//sumsung电池信息
	// 68,94ah电池共用
	uint16_t Sys_Voltage;
	uint16_t Sys_Current;
	uint16_t Sys_SOC;
	uint16_t Sys_SOH;
	uint16_t Single_V_Min;
	uint16_t Single_V_Max;

	int16_t Single_T_Min;
	int16_t Single_T_Max;
	int8_t Room_Temp1;
	int8_t Room_Temp2;

	uint16_t Rack_Fault_Word;
	uint16_t Rack_Alarm_Word;
	uint16_t Sys_Alarm_Word;
	uint16_t Sys_Mode;

	uint16_t Discharge_Limit;
	uint16_t Charge_Limit;
	uint8_t Num_Of_Rack_In_Service;
	uint8_t Num_Of_Rack_In_System;
	uint16_t Service_Voltage;
	uint16_t Service_SOC;

	uint16_t DC_Contactor_P_Status;
	uint16_t DC_Contactor_N_Status;

	uint16_t Fan_Operating_Status;

	uint16_t Total_G_Charge_Elec;
	uint16_t Total_M_Charge_Elec;
	uint16_t Total_K_Charge_Elec;

	uint16_t Total_G_Discharge_Elec;
	uint16_t Total_M_Discharge_Elec;
	uint16_t Total_K_Discharge_Elec;

	//94ah
	uint16_t Protection4;
	uint16_t Protection3;
	uint16_t Protection2;
	uint16_t Protection1;

	uint16_t Alarm4;
	uint16_t Alarm3;
	uint16_t Alarm2;
	uint16_t Alarm1;

	uint16_t Sys_Alarm_Status;

} S_BMS_Info_T;

typedef struct
{
	float I_adc[2];
	float V_adc[4];
	float PT[2];
} S_ADC_Info_T;

//系统用变量 
typedef struct    //S_Com_Data_T
{
	uint8_t Year_Last;
	uint8_t Month_Last;
	uint8_t Day_Last;
	uint8_t Hour_Last;
	uint8_t Minute_Last;
	uint8_t Second_Last;

	uint8_t PC_Time_Flag;		            //跳变标志，说明PC对时

	uint8_t Load_Default_Flag;             //恢复出厂值标识，在主循环中执行
	uint8_t Load_Defalut_Net_Flag;

	uint8_t DHCP_State_Last;
	bool Touch_Set_Net_Flag;            //设置网络参数的标志 ccg 2013.6.3
	bool Touch_Set_Net2_Flag;
	bool Touch_Set_NTP_Flag;
	bool Touch_Set_Net_Port_Flag;
	bool Touch_Set_Server_Port_Flag[PCS_NUM];
	bool Start_DHCP;                    //开始DHCP的标志 ccg 2013.6.3
	uint8_t DHCP_Bound;                    //DHCP成功的标志 ccg 2013.6.3

	uint8_t Protocol_Version;				//协议版本 0--New Protocol;1--Old Protocol.

	uint8_t Touch_Hardware_Edition;		//触摸屏硬件版本
	uint8_t Touch_Software_Edition;		//触摸屏软件版本

} S_Com_Data_T;

//VSG Set
typedef struct
{
	uint16_t Grid_on_permit;
	//uint16_t Grid_off_force;
	//uint16_t Grid_on_force;
} VSG_Set_ST;

//VSG struct
typedef struct
{
	//DSP透传数据
	uint16_t Grid_freq;
	uint16_t MGrid_freq;
	uint16_t Grid_voltage;
	uint16_t MGrid_voltage;
	uint16_t Grid_voltage_Vab;
	uint16_t Grid_voltage_Vbc;
	uint16_t Sync_Ready_Flag;
	int16_t Environment_temp;
	//运算值
	uint16_t Grid_Freq_Average;
	//同步状态信息(to pcs)
	uint16_t Sync_Grid_Contactor_State;
	VSG_Set_ST Set_info;
} VSG_Info_ST;

typedef struct	//HisResetRecord
{
	uint16_t ptr;                            //当前事件记录指针
	uint16_t reset_code;                     //复位代码
	uint16_t total_nub;                      //复位存储的记录条数
	uint16_t total_page_nub;             	 //复位记录的页数
	uint16_t current_page_nub;           	 //复位记录的当前页码
	uint16_t current_page_nub_record_nb;     //复位记录的当前页码的故障记录数
	uint16_t current_page_nub_record_array[EVERY_EVENTRECORD_PAGE_NB];     //读出当前页码中要显示的所有记录的记录号
	bool read_reset_record_flash_flag;       //读复位记录刷新标志
} HisResetRecord;

typedef struct	//HisEventRecord
{
	uint16_t ptr;                            //当前事件记录指针
	uint16_t event_code;                     //事件代码
	uint16_t total_nub;                      //事件存储的记录条数
	uint16_t total_page_nub;             	//事件记录的页数
	uint16_t current_page_nub;           	//事件记录的当前页码
	uint16_t current_page_nub_record_nb;     //事件记录的当前页码的故障记录数
	uint16_t current_page_nub_record_array[EVERY_EVENTRECORD_PAGE_NB];     //读出当前页码中要显示的所有记录的记录号
	bool read_event_record_flash_flag;      //读历史事件刷新标志
} HisEventRecord;

typedef struct      //HisFaultRecord
{
	uint16_t ptr;                  		    //当前故障记录指针
	uint16_t total_nub;                  	//故障存储的记录条数
	uint16_t total_page_nub;             	//故障记录的页数
	uint16_t current_page_nub;           	//故障记录的当前页码
	uint16_t current_page_nub_record_nb;     //故障记录的当前页码的故障记录数
	uint16_t current_page_nub_record_array[EVERY_FAULTRECORD_PAGE_NB];     //读出当前页码中要显示的所有记录的记录号
	uint64_t oldfault;                      //旧故障值
	bool read_fault_record_flash_flag;      //读历史故障刷新标志
} HisFaultRecord;

typedef struct      //HisAlarmRecord
{
	uint16_t ptr;                  		    //当前故障记录指针
	uint16_t total_nub;                  	//故障存储的记录条数
	uint16_t total_page_nub;             	//故障记录的页数
	uint16_t current_page_nub;           	//故障记录的当前页码
	uint16_t current_page_nub_record_nb;     //故障记录的当前页码的故障记录数
	uint16_t current_page_nub_record_array[EVERY_ALARMRECORD_PAGE_NB];     //读出当前页码中要显示的所有记录的记录号
	uint64_t oldalarm;                      //旧告警值
	bool read_alarm_record_flash_flag;      //读历史故障刷新标志
} HisAlarmRecord;

typedef struct      //TGBuf
{
	uint8_t NET1_RBuf[MODBUS_BUF_LEN];	        //网口接收缓冲
	uint8_t NET1_SBuf[MODBUS_BUF_LEN];	        //网口发送缓冲

	uint8_t Can1_RBuf[MODBUS_BUF_LEN];	        //CAN1口接收缓冲
	uint8_t Can1_SBuf[MODBUS_BUF_LEN];	        //CAN1口发送缓冲
	uint8_t Can2_RBuf[MODBUS_BUF_LEN];	        //CAN2口接收缓冲
	uint8_t Can2_SBuf[MODBUS_BUF_LEN];	        //CAN2口发送缓冲

	uint8_t Com1_RBuf[MODBUS_BUF_LEN];	        //串口1接收缓冲
	uint8_t Com1_SBuf[MODBUS_BUF_LEN];	        //串口1发送缓冲
	uint8_t Com2_RBuf[MODBUS_BUF_LEN];	        //串口2接收缓冲
	uint8_t Com2_SBuf[MODBUS_BUF_LEN];	        //串口2发送缓冲
	uint8_t Com3_RBuf[MODBUS_BUF_LEN];	        //串口3接收缓冲
	uint8_t Com3_SBuf[MODBUS_BUF_LEN];	        //串口3发送缓冲
} TGBuf;

struct clinet_info
{
	Connection *client;
	int state;
	CLIENT_PROTOCOL_TYPE type;
};

typedef struct
{
	uint16_t baud;   //串口波特率
	uint16_t vmin;   //串口最大返回字节数
	uint8_t sId;	//扫描起始ID
	uint8_t eId;    //扫描结束ID
	SERIAL_PROTOCOL_TYPE protocol;  //设备协议
}COM_PARAM;

typedef struct
{
	int sock;
} S_ModeBusBody_T;

typedef struct	        //http_state
{
	char *file;
	uint32_t left;
} http_state;

struct h_resource	        //html资源数组
{
	const char* data;
	const unsigned int size;
};

struct j_resource	        //js资源数组
{
	const char* data;
	const unsigned long size;
};
struct c_resource	        //css资源数组
{
	const char* data;
	const unsigned int size;
};
struct i_resource	        //image资源数组
{
	const char* data;
	const unsigned int size;
};
struct s_resource	        //SetValue()函数
{
	char* data;
	unsigned int size;
};
struct a_resource	        //ajax处理json
{
	char* data;
	unsigned int size;
};
struct p_resource	        //曲线图和柱状图数据   Config.csv配置文件    查询历史记录    发电量导出
{
	char* data;
	unsigned long size;
};

//EXTERN------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//Com_IO_TCPClient
extern struct tcp_pcb* client_pcb[PCS_NUM];
extern uint16_t Init_temp[PCS_NUM];
extern uint16_t Recv_temp;
extern uint16_t Sent_temp;
extern uint16_t Poll_temp;
extern uint16_t Conn_temp;
extern uint16_t Err_temp;
extern struct tcp_client_app_arg* client_app_arg[PCS_NUM];

//-------------------------------------------------------------------------------
//ems_calc_.c
//PID电压控制参数
extern uint16_t wInverterAcVoltDQKp,wInverterAcVoltDQKi,Balancing_Kp,wInverterAcVoltDQKp_PVLimit,wInverterAcVoltDQKi_PVLimit; //KP KI
extern _SPID_T sPid[PCS_NUM+1];

//回调参数
extern uint16_t Parameter_recovery_kp;           //回调比例
extern uint16_t Parameter_recovery_dalay_time;   //回调延时时间
//最大逆流功率
extern int16_t Max_anti_reflux_power;
//光伏广播使能
extern uint16_t Send_Data_To_PV_Broadcast_Enable;
//曲线保存时间
extern uint16_t Time_900_seconds;
//串口通信参数
extern COM_PARAM comParam[6];
//重启标志
extern bool reboot;
//adc
extern S_ADC_Info_T adc;

//-------------------------------------------------------------------------------
//分时电价
extern S_Time_Div_Info_T Time_Div_Info;
//End.

//-------------------------------------------------------------------------------
extern uint32_t YX_Data_Send_Left;
extern uint32_t YC_Data_Send_Left;
extern uint32_t YC_data[REMOTE_MESURE_END_ADDR - REMOTE_MESURE_BEGIN_ADDR + 1];

//曲线缓存
extern int32_t PV_Power_Curve_Buffer[1440];  	    			//调整前功率曲线
extern int32_t Grid_Power_Curve_Buffer[1440]; 	    		//调整后功率曲线
extern int32_t PCS_Power_Curve_Buffer[1440];					//储能功率曲线
extern int32_t Load_Power_Curve_Buffer[1440];              //负载功率曲线

//com_io_bms.c
extern uint8_t BMS_PV_DCDC_Auto_Switch;
extern S_BMS_Info_T BMS_Info[BMS_NUM];
extern bool BMS_DC_Contactor_on_cmd[BMS_NUM];
extern bool BMS_DC_Contactor_off_cmd[BMS_NUM];
extern bool BMS_All_Fault_Reset_cmd[BMS_NUM];
extern uint8_t BMS_Switch_Control[BMS_NUM];

//-------------------------------------------------------------------------------
//com_io_dsp.c
extern bool Send_Data_To_DSP_Flag;
extern uint8_t can_id;
//-------------------------------------------------------------------------------
//com_io_em.c

//-------------------------------------------------------------------------------
//com_io_monitor.c

//-------------------------------------------------------------------------------
//com_io_dcdc.c
extern bool Search_DCDC_Info_0x03[DCDC_NUM];
extern bool Search_DCDC_Info_0x04_01[DCDC_NUM];
extern bool Search_DCDC_Info_0x04_02[DCDC_NUM];
extern bool Search_DCDC_Info_0x04_03[DCDC_NUM];
extern bool Send_Data_To_DCDC_0x10_01[DCDC_NUM];
extern bool Send_Data_To_DCDC_0x10_02[DCDC_NUM];

//-------------------------------------------------------------------------------
//com_io_pcs.c
extern bool Search_PCS_Info_0x03[PCS_NUM];
extern bool Search_PCS_Info_0x04_01[PCS_NUM];
extern bool Search_PCS_Info_0x04_02[PCS_NUM];
extern bool Search_PCS_Info_0x04_03[PCS_NUM];
extern bool Send_Data_To_PCS_0x10_01[PCS_NUM];
extern bool Send_Data_To_PCS_0x10_02[PCS_NUM];

extern bool Send_Data_To_IO_0x10_01;
extern bool Send_Data_To_IO_0x10_02;
extern bool Send_Data_To_IO_0x10_03;
extern bool Send_Data_To_IO_0x10_04;
extern bool Search_IO_Info_0x03;

//-------------------------------------------------------------------------------
//com_io_pv.c
extern bool Search_PV_Info_0x03[PV_NUM];
extern bool Send_Data_To_PV_0x10_01[PV_NUM];
extern bool Send_Data_To_PV_0x10_02[PV_NUM];
extern bool Send_Data_To_PV_Broadcast;
extern bool Search_PV_Info_0x04[PV_NUM];
//-------------------------------------------------------------------------------
//com_IO_Touch.c
extern S_Touch_Data_T Touch_Data;          	//触摸屏数据
extern uint8_t Touch_Write_Time[7];        	//ccg 2013.5.18
extern bool Touch_Write_Time_Flag;         	//ccg 2013.5.18
extern uint16_t Disp_PCS_Num;					//当前显示PCS编号
extern uint16_t Disp_PV_Num;					//当前显示PV编号
extern uint16_t Disp_BMS_Num;                  //当前显示BMS编号
extern uint16_t Disp_DCDC_Num;					//当前显示DCDC编号
//-------------------------------------------------------------------------------
//common_function.c
extern HisResetRecord write_reset_record;   //存储历史复位记录
extern HisResetRecord read_reset_record;    //读历史复位记录
extern HisEventRecord write_event_record;  	//存储历史事件记录
extern HisEventRecord read_event_record;   	//读存储历史事件记录
extern HisFaultRecord write_fault_record;  	//存储历史故障记录
extern HisFaultRecord read_fault_record;   	//读存储历史故障记录
extern HisAlarmRecord write_alarm_record;   //存储历史告警记录
extern HisAlarmRecord read_alarm_record;    //读存储历史告警记录
extern UserLogAttr g_logattr[RECORD_QUEUE_NUM];
//-------------------------------------------------------------------------------
//timer.c
extern uint8_t delay_temp;                 	//开机(40*0.5)秒内不报通讯故障，超过20秒后，还是无通讯，就报通讯故障
extern uint16_t Run_Mode_Last;             	//运行状态上次设置的值
extern uint16_t Time_Run_Mode_Last;        	//BMS用来判断停机关机的值
extern uint8_t dig_input[16];
extern bool delay10s;
extern bool Clear_Data;
//-------------------------------------------------------------------------------
//task_net.c
extern S_ModeBusBody_T *g_modbus_ss[TCP_MODBUS_LINK_NUM];
extern S_IEC104Body_T *g_iec104_ss[TCP_IEC104_LINK_NUM];
//-------------------------------------------------------------------------------
//iec104rtu.c
extern ChangedSignal CS[100];
extern uint64_t OldFault;
extern uint32_t OldAlarm;
extern uint16_t OldEvent;
extern uint64_t OldOther;
extern uint8_t OldYX001;
extern uint8_t OldYX042;
//-------------------------------------------------------------------------------
//commom_data.c
extern VSG_Info_ST VSG_Info;	                   //VSG相关信息

extern const uint16_t Device_Type_Code[DEVICE_NUM];
extern uint8_t g_adjust_flag;
extern uint8_t IOD_Poweron;
extern S_System_Info_T System_Info;          //逆变器参数
extern S_System_Run_Info_T System_Run_Info;  //系统运行参数
extern S_Com_Data_T Com_Data;         		 //公共参数
extern S_EMS_Info_T EMS_Info;				 //EMS控制参数
extern S_PCS_Info_T PCS_Info[PCS_NUM];		 //储能逆变器参数
extern S_PV_Info_T PV_Info[PV_NUM];		 //光伏逆变器参数
extern S_DCDC_Info_T DCDC_Info[DCDC_NUM];	 //光伏控制器参数
extern S_AMMETER_Info_T AMMETER_Info[Meter_NUM];
extern S_PVS_Info_T PVS_Info[PVS_NUM];			//光伏汇流箱参数		2016.9.17 lk
extern S_ENV_Info_T ENV_Info[ENV_NUM];			//环境仪参数  2016.9.19 lk

//extern GATTPara     gATTPara;         		//ATT7022B相关参数
extern TGBuf gBuf;             		//全局缓冲
extern gt_time showTime;
extern S_IAT_Info_T IAT_Info;         //IAT协议

extern char* const Device_Info[];
extern const Param_Range P_Range[PCS_Power_Max];

//extern uint8_t  PCS_Type ;

extern uint16_t Machine_Type_Number;
extern uint16_t Div_Factor;            		//全机型的功率曲线百分比因子
extern uint16_t Percent_Factor;        		//全机型的电量柱百分比因子
extern char P1[16];
extern char P2[20];
extern char P3[30];
extern char P4[30];
extern char P5[30];

extern bool True_False;

//-------------------------------------------------------------------------------
//EMS.c
extern PCS_Ctrl_Data_T  PCS_Ctrl_Data[PCS_NUM];			//PCS数据控制
extern PCS_Ctrl_Data_T  PCS_Ctrl_Data_Rec[PCS_NUM];    //PCS数据控制

extern PCS_Ind_Ctrl_T   PCS_Ind_Ctrl[PCS_NUM];			//PCS单机控制
//extern PV_Ctrl_Data_T   PV_Ctrl_Data[PV_NUM];				//PV数据控制
extern PV_Ind_Ctrl_T    PV_Ind_Ctrl[PV_NUM];				//PV单机控制
extern PV_Ctrl_Data_T   PV_Ctrl_Data[PV_NUM];				//PV数据控制
extern DCDC_Ind_Ctrl_T  DCDC_Ind_Ctrl[DCDC_NUM];		//DCDC单机控制
extern DCDC_Ctrl_Data_T DCDC_Ctrl_Data[DCDC_NUM];		//DCDC数据控制
extern Sys_Ctrl_T Sys_Ctrl;								//系统控制

extern bool PCS_Com_Err[PCS_NUM];						//单台PCS通讯故障标志
extern bool PCS_Com_OK[PCS_NUM];
extern bool PV_Com_Err[PV_NUM];							//单台PV通讯故障标志
extern bool PVS_Com_Err[PVS_NUM];						//单台PVS通讯故障标志    2016.9.17 lk
extern bool BMS_Com_Err[BMS_NUM];                       //单台BMS通讯故障标志
extern bool BMS_Com_OK[BMS_NUM];
extern bool DCDC_Com_Err[DCDC_NUM];					    //单台DCDC通讯故障标志
extern bool Meter_Com_Err[Meter_NUM];					//单台PV通讯故障标志
extern bool PCS_Ready_Status[PCS_NUM];                  //PCS故障状态检查
extern bool DSP_Com_Err[DSP_NUM];                       //单台DSP通讯故障标志
extern bool ENV_Com_Err[ENV_NUM];						//单台环境检测仪通讯故障标志

extern bool Sys_Start_Flag;								//系统启动标志
extern bool Sys_Stop_Flag;								//系统停机标志
extern bool Island_Start_Flag;							//孤岛启动标志
extern bool Grid_Start_Flag;					        //并网启动标志
extern bool Sys_Running_Flag;							//系统运行标志
extern bool Reset_Require;								//系统停机完成
extern bool EMG_Stop;									//紧急停机
extern bool Fault_Once;							        //只上报一次故障

//extern uint8_t EMS_Run_Priority;					//运行模式 0-电网优先；1-微网优先
//extern uint8_t EMS_Start_Type;					//启动类型 0-被动模式；1-主动模式
//extern uint8_t Bus_Type;							//母线类型:0-直流母线；1-交流母线
extern uint8_t Grid_Connect_Type;					//是否并网类型:0-非并网型；1-并网型
//extern uint8_t DC_Bus_Charge_Permit;				//直流母线是否允许充电

extern uint16_t EMG_Counter;						//紧急停机计数器

extern uint8_t Load_Power_Status;					//负载供电状态 0-无供电；1-电网供电；2-微网供电。
extern uint8_t Grid_Pic_Direction;					//电网图片滚动方向
extern uint8_t PCS_Pic_Direction;					//PCS图片滚动方向
extern uint8_t PV_Pic_Direction;					//PV图片滚动方向
extern uint8_t Load_Pic_Direction;					//负载图片滚动方向
extern uint8_t MGrid_Pic_Direction;				//微网图片滚动方向
extern uint8_t EMS_RL_Mode;						//EMS远程就地模式 0-远程和本地；1-远程；2-本地
extern uint8_t IO_Set_Touch_Req;					//计量板对时触摸屏
extern uint8_t Touch_Set_Time_Flag;				//触摸屏对时计量板
extern uint8_t Allow_Load_Default;					//允许恢复出厂设置

//Island_Start
extern uint16_t IS_Flag;
extern uint16_t IS_Counter[PCS_NUM];

extern uint16_t Start_Fail_CNT;						//启动失败计数
//Grid_Connected_Start
extern uint16_t GC_Flag;
extern uint16_t GC_Counter[PCS_NUM];

extern uint16_t Grid_Connected_Start_SOC_Delay;

//Self_Check
extern uint16_t SC_Flag;
extern uint16_t SC_Counter[PCS_NUM];

extern bool Self_Check_OK;
extern bool PCS_Ready;

//System_Stop
extern uint16_t SS_Flag;
extern uint16_t SS_Counter[PCS_NUM];

//Alarm_Check
extern uint16_t AC_Counter[PCS_NUM];

//Fault_Check
extern uint16_t FC_Counter[PCS_NUM];

//System_Logic
extern uint8_t Grid_First;											//电网优先标志
extern uint8_t Island_First;										//微网优先标志

//Grid And MGrid Contactor State
extern bool Grid_Contactor_Control_State;
extern bool MGrid_Contactor_Control_State;
//extern bool Run_Light_Control_State;
//extern bool Fault_Light_Control_State;

//SOC_Status
extern uint16_t SOC_Counter[PCS_NUM];
extern uint16_t SOC_Counter1[PCS_NUM];
extern uint16_t SOC_Counter2[PCS_NUM];
extern uint16_t SOC_Counter3[PCS_NUM];
extern uint16_t SOC_Counter4[PCS_NUM];
extern uint16_t SOC_Counter5[PCS_NUM];

extern Bat_Status_T Bat_Status; //电池状态

//Running_Control
extern uint16_t RC_Counter[PCS_NUM];

//Switch_to_Grid
extern bool Switch_to_Grid_Status;
extern uint16_t Switch_to_Grid_Flag;
extern uint16_t StG_Counter[PCS_NUM];

//Switch_to_Island
extern bool Switch_to_Island_Status;
extern uint16_t Switch_to_Island_Flag;
extern uint16_t StI_Counter[PCS_NUM];

//AGC_Control
extern uint16_t AGC_Counter[PCS_NUM];

extern uint16_t PV_OK_DC_V;

//-------------------------------------------------------------------------------
//http_interface.c
/*
 extern struct h_resource h_Rsc[];
 extern struct j_resource j_Rsc[];
 extern struct c_resource c_Rsc[];
 extern struct i_resource i_Rsc[];
 extern struct s_resource s_Rsc[];
 extern struct a_resource a_Rsc[];
 extern struct p_resource p_Rsc[];
 extern char p_000[17000];    				//曲线图数据
 extern char a_000[80];      				//ajax确认回复
 extern char s_000[1024];     				//SetValue();

 extern uint16_t Param_flag[PARAMETER_NUM];  //保存要保存的参数值和保存标识
 extern uint16_t Param_value[PARAMETER_NUM]; //保存要保存的参数值和保存标识
 extern uint8_t Webserver_Language;         	//0x55:中文 0xAA：English

 extern char jsCharts_temp[500];				//用于数据缓存

 extern uint16_t Param_flag[PARAMETER_NUM];  //保存要保存的参数值和保存标识
 extern uint16_t Param_value[PARAMETER_NUM];	//保存要保存的参数值和保存标识
 extern uint8_t save_flag_int;               //异步判断是否保存参数值的标识(数值型)

 extern LogTime R_time;
 */

//FUNCTION----------------------------------------------------------------------------------------------------------------------
void Sys_Init(void);
#endif

