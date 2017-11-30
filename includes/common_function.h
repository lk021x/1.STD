#ifndef __HISTORY_INFORM_H
#define __HISTORY_INFORM_H

bool Check_Sn(char buf[],uint8_t len);                              //检测序列号是否格式有误
uint8_t Sn_Num(char* sn,uint8_t num);                               //检测序列号的有效位数

int get_sys_time(void);                                            	//读取系统时间
int set_sys_time(void);												//设置系统时间

uint32_t GetU32(void* value);										//返回U32
uint16_t GetU16(void* value);										//返回U16
void SetU16(void* value, uint16_t fact);							//设置U16
uint8_t* devices_savedata(uint8_t *data,uint16_t value);			//设置U16到buf
uint16_t Caculate_Value16(uint8_t Value_H,uint8_t Value_L);			//

uint8_t  BcdToBin(uint8_t n);										//BCD转16进制
uint8_t  BinToBcd(uint8_t n);										//16进制转BCD

uint32_t Get_Protocol_ID(uint8_t protocol);                         //获取协议号
uint32_t Get_Protocol_Version(uint8_t protocol);                    //获取协议版本

void Get_Local_Mac(NetParam *param, char *ETH);						//获取网卡MAC
int SetNetParam(NetParam param, char *ETH);							//保存网卡参数
void Read_Web_History_Data();										//读网页历史参数

void Global_Variable_Init(void);                                    //变量初始化
void Next_Day_Data_Process(void);                                   //到第二天的一些数据处理

void Load_Default_RunInfo_Data(void);								//加载默认运行信息数据    被 Read_History_Data（）调用
void Read_History_RunInfo_Data(void);								//读取历史运行信息数据    被 Read_History_Data（）调用
void Read_History_Data(void);                                  		//读取历史数据

void Load_Default_Process(void);                                    //恢复默认值
void Load_Default_Net_Process(void);								//恢复网络默认值

void Save_Power_To_Buffer(int32_t *regbuf1, int32_t buf1, int32_t *regbuf2, int32_t buf2, int32_t *regbuf3, int32_t buf3,int32_t *regbuf4, int32_t buf4);

void Write_Day_Curve_Array(void);									//记录当天曲线, from eeprom
void Read_Day_Curve_Array(void);									//读当前曲线, from eeprom
void Clear_Day_Curve_Array(void);									//清当前曲线

void Write_Histroy_RunRecord(void);									//写运行记录到 running_info
void Write_Histroy_Curve_Data(void);                                //存储历史曲线数据  time_of_use,peak_valley,peak_valley,ac_limiting,output_smooth,fm

void Set_Dev_ID(SERIAL_PROTOCOL_TYPE protocol, int stID);
#endif
