#include "includes.h"
//-------------------------------------------------------------------------------
uint32_t YX_Data_Send_Left=REMOTE_SIGNAL_END_ADDR-REMOTE_SIGNAL_BEGIN_ADDR+1;
uint32_t YC_Data_Send_Left=REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1;
uint32_t YC_data[REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1]={0};

//消峰填谷曲线缓存
int32_t PV_Power_Curve_Buffer[1440]={0};  	    			//PV功率曲线
int32_t Grid_Power_Curve_Buffer[1440]={0}; 	    			//Grid功率曲线
int32_t PCS_Power_Curve_Buffer[1440]={0};					//储能功率曲线
int32_t Load_Power_Curve_Buffer[1440]={0};                   //负载功率曲线

//串口参数
COM_PARAM comParam[6];

//重启标志
bool reboot = false;

//ADC
S_ADC_Info_T adc;

//Add by Lihd.15-11-17
//分时电价
S_Time_Div_Info_T Time_Div_Info;
//End.

//-------------------------------------------------------------------------------
//Com_IO_TCPClient
struct tcp_pcb *client_pcb[PCS_NUM];
uint16_t Init_temp[PCS_NUM];
uint16_t Recv_temp;
uint16_t Sent_temp;
uint16_t Poll_temp;
uint16_t Conn_temp;
uint16_t Err_temp;
struct tcp_client_app_arg* client_app_arg[PCS_NUM];

//-------------------------------------------------------------------------------
//com_io_bms.c
uint8_t  BMS_PV_DCDC_Auto_Switch;
S_BMS_Info_T BMS_Info[BMS_NUM];
bool BMS_DC_Contactor_on_cmd[BMS_NUM];
bool BMS_DC_Contactor_off_cmd[BMS_NUM];	
bool BMS_All_Fault_Reset_cmd[BMS_NUM];
uint8_t BMS_Switch_Control[BMS_NUM];

//-------------------------------------------------------------------------------
//com_io_monitor.c

//-------------------------------------------------------------------------------
//com_io_dsp.c
bool Send_Data_To_DSP_Flag=false;

uint8_t can_id = 0x01;
//com_io_dcdc.c
bool Search_DCDC_Info_0x03[DCDC_NUM];
bool Search_DCDC_Info_0x04_01[DCDC_NUM];
bool Search_DCDC_Info_0x04_02[DCDC_NUM];
bool Search_DCDC_Info_0x04_03[DCDC_NUM];
bool Send_Data_To_DCDC_0x10_01[DCDC_NUM];
bool Send_Data_To_DCDC_0x10_02[DCDC_NUM];

//-------------------------------------------------------------------------------
//com_io_pcs.c
bool Search_PCS_Info_0x03[PCS_NUM];
bool Search_PCS_Info_0x04_01[PCS_NUM];
bool Search_PCS_Info_0x04_02[PCS_NUM];
bool Search_PCS_Info_0x04_03[PCS_NUM];
bool Send_Data_To_PCS_0x10_01[PCS_NUM];
bool Send_Data_To_PCS_0x10_02[PCS_NUM];
//-------------------------------------------------------------------------------
//com_io_pv.c
bool Search_PV_Info_0x03[PV_NUM];
bool Send_Data_To_PV_0x10_01[PV_NUM];
bool Send_Data_To_PV_0x10_02[PV_NUM];
bool Send_Data_To_PV_Broadcast;
bool Search_PV_Info_0x04[PV_NUM];

//-------------------------------------------------------------------------------
//com_io_touch.c
S_Touch_Data_T Touch_Data;          //触摸屏数据
uint8_t Touch_Write_Time[7]={0};    //ccg 2013.5.18
bool Touch_Write_Time_Flag=false;   //ccg 2013.5.18
uint16_t Disp_PCS_Num=0;			//当前显示PCS编号
uint16_t Disp_PV_Num=0;				//当前显示PV编号
uint16_t Disp_BMS_Num=0;            //当前显示BMS编号
uint16_t Disp_DCDC_Num=0;           //当前显示DCDC编号

//-------------------------------------------------------------------------------
//commom_function.c
HisResetRecord write_reset_record;  //存储历史复位记录
HisResetRecord read_reset_record;   //读历史复位记录
HisEventRecord write_event_record;  //存储历史事件记录
HisEventRecord read_event_record;   //读存储历史事件记录
HisFaultRecord write_fault_record;  //存储历史故障记录
HisFaultRecord read_fault_record;   //读存储历史故障记录
HisAlarmRecord write_alarm_record;  //存储历史告警记录
HisAlarmRecord read_alarm_record;   //读存储历史告警记录
//log
UserLogAttr g_logattr[RECORD_QUEUE_NUM];
//-------------------------------------------------------------------------------
//timer.c
uint8_t delay_temp=20;              //开机(20*0.5)秒内不报通讯故障，超过10秒后！
									//还是无通讯，就报通讯故障(与DSP同时控制停机
									//和显示故障)//ccg 2013.6.5
uint8_t dig_input[16]={0};
bool delay10s=false;				//上位机对时，要给触摸屏连续对时10秒钟，确保触摸屏对时成功  ccg 2014.5.22
bool Clear_Data =false;				//判断是否要判断时间，根据时间判断是否要清除前一天的数据
//-------------------------------------------------------------------------------
//task_net.c
S_ModeBusBody_T *g_modbus_ss[TCP_MODBUS_LINK_NUM]; //modbus从机接入信息
S_IEC104Body_T *g_iec104_ss[TCP_IEC104_LINK_NUM];  //ice104从机接入信息
//-------------------------------------------------------------------------------
//iec104rtu.c
ChangedSignal CS[100];
uint64_t OldFault=0;
uint32_t OldAlarm=0;
uint16_t OldEvent=0;
uint64_t OldOther=0;
uint8_t  OldYX001=0;
uint8_t  OldYX042=0;
//--------------------------------------------------------------------------------
//EMS.c
PCS_Ctrl_Data_T  PCS_Ctrl_Data[PCS_NUM];		//PCS数据控制
PCS_Ctrl_Data_T  PCS_Ctrl_Data_Rec[PCS_NUM];    //PCS数据控制


PCS_Ind_Ctrl_T   PCS_Ind_Ctrl[PCS_NUM];			//PCS单机控制
//PV_Ctrl_Data_T   PV_Ctrl_Data[PV_NUM];			//PV数据控制
PV_Ind_Ctrl_T    PV_Ind_Ctrl[PV_NUM];			//PV单机控制
PV_Ctrl_Data_T   PV_Ctrl_Data[PV_NUM];			//PV数据控制
DCDC_Ind_Ctrl_T  DCDC_Ind_Ctrl[DCDC_NUM];		//DCDC单机控制
DCDC_Ctrl_Data_T DCDC_Ctrl_Data[DCDC_NUM];		//DCDC数据控制

Sys_Ctrl_T      Sys_Ctrl;						//系统控制

bool PCS_Com_Err[PCS_NUM]={false};				//单台PCS通讯故障标志
bool PCS_Com_OK[PCS_NUM]={false};
bool PV_Com_Err[PV_NUM]={false};				//单台PV通讯故障标志
bool PVS_Com_Err[PVS_NUM]={false};				//单台PVS通讯故障标志
bool BMS_Com_Err[BMS_NUM]={false};              //单台BMS通讯故障标志
bool BMS_Com_OK[BMS_NUM]={false};
bool DCDC_Com_Err[DCDC_NUM]={false};			//单台PV通讯故障标志
bool Meter_Com_Err[Meter_NUM]={false};			//单台PV通讯故障标志
bool PCS_Ready_Status[PCS_NUM]={false};         //PCS故障状态检查
bool DSP_Com_Err[DSP_NUM]={false};              //单台DSP通讯故障标志
bool ENV_Com_Err[ENV_NUM]={false};				//单台环境检测仪通讯故障标志

bool Sys_Start_Flag=false;						//系统启动标志
bool Sys_Stop_Flag=false;						//系统停机标志
bool Island_Start_Flag=false;					//孤岛启动标志
bool Grid_Start_Flag=false;				        //并网启动标志
bool Sys_Running_Flag=false;					//系统运行标志
bool Reset_Require=false;						//系统停机完成
bool EMG_Stop=false;							//紧急停机
bool Fault_Once=false;							//只上报一次故障

//uint8_t EMS_Run_Mode;							//运行模式 0-电网优先；1-微网优先
//uint8_t EMS_Start_Type;						//启动类型 0-主动模式；1-被动模式
//uint8_t Bus_Type;							    //母线类型:0-直流母线；1-交流母线
uint8_t Grid_Connect_Type=1;					//是否并网类型:0-非并网型；1-并网型
//uint8_t DC_Bus_Charge_Permit;				    //直流母线是否允许充电

uint16_t EMG_Counter=0;							//紧急停机计数器
uint8_t Load_Power_Status=0;					//负载供电状态 0-无供电；1-电网供电；2-微网供电。
uint8_t Grid_Pic_Direction=0;					//电网图片滚动方向
uint8_t PCS_Pic_Direction=0;					//PCS图片滚动方向
uint8_t PV_Pic_Direction=0;					 	//PV图片滚动方向
uint8_t Load_Pic_Direction=0;					//负载图片滚动方向
uint8_t MGrid_Pic_Direction=0;				 	//微网图片滚动方向
uint8_t EMS_RL_Mode=0;						 	//EMS远程就地模式 0-远程和本地；1-远程；2-本地
uint8_t IO_Set_Touch_Req=0;					 	//计量板对时触摸屏
uint8_t Touch_Set_Time_Flag=0;				 	//触摸屏对时计量板
uint8_t Allow_Load_Default=0;					//允许恢复出厂设置

//Island_Start
uint16_t IS_Flag=0;
uint16_t IS_Counter[PCS_NUM]={0};

uint16_t Start_Fail_CNT=0;						//启动失败计数
//Grid_Connected_Start
uint16_t GC_Flag=0;
uint16_t GC_Counter[PCS_NUM]={0};

//Self_Check
uint16_t SC_Flag=0;
uint16_t SC_Counter[PCS_NUM]={0};

bool Self_Check_OK=false;
bool PCS_Ready=false;

//System_Stop
uint16_t SS_Flag=0;
uint16_t SS_Counter[PCS_NUM]={0};

//Alarm_Check
uint16_t AC_Counter[PCS_NUM]={0};

//Fault_Check
uint16_t FC_Counter[PCS_NUM]={0};

//System_Logic
uint8_t Grid_First=DISABLE;								//电网优先标志
uint8_t Island_First=DISABLE;							//微网优先标志

//Grid And MGrid Contactor State
bool Grid_Contactor_Control_State = OP_LOW;
bool MGrid_Contactor_Control_State = OP_LOW;
//bool Run_Light_Control_State;
//bool Fault_Light_Control_State;

//SOC_Status
uint16_t SOC_Counter[PCS_NUM]={0};
uint16_t SOC_Counter1[PCS_NUM]={0};
uint16_t SOC_Counter2[PCS_NUM]={0};
uint16_t SOC_Counter3[PCS_NUM]={0};
uint16_t SOC_Counter4[PCS_NUM]={0};
uint16_t SOC_Counter5[PCS_NUM]={0};

Bat_Status_T Bat_Status;						//电池状态

//Running_Control
uint16_t RC_Counter[PCS_NUM]={0};

//Switch_to_Grid
bool Switch_to_Grid_Status=false;
uint16_t Switch_to_Grid_Flag=0;
uint16_t StG_Counter[PCS_NUM]={0};

//Switch_to_Island
bool Switch_to_Island_Status=false;
uint16_t Switch_to_Island_Flag=0;
uint16_t StI_Counter[PCS_NUM]={0};

//AGC_Control
uint16_t AGC_Counter[PCS_NUM]={0};

uint16_t PV_OK_DC_V=2500;

//-------------------------------------------------------------------------------
//webserver
/*LogTime R_time;

char p_000[17000];    //曲线图数据
char a_000[80];      //ajax确认回复
char s_000[1024];     //SetValue();


uint16_t Param_flag[PARAMETER_NUM];      	//保存要保存的参数值和保存标识
uint16_t Param_value[PARAMETER_NUM];     	//保存要保存的参数值和保存标识
uint8_t Webserver_Language=0xAA;         	//0x55:中文 0xAA：English

char jsCharts_temp[500];					//用于数据缓存

uint16_t Param_flag[PARAMETER_NUM];     	//保存要保存的参数值和保存标识
uint16_t Param_value[PARAMETER_NUM];    	//保存要保存的参数值和保存标识
uint8_t save_flag_int=0;                	//异步判断是否保存参数值的标识(数值型)
*/

//-------------------------------------------------------------------------------
//commom_data.c
//uint16_t System_Power_Set = SYS_POWER_SET_DFT;

const uint16_t Device_Type_Code[DEVICE_NUM] = //设备类型编码
{
    0x0b08,                         //SC30
    0x0b04,                         //SC50
    0x0b01,                         //SC100
    0x0b02,                         //SC250
    0x0b06,                         //SC500
    0x0b03,                         //SC500TL
	0x0b09,                         //SC1000TL
    0x0b0a							//PMD-AI100k
};

const Param_Range P_Range[PCS_Power_Max]=   //参数范围和默认值
{
	{
		//PCS-30K/50K
		{1,PCS_NUM,1},					{1,PV_NUM,1},	                {1,BMS_NUM,1},              {1,DCDC_NUM,1},			{1,Meter_NUM,3},	{1,PVS_NUM,1},
		{0,1000,950}, 				    {0,1000,900},					{0,1000,200},				{0,1000,100},
		{0,1000,250},					{0,1000,150},					{0,120,4},					{0,120,4},
		{1950,6500,6500},				{1950,6500,6000},				{1950,6500,2260},			{1950,6500,1980},
		{1950,6500,2500},				{1950,6500,2200},				{0,120,4},					{0,120,4},
		{3700,4100,4000},				{4950,5050,5000},				{1950,6500,3200},			{0,2820,0},		
		{-1000,1000,1000},				{-1000,1000,0},
	},
	{
		//PCS-100K
		{1,PCS_NUM,1},					{1,PV_NUM,1},	                {1,BMS_NUM,1},              {1,DCDC_NUM,1},			{1,Meter_NUM,3},	{1,PVS_NUM,1},
		{0,1000,950}, 				    {0,1000,900},					{0,1000,200},				{0,1000,100},
		{0,1000,250},					{0,1000,150},					{0,120,4},					{0,120,4},
		{5000,8000,8000},				{5000,8000,7500},				{5000,8000,5500},			{5000,8000,5000},
		{5000,8000,5800},				{5000,8000,5400},				{0,120,4},					{0,120,4},
		{3700,4100,4000},				{4950,5050,5000},				{5000,8000,8000},			{0,2200,0},		
		{-1000,1000,1000},				{-1000,1000,0},
	},
    {
        //PCS-250K
        {1,PCS_NUM,1},                  {1,PV_NUM,1},                   {1,BMS_NUM,1},              {1,DCDC_NUM,1},			{1,Meter_NUM,3},	{1,PVS_NUM,1},
		{0,1000,950}, 				    {0,1000,900},					{0,1000,200},				{0,1000,100},
		{0,1000,250},					{0,1000,150},					{0,120,4},					{0,120,4},
        {5000,8500,8000},               {5000,8500,7500},               {5000,8500,5500},           {5000,8500,5000},
        {5000,8500,5800},               {5000,8500,5400},               {0,120,4},                  {0,120,4},
        {3700,4100,4000},               {4950,5050,5000},               {5000,8500,8000},           {0,2200,0},
        {-1000,1000,1000},              {-1000,1000,0},
    },
    {
        //PCS-500
        {1,PCS_NUM,1},                  {1,PV_NUM,1},                   {1,BMS_NUM,1},              {1,DCDC_NUM,1},			{1,Meter_NUM,3},	{1,PVS_NUM,1},
		{0,1000,950}, 				    {0,1000,900},					{0,1000,200},				{0,1000,100},
		{0,1000,250},					{0,1000,150},					{0,120,4},					{0,120,4},
        {5000,8500,8000},               {5000,8500,7500},               {5000,8500,5500},           {5000,8500,5000},
        {5000,8500,5800},               {5000,8500,5400},               {0,120,4},                  {0,120,4},
        {3700,4100,4000},               {4950,5050,5000},               {5000,8500,8000},           {0,2200,0},
        {-1000,1000,1000},              {-1000,1000,0},
    },
    {
        //PCS-500TL
        {1,PCS_NUM,1},                  {1,PV_NUM,1},                   {1,BMS_NUM,1},              {1,DCDC_NUM,1},			{1,Meter_NUM,3},	{1,PVS_NUM,1},
		{0,1000,950}, 				    {0,1000,900},					{0,1000,200},				{0,1000,100},
		{0,1000,250},					{0,1000,150},					{0,120,4},					{0,120,4},
        {5000,8500,8000},               {5000,8500,7500},               {5000,8500,5500},           {5000,8500,5000},
        {5000,8500,5800},               {5000,8500,5400},               {0,120,4},                  {0,120,4},
        {2800,3300,3150},               {4950,5050,5000},               {5000,8500,8000},           {0,2200,0},
        {-1000,1000,1000},              {-1000,1000,0},
    },
    {
        //PCS-1000K
        {1,PCS_NUM,1},                  {1,PV_NUM,1},                   {1,BMS_NUM,1},              {1,DCDC_NUM,1},			{1,Meter_NUM,3},	{1,PVS_NUM,1},
		{0,1000,950}, 				    {0,1000,900},					{0,1000,200},				{0,1000,100},
		{0,1000,250},					{0,1000,150},					{0,120,4},					{0,120,4},
        {5000,8500,8000},               {5000,8500,7500},               {5000,8500,5500},           {5000,8500,5000},
        {5000,8500,5800},               {5000,8500,5400},               {0,120,4},                  {0,120,4},
        {3700,4100,4000},               {4950,5050,5000},               {5000,8500,8000},           {0,2200,0},
        {-1000,1000,1000},              {-1000,1000,0},
    }	
};

//uint8_t PCS_Type = 0;
uint8_t g_adjust_flag = 0;//是否是校表状态//lixia;2012.12.20;
uint8_t	IOD_Poweron=0;

//PID电压控制参数
uint16_t wInverterAcVoltDQKp,wInverterAcVoltDQKp_PVLimit,wInverterAcVoltDQKi_PVLimit,wInverterAcVoltDQKi,Balancing_Kp; //KP KI
_SPID_T sPid[PCS_NUM+1];
//回调参数
uint16_t Parameter_recovery_kp=0;
uint16_t Parameter_recovery_dalay_time=0;
//最大逆流功率
int16_t Max_anti_reflux_power=0;
//光伏广播使能
uint16_t Send_Data_To_PV_Broadcast_Enable=0;
//曲线保存时间设置
uint16_t Time_900_seconds = 900;


//系统配置参数
S_System_Info_T System_Info;    //系统参数
S_System_Run_Info_T System_Run_Info;  //系统运行参数
S_Com_Data_T Com_Data;         //公共参数
S_EMS_Info_T EMS_Info;				//EMS控制参数
S_PCS_Info_T PCS_Info[PCS_NUM];		//储能逆变器参数

S_PV_Info_T PV_Info[PV_NUM];		//光伏逆变器参数
S_DCDC_Info_T DCDC_Info[DCDC_NUM];	//光伏控制器参数
S_AMMETER_Info_T AMMETER_Info[Meter_NUM];
VSG_Info_ST VSG_Info;	            //VSG相关信息
S_PVS_Info_T PVS_Info[PVS_NUM];		//光伏汇流箱参数  2016.9.17 lk
S_ENV_Info_T ENV_Info[ENV_NUM];		//环境仪参数  2016.9.19 lk
S_IAT_Info_T IAT_Info;         //IAT协议

//GATTPara gATTPara;         //ATT7022B相关参数//lixia;2012.12.13;
TGBuf gBuf;             //全局缓冲//lixia;2012.10.30
gt_time showTime;

char P1[16]="EMS200";
char P2[20]="V10";
char P3[30]="LCD_EMS200-V11_V1_A_M";
char P4[30]="TOUCH_EMS200-V11_V1_A_M";
char P5[30]=""; //DSP_EMS200-V11_V1_A_M
char P6[30]="V1.0.2017.11.08";

char* const Device_Info[]={P1,P2,P3,P4,P5,P6};

