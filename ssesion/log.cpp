/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : log.c
 * Author         : Linfei
 * Date           : 2011.11.03
 * Description    : Log CBB模块具体实现，包括日志队列定义以及初始化、存储、查询
 *                  功能实现，支持时间戳和索引号查询
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2013.02.27 : 1.0.2 : Linfei
 * Modification   : 初次使用时统一对存储体擦除改为分块
 *------------------------------------------------------------------------------
 * 2012.06.20 : 1.0.4 : Linfei
 * Modification   : 修复记录存储在EEPROM设备中记录条数返回有时少一条的bug
 *------------------------------------------------------------------------------
 * 2012.03.08 : 1.0.4 : Linfei
 * Modification   : 修复get_record_by_index函数中数据在无校验时误修改数据bug
 *------------------------------------------------------------------------------
 * 2011.11.29 : 1.0.3 : Linfei
 * Modification   : 增加恢复默认值接口restore_all_log，修改用户数据合法性检查，
 *                  放宽记录条数与记录字节数限制
 *------------------------------------------------------------------------------
 * 2011.11.09 : 1.0.2 : Linfei
 * Modification   : 更改get_record_num函数出错时的返回值；修复refresh_logbody在内
 *                  存副本与用户指定host相同时，不刷新logfd的bug
 *------------------------------------------------------------------------------
 * 2011.11.08 : 1.0.1 : Linfei
 * Modification   : 修改qurey系列函数，添加query_pre和query_rcd，更改query_next返
 *                  回值；修复store_record内存bug
 *------------------------------------------------------------------------------
 * 2011.11.02 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#include "includes.h"


/*lint -e506*/

// 有效属性信息头标志
#define  VALID_ATTR_HEAD_MARK           0x55AA

// CRC校验失败后的重试次数
#define  ATTR_CHECK_TRY_TIMES           5

// 记录年月日
typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
} RecTime;

// 日志模块主结构，包含日志所有属性信息
typedef struct
{
    uint16_t  crc;                 // CRC校验，保证该结构体数据合法
    uint8_t   host;                // 记录队列的属主，可以是设备编号
    uint8_t   fullFlag;            // linf 20120620 存储满标记，增加该标记后不会引起属性储存占用空间变化
    RecTime   lrtime;              // 上次记录的年月日，用于判断是否到达另一天
    uint32_t  base_addr;           // 记录存储基地址
    uint16_t  max_day_rnum;        // 最大天记录条数
    uint16_t  each_record_bytes;   // 每条记录的字节数
    uint16_t  max_record_num;      // 最大记录条数
    uint16_t  day_record_num;      // 天记录条数
    uint16_t  front;               // 记录存储头指针
    uint16_t  rear;                // 记录存储尾指针
} LogBody;

// 日志查询结构
typedef struct
{
    uint8_t     quering;                             // 正在查询标志
    RecTime     qtime;                               // 查询时间戳
    uint16_t    qstart_idx;                          // 对应起始时间戳的记录号
    uint16_t    qrcd_num;                            // 对应起始时间戳的查询记录数
    uint16_t    q_idx;                               // 当前查询记录号
    uint8_t     host;                                // 当前查询队属主
    uint16_t    each_record_bytes;                   // 每条记录的字节数
} LogQuery;

#if (LOG_OS_SUPPORT == 1)
LogBody  logbody[RECORD_QUEUE_NUM] = { {0} };
#else
LogBody  logbody = { 0 };
#endif

volatile LogQuery logquery = { 0 };

// 属主号与队列号的映射数组
uint8_t hostmap[RECORD_QUEUE_NUM] = { 0 };

// Log 模块内部函数声明 ------------------------------------------------------
static void     refresh_Queue_ptr(uint8_t logfd);
static void     store_log_attr(uint8_t logfd);
static void     load_log_attr(uint8_t logfd);
static void     init_single_log(uint8_t logfd, UserLogAttr *logattr, uint32_t base_addr);
static int32_t  refresh_logbody(uint8_t host, uint8_t *logfd);
static int32_t  get_ptr_by_index(LogBody *log_body, uint16_t index, uint16_t *ptr);
static uint32_t get_base_addr(uint8_t logfd, UserLogAttr *logattr);
static int32_t  getfd_byhost(uint8_t host, uint8_t *fd);
static uint16_t record_num(LogBody *log_body);
static uint32_t total_record_num(LogBody *log_body);
static uint16_t getCRC16(volatile uint8_t *ptr, uint16_t len);
static uint8_t  crccheck(uint8_t *pbuf, uint16_t buflen, uint8_t crctype);
// ---------------------------------------------------------------------------

/********************************************************************************
 * Function       : init_log
 * Author         : Linfei
 * Date           : 2011.11.02
 * Description    : 检查存储设备中是否有日志属性信息，没有则根据用户配置logattr初始
 *                  化日志属性信息，有则返回
 * Calls          : do_attr_extract, init_single_log, do_attr_store, store_log_attr
 *                  record_num
 * Input          : None
 * Output         : None
 * Return         : None
 *********************************************************************************
 * History        :
 *--------------------------------------------------------------------------------
 * 2011.11.30 : 1.0.2 : Linfei
 * Modification   : 修改用户数据合法性检查，加强报错机制，记录条数不受限，单记录字
 *                  节数需满足：STORE_SIZE_ADJUST模取记录字节数等于0
 *--------------------------------------------------------------------------------
 * 2011.11.29 : 1.0.1 : Linfei
 * Modification   : 修改用户数据合法性检查，放宽记录条数与记录字节数限制
 *--------------------------------------------------------------------------------
 * 2011.11.02 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 ********************************************************************************/
int32_t init_log(UserLogAttr *logattr)
{
    if(logattr == NULL)
    {
        return EFAULT;
    }
    uint8_t i;
    uint8_t head[2] = { 0 };
    
    // 验证属性信息有效
    for(i = 0; i < RECORD_QUEUE_NUM; i++)
    {
        if(logattr[i].host == 0 || logattr[i].each_record_bytes == 0 || logattr[i].max_record_num == 0)
        {
            return EFAULT;
        }
    }
    
    // 初始化RAM中的host映射数组，在验证合法性后分开进行，这样可以在用户未发现初始化失败时，模块后续操作都能报错
    for(i = 0; i < RECORD_QUEUE_NUM; i++)
    {
        hostmap[i] = logattr[i].host;  //备份主属性序列号
    }
    
    // 存储eeprom中的属性信息头校验, 如果读出的是0x55AA说明已经有历史数据，无需初始化属性, 否则需要初始化属性
    i = ATTR_CHECK_TRY_TIMES;
    while(i != 0)
    {
        // 读取日志属性信息头2个字节
        do_attr_extract(LOG_ATTR_HEAD_ADDRESS, head, 2);
        // 属性信息头校验成功，表明存储设备中已存有日志属性信息，无需重新初始化
        if((head[0] + (head[1]<<8)) == VALID_ATTR_HEAD_MARK)
        {
            return DSUCCESS;
        }
        i--;
    }
    // 至此，表明存储设备中的日志属性信息未初始化或已损坏，重新初始化存储设备中的日志
    // 属性信息，代码执行到此时较危险，所有已有记录将会被擦除
    resume_all_log(logattr);
    
    head[0] = (VALID_ATTR_HEAD_MARK) & 0xFF;
    head[1] = (VALID_ATTR_HEAD_MARK>>8) & 0xFF;
    do_attr_store(LOG_ATTR_HEAD_ADDRESS, head, 2);
    return DSUCCESS;
}
/********************************************************************************
 * Function       : restore_all_log
 * Author         : Linfei
 * Date           : 2011.11.29
 * Description    : 日志队列恢复默认，清除所有日志队列中的数据
 * Calls          : init_single_log, store_log_attr
 *                  record_num
 * Input          : None
 * Output         : None
 * Return         : None
 *********************************************************************************
 * History        :
 *--------------------------------------------------------------------------------
 * 2011.11.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 ********************************************************************************/
int32_t resume_all_log(UserLogAttr *logattr)
{
    uint8_t i = 0;

    //printf("resume all log\n");
    for(i = 0; i < RECORD_QUEUE_NUM; i++)
    {
        init_single_log(i, &logattr[i], 0);
        store_log_attr(i);
    }
    return DSUCCESS;
}
/********************************************************************************
 * Function       : restore_single_log
 * Author         : Linfei
 * Date           : 2011.11.16
 * Description    : 出错时使用，重新初始化某一单个日志队列属性，重新初始化后，该队
 *                  列中的所有已存数据都将被擦除
 * Calls          : getfd_byhost, init_single_log, get_base_addr, store_log_attr
 * Input          : host：需要重新初始化的属主编号
 *                  logattr：初始化日志队列使用的初始化配置，必须与调用init_log
 *                  函数时传入的logattr参数一致
 * Output         : None
 * Return         : HOST_NUM_ERROR：初始化失败，属主编号不存在；
 *                  DSUCCESS：重新初始化成功
 *********************************************************************************
 * History        :
 *--------------------------------------------------------------------------------
 * 2011.11.02 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 ********************************************************************************/
int32_t resume_single_log(uint8_t host, UserLogAttr *logattr)
{
    uint8_t logfd = 0;
    // 匹配日志队列编号
    if(getfd_byhost(host, &logfd) == EFAULT)
    {
        return HOST_NUM_ERROR;
    }
    init_single_log(logfd, &logattr[logfd], get_base_addr(logfd, logattr));
    store_log_attr(logfd);
    return DSUCCESS;
}
/********************************************************************************
 * Function       : get_log_body
 * Author         : Linfei
 * Date           : 2013.04.08
 * Description    : 获取logfd指定的host日志属性体
 * Input          : logfd：队列编号
 * Output         : None
 * Return         : 对应队列编号的指针
 *********************************************************************************
 * History        :
 *--------------------------------------------------------------------------------
 * 2013.04.08 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 ********************************************************************************/
LogBody *get_log_body(uint8_t logfd)
{
#if (LOG_OS_SUPPORT == 1)
    return &logbody[logfd];
#else
    return &logbody;
#endif
}
/********************************************************************************
 * Function       : store_record
 * Author         : Linfei
 * Date           : 2011.11.02
 * Description    : 将记录数据存储到存储设备中
 * Calls          : refresh_logbody, getTime, do_record_erase, record_num, memcpy
 *                  getCRC32, getCRC16, do_record_store, refresh_Queue_ptr
 * Input          : host：属主
 *                  buf：待存储数据的缓冲区首地址，缓冲区长度须大于单条记录块长度
 *                  datalen：待储存数据长度
 *                  buflen：缓冲区长度，须大于等于队列记录项长度
 * Output         : None
 * Return         : EFAULT：存储失败；DAY_RECORD_FULL：当天记录已满
 *                  DSUCCESS：存储成功
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2013.02.27 : 1.0.2 : Linfei
 * Modification   : 初次使用时统一对存储体擦除改为分块
 *------------------------------------------------------------------------------
 * 2011.11.02 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/ 
int32_t store_record(uint8_t host, uint8_t *buf, uint16_t datalen, uint16_t buflen)
{
    if(buf == NULL || datalen == 0)
    {
        return EFAULT;
    }
    uint8_t logfd = 0;
    uint32_t crc = 0;
    uint16_t i = 0;
    LogTime logtime = { 0 };
    int32_t ret = EFAULT;
    
    // 刷新RAM中的属主队列属性信息
    ret = refresh_logbody(host, &logfd);
    if(ret != DSUCCESS)
    {
        return ret;
    }
    LogBody *log_body = get_log_body(logfd);

    // 用户缓冲区长度太小
    if(buflen < log_body->each_record_bytes)
    {
        return EFAULT;
    }
   
    // 记录：数据长度(2B)+天内序号(2B)+时间戳(6B)+数据+校验位(0B/2B/4B)
#if CRC_STATUS == CRC_ENABLE
    #if CRC_TYPE == HARDWARE_CRC32
    if(10 + datalen + 4 > log_body->each_record_bytes)
    #else
    if(10 + datalen + 2 > log_body->each_record_bytes)
    #endif
#else
    if(10 + datalen > log_body->each_record_bytes)
#endif
    {
        return EFAULT;
    }

    //获取记录时间
    getTime(&logtime);

	// 与上次日志记录时间比较,判断是否是新的一天
	if(logtime.Date != log_body->lrtime.date ||
		logtime.Month !=  log_body->lrtime.month ||
		logtime.Year !=  log_body->lrtime.year)
	{
		//printf("host:%d, old: %d-%d-%d new: %d-%d-%d\n", host, log_body->lrtime.year,log_body->lrtime.month,log_body->lrtime.date,
				//logtime.Year, logtime.Month, logtime.Date);
		log_body->day_record_num = 0;
		log_body->lrtime.date  = logtime.Date;
		log_body->lrtime.month = logtime.Month;
		log_body->lrtime.year  = logtime.Year;
	}

	// 达到最大天记录条数限制则返回，不执行存储, 如果log_body->max_day_rnum == NO_RECORD_NUM_LIMIT则每天无最大记录数限制
	//printf("day_record_num %d, max_day_rnum %d\n", log_body->day_record_num, log_body->max_day_rnum);
	if(log_body->max_day_rnum != NO_RECORD_NUM_LIMIT)
	{
		if(log_body->day_record_num == log_body->max_day_rnum)
		{
			return DAY_RECORD_FULL;
		}
	}
    
    // 用户待存数据后移10B，空间留给记录头：数据长度(2B)+天内序号(2B)+时间戳(6B)
    for(i = datalen; i > 0; i--)
    {
        buf[i - 1 + 10] = buf[i - 1];
    }
    
    // 数据长度+天记录序号+时间
    buf[0] = (datalen + 8) & 0xFF;
    buf[1] = ((datalen + 8) >> 8) & 0xFF;
    buf[2] = (log_body->day_record_num +1) & 0xFF;
    buf[3] = ((log_body->day_record_num + 1) >> 8) & 0xFF;
    buf[4] = logtime.Year;
    buf[5] = logtime.Month;
    buf[6] = logtime.Date;
    buf[7] = logtime.Hour;
    buf[8] = logtime.Minute;
    buf[9] = logtime.Second;
    // CRC校验
#if CRC_STATUS == CRC_ENABLE
    #if CRC_TYPE == HARDWARE_CRC32
    crc = getCRC32(buf, log_body->each_record_bytes - 4);
    buf[log_body->each_record_bytes - 4] = crc & 0xFF;
    buf[log_body->each_record_bytes - 3] = (crc >> 8) & 0xFF;
    buf[log_body->each_record_bytes - 2] = (crc >> 16) & 0xFF;
    buf[log_body->each_record_bytes - 1] = (crc >> 24) & 0xFF;
    #else
    crc = getCRC16(buf, log_body->each_record_bytes - 2);
    buf[log_body->each_record_bytes - 2] = crc & 0xFF;
    buf[log_body->each_record_bytes - 1] = (crc >> 8) & 0xFF;
    #endif
#endif
    uint32_t store_addr = log_body->base_addr + log_body->front * log_body->each_record_bytes;
    do_record_store(host, store_addr, buf, log_body->each_record_bytes);
    refresh_Queue_ptr(logfd);
    log_body->day_record_num++;

    // 恢复buf中的数据为用户传入时状态，保证const
    for(i = 0; i < datalen; i++)
    {
        buf[i] = buf[i + 10];
    }
    for(i = 0; i < 10; i++)
    {
        buf[datalen + i] = 0;    
    }

#if CRC_STATUS == CRC_ENABLE
    #if CRC_TYPE == HARDWARE_CRC32    
    buf[log_body->each_record_bytes - 3] = 0;
    buf[log_body->each_record_bytes - 4] = 0;
#else
    buf[log_body->each_record_bytes - 1] = 0;
    buf[log_body->each_record_bytes - 2] = 0;
    #endif
#endif
    return DSUCCESS;
}
/********************************************************************************
 * Function       : get_record_num
 * Author         : Linfei
 * Date           : 2011.11.02
 * Description    : 根据属主号获取其记录条数
 * Calls          : getfd_byhost
 * Input          : host：属主
 * Output         : None
 * Return         : 属主队列中的记录数，出错返回0
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.11.09 : 1.0.0 : Linfei
 * Modification   : 出错返回错误号改为出错返回0
 *------------------------------------------------------------------------------
 * 2011.11.02 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
int32_t get_record_num(uint8_t host)
{
    int32_t ret = EFAULT;
    uint8_t logfd = 0;
    
    // 刷新RAM中的属主队列属性信息
    ret = refresh_logbody(host, &logfd);
    if(ret != DSUCCESS)
    {
        return 0;
    }
    LogBody *log_body = get_log_body(logfd);
    
    // linf 20120620 修改记录条数计算方式
    if(log_body->front >= log_body->rear)
    {
        if(log_body->fullFlag == 1)
        {
            return log_body->max_record_num;
        }
        else
        {
			if(log_body->front - log_body->rear > log_body->max_record_num)
			{
				return log_body->max_record_num;
			}
			else
			{
				return log_body->front - log_body->rear;
			}
        }
    }
    else
    {
        return log_body->max_record_num;
    }
}
/********************************************************************************
 * Function       : get_record_by_index
 * Author         : Linfei
 * Date           : 2011.11.02
 * Description    : 根据属主和索引号获取一条记录数据
 * Calls          : getfd_byhost, get_ptr_by_index, do_record_extract, 
 *                  crccheck, memcpy
 * Input          : host：属主号
 *                  index：索引号，从1开始编号
 *                  buf：读取到的记录存储缓冲区首地址
 *                  buflen：用户记录存储缓冲区长度，长度须大于单条记录块长度
 * Output         : buf：记录序号(2B)+时间戳(6B)+数据
 * Return         : 为非负数则是实际获取的记录条数，为负则表示出错
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.11.02 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
int32_t get_record_by_index(uint8_t host, uint16_t index, uint8_t *buf, uint16_t buflen)
{
    uint8_t logfd = 0;
    uint16_t i;
    int32_t ret = EFAULT;
    uint16_t extract_ptr = 0;
    
    if(index < 1 || buf == NULL || buflen == 0)
    {
        return EFAULT;
    }
    
    // 刷新RAM中的属主队列属性信息
    ret = refresh_logbody(host, &logfd);
    if(ret != DSUCCESS)
    {
        return ret;
    }
    LogBody *log_body = get_log_body(logfd);

    if(get_ptr_by_index(log_body, index, &extract_ptr) == EFAULT)
    {
        return 0;
    }
    
    uint32_t store_addr = log_body->base_addr + extract_ptr * log_body->each_record_bytes;
    // 执行记录提取操作
    do_record_extract(host,
    				  store_addr,
                      buf,
                      log_body->each_record_bytes);
        
#if CRC_STATUS == CRC_ENABLE
    #if CRC_TYPE == HARDWARE_CRC32
    if(!crccheck(buf, log_body->each_record_bytes, 1))
    #else
    //for(int i=0;i<log_body->each_record_bytes;i++) printf("%2x ", buf[i]);printf("\r\n");
    if(!crccheck(buf, log_body->each_record_bytes, 0))
    #endif
    {
        return 0;
    }
#endif
    uint16_t rcd_len = buf[0] + (buf[1] << 8);
    
#if CRC_STATUS == CRC_ENABLE    
    // 去除尾部校验，保证返回给用户的数据是纯的记录数据
    buf[log_body->each_record_bytes - 1] = 0;
    buf[log_body->each_record_bytes - 2] = 0;
    #if CRC_TYPE == HARDWARE_CRC32    
    buf[log_body->each_record_bytes - 3] = 0;
    buf[log_body->each_record_bytes - 4] = 0;
    #endif
#endif

    for(i = 0; i < rcd_len; i++)
    {
        buf[i] = buf[i + 2];
    }
    buf[rcd_len] = 0;
    buf[rcd_len + 1] = 0;

    return 1;
}
/********************************************************************************
 * Function       : start_query
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 初始化按时间戳查询操作，包括时间戳初始化和记录指针定位
 *                  注：start_query与end_query必须成对使用
 * Calls          : refresh_logbody, record_num, get_record_by_index,
 * Input          : host：属主号
 *                  time：查询时间戳
 *                  buf：用于启动查询函数内部使用
 *                  buflen：buf长度，不得小于当前单条查询记录的长度
 *                  pre_day：向前查询天数
 * Output         : None
 * Return         : 非负数：符合查询条件的剩余记录条数
 *                  EFAULT：参数错误
 *                  HOST_NUM_ERROR：属主号错误；
 *                  ATTR_CRC_ERROR：记录CRC错误，只返回错误，不处理
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
int32_t start_query(uint8_t host, LogTime *time, uint16_t pre_day,uint8_t *buf, uint16_t buflen)
{
    uint16_t cnt, rdnum;
    uint16_t rcd_num;
    uint8_t logfd = 0;
    int32_t  ret = EFAULT;
    
    if(time == NULL || pre_day == 0 || buf == NULL)
    {
        return EFAULT;
    }
    
    // 若已经处于查询状态，则返回出错
    if(logquery.quering == 1)
    {
        return EFAULT;
    }
    
    // 新的查询，刷新RAM中的属主队列属性信息
    ret = refresh_logbody(host, &logfd);
    if(ret != DSUCCESS)
    {
        return ret;
    }
    LogBody *log_body = get_log_body(logfd);
    
    // 用户缓冲区长度太小
    if(buflen < log_body->each_record_bytes)
    {
        return EFAULT;
    }
    
    // 若总记录条数为0，则返回
    rcd_num = record_num(log_body);
    if(rcd_num == 0)
    {
        return 0;
    }
    
    // 查询时间戳初始化
    logquery.qtime.year = time->Year;
    logquery.qtime.month = time->Month;
    logquery.qtime.date = time->Date;
    
    logquery.q_idx = 0;
    logquery.host = host;
    logquery.each_record_bytes = log_body->each_record_bytes;
   
    cnt = rcd_num;
    rdnum = 0;
    while(rdnum == 0 && cnt != 0)
    {
        rdnum = get_record_by_index(log_body->host,
                                    cnt,
                                    buf,
                                    buflen);
        // 读取到有效记录
        if(rdnum != 0)
        {
            // 与读取的记录年月日比较，相等则查询到首记录
            if(logquery.qtime.date == buf[4] &&
                    logquery.qtime.month == buf[3] &&
                    logquery.qtime.year == buf[2])
            {
            	//printf("get min %d: sec: %d\n", buf[5], buf[6]);
                logquery.qstart_idx = cnt;    //查询记录的起始编号
                logquery.quering = 1;
                if(buf[0] + (buf[1] << 8) > rcd_num)
                {
                    logquery.qrcd_num = rcd_num;   //查询记录数
                }
                else
                {
                    logquery.qrcd_num = buf[0] + (buf[1] << 8);  //查询记录数
                }
    
                // 继续搜索pre_day有多少条记录
                if(cnt >= buf[0] + (buf[1] << 8))
                {
                    cnt -= buf[0] + (buf[1] << 8);
                }
                else
                {
                    return logquery.qrcd_num;
                }
    
                pre_day--;
                rdnum = 0;
                uint16_t tmp = 0;
                while(pre_day != 0 && cnt != 0)
                {
                    rdnum = get_record_by_index(log_body->host,
                                                cnt,
                                                buf,
                                                buflen);
                    // 读取到有效记录
                    if(rdnum != 0)
                    {
                        if(cnt >= buf[0] + (buf[1] << 8))
                        {
                            cnt -= buf[0] + (buf[1] << 8);
                            logquery.qrcd_num += buf[0] + (buf[1] << 8);
                            pre_day--;
                        }
                        else
                        {
                            // 查询到队列尾，且有记录已过期，只保留未过期记录
                            logquery.qrcd_num += cnt;
                            break;
                        }
                    }
                    else
                    {
                        cnt--;
                        tmp++; // 失效记录计数
                    }
                }
                logquery.qrcd_num += tmp;
                return logquery.qrcd_num;
            }
            else
            {
                // 跳到下一天的首记录，若已无记录则返回0
                if(cnt >= buf[0] + (buf[1] << 8))
                {
                    cnt -= buf[0] + (buf[1] << 8);
                    rdnum = 0;
                }
                else
                {
                    return 0;
                }
            }
        }
        else
        {
            cnt--;
        }
    }
    return 0;
}
/********************************************************************************
 * Function       : query_next
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 获取下一条记录，必须在调用了start_query后使用
 * Calls          : get_record_by_index
 * Input          : buf：读取到的记录存储首地址
 *                  buflen：用户记录存储缓冲区长度
 * Output         : buf：读取到的记录数据
 * Return         : 1：读取到一条有效记录，0：该条记录无效
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.11.08 : 1.0.1 : Linfei
 * Modification   : 将其返回值由剩余记录条数改为该函数是否读取到有效记录
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint32_t query_next(uint8_t *buf, uint16_t buflen)
{
    if(logquery.quering == 0 || buf == NULL || buflen < logquery.each_record_bytes)
    {
        return 0;
    }
    
    uint8_t rdnum = get_record_by_index(logquery.host,
                                logquery.qstart_idx - logquery.q_idx,
                                buf,
                                buflen);
    logquery.q_idx = (logquery.q_idx + 1)%logquery.qrcd_num;
    return rdnum;
}
/********************************************************************************
 * Function       : query_pre
 * Author         : Linfei
 * Date           : 2011.11.08
 * Description    : 获取前一条记录，必须在调用了start_query后使用
 * Calls          : get_record_by_index
 * Input          : buf：读取到的记录存储首地址
 *                  buflen：用户记录存储缓冲区长度
 * Output         : buf：读取到的记录数据
 * Return         : 1：读取到一条有效记录，0：该条记录无效
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint32_t query_pre(uint8_t *buf, uint16_t buflen)
{
    if(logquery.quering == 0 || buf == NULL || buflen < logquery.each_record_bytes)
    {
        return 0;
    }
    
    uint8_t rdnum = get_record_by_index(logquery.host,
                                logquery.qstart_idx - logquery.q_idx,
                                buf,
                                buflen);
    if(logquery.q_idx > 0)
    {
        logquery.q_idx--;
    }
    else
    {
        logquery.q_idx = logquery.qrcd_num - 1;
    }
    return rdnum;
}
/********************************************************************************
 * Function       : query_rcd
 * Author         : Linfei
 * Date           : 2011.11.08
 * Description    : 获取一条记录，必须在调用了start_query后使用
 * Calls          : get_record_by_index
 * Input          : idx：指定的天内记录序号，即该天内的第几条记录，1 <= idx <= 天内
 *                       记录条数
 *                  buf：读取到的记录存储首地址
 *                  buflen：用户记录存储缓冲区长度
 * Output         : buf：读取到的记录数据
 * Return         : 1：读取到一条有效记录，0：该条记录无效
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint32_t query_rcd(uint16_t idx, uint8_t *buf, uint16_t buflen)
{
    if(logquery.quering == 0 || buf == NULL || buflen < logquery.each_record_bytes)
    {
        return 0;
    }

    if(idx < 1 || idx > logquery.qrcd_num)
    {
        return 0;
    }
    
    uint8_t rdnum = get_record_by_index(logquery.host,
                                logquery.qstart_idx - idx + 1,
                                buf,
                                buflen);
    return rdnum;
}
/********************************************************************************
 * Function       : end_query
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 结束查询，与start_query配对使用
 * Calls          : None
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void end_query(void)
{
    logquery.quering = 0;
    logquery.q_idx = 0;
/*
    logquery.qtime.year = 0;
    logquery.qtime.month = 0;
    logquery.qtime.date = 0;
    logquery.q_ptr = 0;
    logquery.qrcd_num = 0;
*/
}
/********************************************************************************
 * Function       : get_base_addr
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 获取指定队列日志的记录首地址
 * Calls          : None
 * Input          : logfd：队列号
 *                  logattr：队列属性初始化数组头指针
 * Output         : None
 * Return         : 相应队列的记录首地址
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static uint32_t get_base_addr(uint8_t logfd, UserLogAttr *logattr)
{
    uint32_t addr = LOG_RECORD_BASE_ADDRESS;  //每种记录放在单独的文件中，起始地址都是0
    return addr;
}
/********************************************************************************
 * Function       : refresh_logbody
 * Author         : Linfei
 * Date           : 2011.11.02
 * Description    : 刷新当前日志属性结构体
 * Calls          : getfd_byhost, load_log_attr, getCRC16
 * Input          : host：属主号
 *                  logfd：队列编号
 * Output         : None
 * Return         : DSUCCESS：刷新成功；EFAULT：参数错误
 *                  HOST_NUM_ERROR：属主号错误；
 *                  ATTR_CRC_ERROR：记录CRC错误
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.11.16 : 1.0.2 : Linfei
 * Modification   : 补充CRC校验尝试
 *------------------------------------------------------------------------------
 * 2011.11.09 : 1.0.1 : Linfei
 * Modification   : 修复在host不刷新情况下logfd不更新问题
 *------------------------------------------------------------------------------
 * 2011.11.02 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static int32_t refresh_logbody(uint8_t host, uint8_t *logfd)
{
    if(logfd == NULL)
    {
        return EFAULT;
    }
    uint16_t crc = 0xFFFF;
    // 匹配日志队列编号
    if(getfd_byhost(host, logfd) == EFAULT)
    {
        return HOST_NUM_ERROR;
    }
    LogBody *log_body = get_log_body(*logfd);
    
    // 若属主号与当前属主号相同则返回，不同需重新调取对应属主的队列属性信息
    if(host == log_body->host)
    {
        return DSUCCESS;
    }
    uint8_t i = ATTR_CHECK_TRY_TIMES;
    while(i != 0)
    {
        // 从存储设备中读取对应编号的队列属性信息
        load_log_attr(*logfd);
        // 检验出错则恢复logbody为默认
        crc = getCRC16((uint8_t *)log_body + 2, sizeof(LogBody) - 2);
        if(crc == log_body->crc)
        {
            return DSUCCESS;
        }
        i--;
    }
    return ATTR_CRC_ERROR;
}
/********************************************************************************
 * Function       : get_ptr_by_index
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 根据索引获取记录指针
 * Calls          : None
 * Input          : index：索引号
 * Output         : ptr：保存索引对应的记录指针
 * Return         : EFAULT：获取失败，DSUCCESS：获取成功
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static int32_t get_ptr_by_index(LogBody *log_body, uint16_t index, uint16_t *ptr)
{
    if(index < 1 || ptr == NULL)
    {
        return EFAULT;
    }

    // 若序号大于记录条数，则返回错误
    uint16_t rcd_num = record_num(log_body);
    if(index > rcd_num || rcd_num == 0)
    {
        return EFAULT;
    }

    // 定位指针，初始化为front需要回退的个数
    uint16_t extract_ptr = rcd_num - index;
    
    // 记录指针定位
    if(log_body->front > log_body->rear)
    {
        extract_ptr = log_body->front - extract_ptr;
    }
    else
    {
        if(log_body->front >= extract_ptr)
        {
            extract_ptr = log_body->front - extract_ptr;
        }
        else
        {
            extract_ptr = total_record_num(log_body) - (extract_ptr - log_body->front);
        }
    }
    
    if(extract_ptr == 0)
    {
        *ptr = total_record_num(log_body) - 1;
    }
    else
    {
        *ptr = extract_ptr - 1;
    }
    return DSUCCESS;
}
/********************************************************************************
 * Function       : store_log_attr
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 存储某一日志记录队列的属性信息
 * Calls          : do_attr_store
 * Input          : logfd：队列号
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static void store_log_attr(uint8_t logfd)
{
    uint32_t log_addr = LOG_ATTR_BASE_ADDRESS + sizeof(LogBody) * logfd;
    LogBody *log_body = get_log_body(logfd);
    do_attr_store(log_addr, (uint8_t *)log_body, sizeof(LogBody));
}
/********************************************************************************
 * Function       : load_log_attr
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 加载某一日志记录队列的属性信息
 * Calls          : do_attr_extract
 * Input          : logfd：队列号
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static void load_log_attr(uint8_t logfd)
{
    uint32_t log_addr = LOG_ATTR_BASE_ADDRESS + sizeof(LogBody) * logfd;
    LogBody *log_body = get_log_body(logfd);
    do_attr_extract(log_addr, (uint8_t *)log_body, sizeof(LogBody));
}
/********************************************************************************
 * Function       : init_single_log
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 初始化单个日志队列
 * Calls          : getCRC16
 * Input          : logfd：队列号
 *                  logattr：队列属性
 *                  base_addr：队列记录存储首地址，可以通过get_base_addr获得
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static void init_single_log(uint8_t logfd, UserLogAttr *logattr, uint32_t base_addr)
{
    if(logattr == NULL || \
        logattr->each_record_bytes == 0 || \
        logattr->max_record_num == 0)
    {
        return;
    }
    LogBody *log_body = get_log_body(logfd);
    
    log_body->host = logattr->host;
    log_body->fullFlag = 0;    // linf 20120620 增加存储满标志
    log_body->base_addr = base_addr;
    log_body->each_record_bytes = logattr->each_record_bytes;
    log_body->max_record_num = logattr->max_record_num;    
    log_body->day_record_num = 0;
    log_body->front = 0;
    log_body->rear  = 0;
    log_body->max_day_rnum = logattr->max_day_record_num;
    log_body->crc = getCRC16((uint8_t *)log_body + 2, sizeof(LogBody) - 2);
    do_record_erase(log_body->host, 0, 0);
}
/********************************************************************************
 * Function       : refresh_Queue_ptr
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 用于日志记录存入后更新队列头指针
 * Calls          : do_record_erase, getCRC16, store_log_attr
 * Input          : logfd：队列号，存储时使用
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static void refresh_Queue_ptr(uint8_t logfd)
{
    LogBody *log_body = get_log_body(logfd);
    
    // 若已存满，置满标志，不擦写
    if((log_body->front + 1) % total_record_num(log_body) == log_body->rear)
    {
        //do_record_erase(log_body->host, log_body->base_addr + log_body->rear*log_body->each_record_bytes, 0);
        log_body->rear = (log_body->rear + 1) % total_record_num(log_body);
    }
    log_body->front = (log_body->front + 1) % total_record_num(log_body);
    if(log_body->front < log_body->rear)
    {
        log_body->fullFlag = 1;  // linf 20120620 存满则置位满标志，该标志只有置位，不会被清除（除非重新初始化）
    }
    log_body->crc = getCRC16((uint8_t *)log_body + 2, sizeof(LogBody) - 2);

    //printf("add, front %d, rear %d\n", log_body->front, log_body->rear);
    store_log_attr(logfd);
}
/********************************************************************************
 * Function       : getfd_byhost
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 根据日志属主获得日志队列编号
 * Calls          : None
 * Input          : host：属主号
 * Output         : fd：队列编号
 * Return         : EFAULT：参数不合法或未找到编号；DSUCCESS：
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static int32_t getfd_byhost(uint8_t host, uint8_t *fd)
{
    uint8_t i;
    if(fd == NULL || host == 0)
    {
        return EFAULT;
    }
    
    for(i = 0; i < RECORD_QUEUE_NUM; i++)
    {
        if(hostmap[i] == host)
        {
            *fd = i;
            return DSUCCESS;
        }
    }
    return EFAULT;
}
/********************************************************************************
 * Function       : record_num
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 获取当前日志队列中的记录条数
 * Calls          : None
 * Input          : None
 * Return         : 记录条数
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static uint16_t record_num(LogBody *log_body)
{
    // linf 20120620 修改记录条数计算方式
    if(log_body->front >= log_body->rear)
    {
        if(log_body->fullFlag == 1)
        {
            return log_body->max_record_num;
        }
        else
        {
            if(log_body->front - log_body->rear >= log_body->max_record_num)
            {
                return log_body->max_record_num;
            }
            else
            {
                return log_body->front - log_body->rear;
            }
        }
    }
    else
    {
        return log_body->max_record_num;
    }
}
/********************************************************************************
 * Function       : total_record_num
 * Author         : Linfei
 * Date           : 2011.10.31
 * Description    : 获取当前日志队列中的日志记录条数
 * Calls          : None
 * Input          : None
 * Return         : 记录条数
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.31 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
static uint32_t total_record_num(LogBody *log_body)
{
    uint32_t rcd_num = (log_body->each_record_bytes * log_body->max_record_num);

    return (uint32_t)(rcd_num/log_body->each_record_bytes);
}
/***********************************************************************
 * Function       : getCRC16
 * Author         : Xu Shun'an
 * Date           : 2011.06.27
 * Description    : 生成16位的CRC校验码
 * Calls          : None
 * Input          : ptr: 待校验的数组指针，len: 待校验的数组长度
 * Output         : None
 * Return         : crc: 16位校验码
************************************************************************/
static uint16_t getCRC16(volatile uint8_t *ptr, uint16_t len)
{
    uint8_t i;
    uint16_t crc = 0xFFFF;
    if(len == 0)
    {
        len = 1;
    }
    while (len--)
    {
        crc ^= *ptr;
        for (i = 0; i < 8; i++)
        {
            if (crc & 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        ptr++;
    }
    return (crc);
}
/*************************************************************************
 * Function       : crccheck
 * Author         : linfei
 * Date           : 2011.10.31
 * Description    : CRC校验（CRC16/CRC32）
 * Calls          : getCRC16
 * Input          : pbuf: 待校验的数组指针
 *                  len: 待校验的数组长度
 *                  crctype: CRC校验类型，=0 为CRC16，=1 为CRC32
 * Output         : None
 * Return         : 0: 校验错误；1：校验成功
**************************************************************************/
static uint8_t crccheck(uint8_t *pbuf, uint16_t buflen, uint8_t crctype)
{
    if (buflen < 3)
    {
        return 0;
    }
    uint32_t crcresult = 0;
    uint8_t tmp[4] = {0};

    if (crctype == 0)
    {
        crcresult = getCRC16(pbuf, buflen - 2);
        tmp[0] = crcresult & 0xff;
        tmp[1] = (crcresult >> 8) & 0xff;
    
        if ((pbuf[buflen-2] == tmp[0]) && (pbuf[buflen-1] == tmp[1]))
        {
            return 1;
        }
    }
    else if(crctype == 1)
    {
        crcresult = getCRC32(pbuf, buflen - 4);
        tmp[0] = crcresult & 0xff;
        tmp[1] = (crcresult >> 8) & 0xff;
        tmp[2] = (crcresult >> 16) & 0xff;
        tmp[3] = (crcresult >> 24) & 0xff;
    
        if ((pbuf[buflen-4] == tmp[0]) && (pbuf[buflen-3] == tmp[1]) && 
            (pbuf[buflen-2] == tmp[2]) && (pbuf[buflen-1] == tmp[3]))
        {
            return 1;
        }
    }
    
    return 0;
}

