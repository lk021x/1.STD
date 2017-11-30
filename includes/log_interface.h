/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : log_interface.h
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : Log CBB底层数据访问接口声明头文件
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#ifndef _LOG_INTERFACE_H
#define _LOG_INTERFACE_H

//日志存储的都是历史的
#define VALID_CURVE_POINT_NUM 						96  //功率曲线点个数
#define VALID_HOUR_NUM        						24  //
#define VALID_DAY_NUM         						31  //
#define VALID_MONTH_NUM       						12  //
#define VALID_YEAR_NUM        						25  //

//标记日志数据类型
typedef enum
{
    E_RUNRECORD_LOG      							= 1,//运行记录
    E_TIME_OF_USE_CURVE_LOG							= 2,//分时电价
    E_PEAKVALLEY_CURVE_LOG    						= 3,//消峰填谷功率曲线
    E_AC_LIMITED_PRODUCE_CURVE_LOG 					= 4,//交流限发功率曲线
    E_OUTPUT_SMOOTH_CURVE_LOG						= 5,//输出平滑
    E_FM_CURVE_LOG									= 6,//调频
    E_MGRID_CURVE_LOG								= 7//微网曲线
}E_LOG_T;

//100天运行记录
#define EVERY_RUNRECORD_DATA_LEN   					116
#define EVERY_RUNRECORD_BUF_LEN   					(EVERY_RUNRECORD_DATA_LEN +12)   //2(长度)+2(序号)+6(时间戳)+ 116(运行记录)+2（CRC）
#define RUNRECORD_MAX_NUM     						(25*365*24*60) //25年 1min记录一次

//分时电价曲线, < 12M
#define EVERY_DAY_TIME_OF_USE_DATA_LEN				1152
#define EVERY_DAY_TIME_OF_USE_CURVE_BUF_LEN   		(EVERY_DAY_TIME_OF_USE_DATA_LEN + 12) //2(长度)+2(序号)+6(时间戳)+ 96x3x2(曲线数据)+2（CRC）
#define RUNRECORD_DAY_MAX_NUM 						(24*60)  //一天最大记录数
#define DAY_TIME_OF_USE_CURVE_MAX_NUM       		(25*365)   //25年 1天记录一次

//消峰填谷曲线, < 12M
#define EVERY_DAY_PEAKVALLEY_CURVE_DATA_LEN			1152
#define EVERY_DAY_PEAKVALLEY_CURVE_BUF_LEN   		(EVERY_DAY_PEAKVALLEY_CURVE_DATA_LEN + 12) //2(长度)+2(序号)+6(时间戳)+ 96x3x2(曲线数据)+2（CRC）
#define DAY_PEAKVALLEY_CURVE_MAX_NUM       			(25*365)   //25年 1天记录一次

//交流限发曲线, < 12M
#define EVERY_DAY_ACLIMIT_CURVE_DATA_LEN			1152
#define EVERY_DAY_ACLIMIT_CURVE_BUF_LEN   			(EVERY_DAY_ACLIMIT_CURVE_DATA_LEN + 12) //2(长度)+2(序号)+6(时间戳)+ 96x3x2(曲线数据)+2（CRC）
#define DAY_ACLIMIT_CURVE_MAX_NUM       			(25*365)   //25年 1天记录一次

//输出平滑曲线, < 153M
#define EVERY_DAY_OUTPUT_SMOOTH_CURVE_DATA_LEN		17280
#define EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN   			(EVERY_DAY_OUTPUT_SMOOTH_CURVE_DATA_LEN + 12) //2(长度)+2(序号)+6(时间戳)+ 96x3x2(曲线数据)+2（CRC）
#define DAY_OUTPUT_SMOOTH_CURVE_MAX_NUM       		(25*365)   //25年 1天记录一次

//调频曲线, < 12M
#define EVERY_DAY_FM_CURVE_DATA_LEN					1152
#define EVERY_FM_BUF_LEN   							(EVERY_DAY_FM_CURVE_DATA_LEN + 12) //2(长度)+2(序号)+6(时间戳)+ 96x3x2(曲线数据)+2（CRC）
#define DAY_FM_CURVE_MAX_NUM       					(25*365)   //25年 1天记录一次

//微网曲线, < 12M
#define EVERY_DAY_MGRID_CURVE_DATA_LEN				1152
#define EVERY_DAY_MGRID_CURVE_BUF_LEN   			(EVERY_DAY_MGRID_CURVE_DATA_LEN + 12) //2(长度)+2(序号)+6(时间戳)+ 96x3x2(曲线数据)+2（CRC）
#define DAY_MGRID_CURVE_MAX_NUM       				(25*365)   //25年 1天记录一次

// 时间戳记录结构体，查询用
typedef struct
{
    uint8_t Year;
    uint8_t Month;
    uint8_t Date;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
} LogTime;

// Log CBB底层接口声明 --------------------------------------------------------
void log_init(void);//LOG模块初始化
void do_attr_store(uint32_t addr, uint8_t *data, uint16_t len);
void do_attr_extract(uint32_t addr, uint8_t *data, uint16_t len);
void do_record_store(uint8_t host, uint32_t addr, uint8_t *data, uint16_t len);
void do_record_extract(uint8_t host, uint32_t addr, uint8_t *data, uint16_t len);
uint16_t do_record_erase(uint8_t host, uint32_t addr, uint32_t len);
void getTime(LogTime *time);
uint32_t getCRC32(volatile uint8_t *ptr, uint16_t len);
void setLogLastDaytime(LogTime *time);//设置存储记录的时间

#endif
