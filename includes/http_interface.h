#ifndef __HTTP_INTERFACE_H__
#define __HTTP_INTERFACE_H__

typedef struct 
{
	uint8_t  pcs;			//PCS界面
	uint8_t  pv;			//PV界面
	uint8_t  dcdc;			//DCDC界面
	uint8_t  pvs;			//PVS界面
	uint8_t  bms;			//BMS界面
	uint8_t  ammeter;		//电表界面
	uint8_t  realtimeMonitor;//实时监控界面
	uint8_t  batteryctrl;   //电池控制界面
	uint8_t  env;			//环境检测仪界面
}gt_web_page_show;

#define MAX_CLIENT_NUM	3

//-------------------------------------------------------------------------------
//http_interface.c
extern struct h_resource h_Rsc[];
extern struct j_resource j_Rsc[];
extern struct c_resource c_Rsc[];
extern struct i_resource i_Rsc[];
extern struct s_resource s_Rsc[];
extern struct a_resource a_Rsc[];
extern struct p_resource p_Rsc[];

extern LogTime R_time;

extern char p_000[6*1024*1024];    			//曲线图数据
extern char a_000[80];      				//ajax确认回复
extern char s_000[4096];     				//SetValue();
extern char jsCharts_temp[4096];			//用于数据缓存

extern uint8_t Cur_PCS_Num;
extern uint8_t Cur_PV_Num;
extern uint8_t Cur_PVS_Num;
extern uint8_t Cur_DCDC_Num;
extern uint8_t Cur_BMS_Num;
extern uint8_t Cur_AMMETER_Num;

extern uint16_t Param_flag[100];  			//保存要保存的参数值和保存标识
extern int32_t Param_value[100]; 			//保存要保存的参数值和保存标识
extern uint8_t Webserver_Language;         	//0x55:中文 0xAA：English
extern uint8_t ExportST;					//数据导出状态 0 - NO, 1 - YES
extern gt_web_page_show webPageShow;

extern uint8_t save_flag_int;               //异步判断是否保存参数值的标识(数值型)

extern const char* Event_Name[36][2];
extern const char* Fault_Name[68][2];
extern const char* Alarm_Name[36][2];
extern const char* Export_Name[31][2];

extern bool Serial_Param_Change[];
extern bool PCS_Svr_Param_Change[];
extern bool PV_Svr_Param_Change[];

uint32_t return_max_u32(uint32_t *arr,uint8_t k,uint8_t num);	//返回32位无符号数中，最大的数
void set_param(uint16_t num, uint16_t id, int32_t val); 		//设置参数数组的保存值
uint16_t do_save_data_int(uint16_t id,int32_t INT32_Val);		//设置参数
uint32_t get_param_by_id(uint32_t addr, uint8_t id);			//获取参数
float get_realtimedata_by_id(uint32_t addr);     				//获取实时数据
void webserver_do_save_int(void);								//异步保存参数值(数值型)
void s_handler(int id,char* data,char *ip);						//处理初始化参数值
void a_handler(int id,char* data,char *ip);						//处理返回的参数，并回应
void p_handler(int id,char* data);								//处理曲线图和柱状图的参数请求
void r_handler(int id,char* data);								//导出历史记录（运行记录、故障记录）
void his_handler(uint8_t type,char* data);					    //导出历史记录
#endif
