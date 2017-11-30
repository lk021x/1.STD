/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : log.h
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : Log CBB 访问接口声明，日志属性和记录的存储首地址定义，记录队
 *                  个数设定，CRC功能设定。该模块不随日志队列个数增加而消耗RAM，
 *                  属性信息占RAM 0x20B，每个队列的属性信息占用非易失存储设备空
 *                  间：0x18B * RECORD_QUEUE_NUM
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2013.02.27 : 1.0.2 : Linfei
 * Modification   : 初次使用时统一对存储体擦除改为分块
 *------------------------------------------------------------------------------
 * 2011.11.29 : 1.0.1 : Linfei
 * Modification   : 添加DSUCCESS和EFAULT宏。
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#ifndef _HISTORY_INFORM_H_
#define _HISTORY_INFORM_H_
#include "log_interface.h"

// 只读，勿修改 --------------------------------------------------------------
#define  CRC_ENABLE                     1           // CRC校验使能
#define  CRC_DISABLE                    0           // CRC校验失能
#define  SOFTWARE_CRC16                 1           // 软件校验，CRC16
#define  HARDWARE_CRC32                 2           // 硬件校验，CRC32

#ifndef  DSUCCESS
 #define  DSUCCESS                      0           // 操作成功
#endif

#ifndef  EFAULT
 #define  EFAULT                        (-1)        // 操作出错
#endif

#define  ATTR_CRC_ERROR                 (-3)        // 队列属性信息校验出错，损坏
#define  HOST_NUM_ERROR                 (-2)        // 属主号错误，不存在
#define  DAY_RECORD_FULL                (-4)        // 天记录满
#define  NO_RECORD_NUM_LIMIT        	  0         // 无最大天记录条数限制
#define  RECORD_NUM_LIMIT				  1         // 每天限制保存一条记录
// ---------------------------------------------------------------------------
//LOG模块支持多进程访问
#define LOG_OS_SUPPORT 					  1

// 日志属性信息存储首地址，2为属性信息头长度：0x55AA(2b)，勿修改
#define LOG_ATTR_BASE_ADDRESS          (LOG_ATTR_HEAD_ADDRESS + 2)


// 日志队列初始化结构
typedef struct
{
    uint8_t   host;                 // 设备类型
    uint16_t  each_record_bytes;    // 每条记录的字节数
    uint32_t  max_record_num;       // 最大记录条数
    uint16_t  max_day_record_num;   // 每天记录的最大条数
} UserLogAttr;

// 用户设定 ------------------------------------------------------------------

// 日志属性信息存储首地址，属性信息头(0x55AA)+队列属性信息1+队列属性信息2+...
#define  LOG_ATTR_HEAD_ADDRESS          LOG_ATTR_ADDRESS

// 日志记录存储首地址 存入文件，注意: 每种曲线单独使用一个文件存放(使用追加的方式)
#define  LOG_RECORD_BASE_ADDRESS        0

// 记录队列个数, 运行信息/分时电价/消峰填谷曲线/交流限发曲线/输出平滑/调频/微网曲线
#define  RECORD_QUEUE_NUM               7

// 校验设定
#define  CRC_STATUS                     CRC_ENABLE       // CRC是否开启
#define  CRC_TYPE                       SOFTWARE_CRC16   // CRC类型，软/硬校验

// 日志模块接口 -------------------------------------------------------------
int32_t  init_log(UserLogAttr *logattr);
int32_t  resume_single_log(uint8_t host, UserLogAttr *logattr);
int32_t  resume_all_log(UserLogAttr *logattr);

int32_t  store_record(uint8_t host, uint8_t *buf, uint16_t datalen, uint16_t buflen);
int32_t  get_record_num(uint8_t host);
int32_t  get_record_by_index(uint8_t host, uint16_t index, uint8_t *buf, uint16_t buflen);

int32_t  start_query(uint8_t host, LogTime *time, uint16_t pre_day, uint8_t *buf, uint16_t buflen);
uint32_t query_next(uint8_t *buf, uint16_t buflen);
uint32_t query_pre(uint8_t *buf, uint16_t buflen);
uint32_t query_rcd(uint16_t idx, uint8_t *buf, uint16_t buflen);
void end_query(void);

#endif

