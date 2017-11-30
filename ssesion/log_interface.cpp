/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : log_interface.c
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : Log CBB底层数据访问接口，须由用户根据具体的存储设备实现，主要
 *                  包括日志属性信息读写接口、日志记录读写接口、时间获取接口
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#include "includes.h"

//log记录是采用昨天时间还是今天时间
uint8_t m_new_day_flag = 0;
LogTime m_last_time;
void log_init(void)//LOG模块初始化
{
	g_logattr[0].host = E_RUNRECORD_LOG; 		//运行记录
	g_logattr[0].each_record_bytes = EVERY_RUNRECORD_BUF_LEN;
	g_logattr[0].max_record_num = RUNRECORD_MAX_NUM;
	g_logattr[0].max_day_record_num = NO_RECORD_NUM_LIMIT;

	g_logattr[1].host = E_TIME_OF_USE_CURVE_LOG; 		//交流限发功率曲线
	g_logattr[1].each_record_bytes = EVERY_DAY_TIME_OF_USE_CURVE_BUF_LEN;
	g_logattr[1].max_record_num = DAY_TIME_OF_USE_CURVE_MAX_NUM;
	g_logattr[1].max_day_record_num = RECORD_NUM_LIMIT;

	g_logattr[2].host = E_PEAKVALLEY_CURVE_LOG; 		//消峰填谷功率曲线
	g_logattr[2].each_record_bytes = EVERY_DAY_PEAKVALLEY_CURVE_BUF_LEN;
	g_logattr[2].max_record_num = DAY_PEAKVALLEY_CURVE_MAX_NUM;
	g_logattr[2].max_day_record_num = RECORD_NUM_LIMIT;

	g_logattr[3].host = E_AC_LIMITED_PRODUCE_CURVE_LOG; 		//交流限发功率曲线
	g_logattr[3].each_record_bytes = EVERY_DAY_ACLIMIT_CURVE_BUF_LEN;
	g_logattr[3].max_record_num = DAY_ACLIMIT_CURVE_MAX_NUM;
	g_logattr[3].max_day_record_num = RECORD_NUM_LIMIT;

	g_logattr[4].host = E_OUTPUT_SMOOTH_CURVE_LOG; 		//交流限发功率曲线
	g_logattr[4].each_record_bytes = EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN;
	g_logattr[4].max_record_num = DAY_OUTPUT_SMOOTH_CURVE_MAX_NUM;
	g_logattr[4].max_day_record_num = RECORD_NUM_LIMIT;

	g_logattr[5].host = E_FM_CURVE_LOG; 		//交流限发功率曲线
	g_logattr[5].each_record_bytes = EVERY_FM_BUF_LEN;
	g_logattr[5].max_record_num = DAY_OUTPUT_SMOOTH_CURVE_MAX_NUM;
	g_logattr[5].max_day_record_num = DAY_FM_CURVE_MAX_NUM;

	g_logattr[6].host = E_MGRID_CURVE_LOG; 		//微网功率曲线
	g_logattr[6].each_record_bytes = EVERY_DAY_MGRID_CURVE_BUF_LEN;
	g_logattr[6].max_record_num = DAY_MGRID_CURVE_MAX_NUM;
	g_logattr[6].max_day_record_num = RECORD_NUM_LIMIT;

    init_log(g_logattr);//日志模块初始化
}

/********************************************************************************
 * Function       : do_attr_store
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : 日志属性信息存储接口，将属性存储到EEPROM或其他非易失存储体中
 * Calls          : None
 * Input          : addr：数据存储地址
 *                  data：待存储的数据
 *                  len： 待存储的数据字节数
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void do_attr_store(uint32_t addr, uint8_t *data, uint16_t len)
{
    if(data != NULL)
    {
		write_eeprom(addr, data, len);
    }
}
/********************************************************************************
 * Function       : do_attr_extract
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : 日志属性信息提取接口
 * Calls          : None
 * Input          : addr：待读取的数据首地址
 *                  data：保存待读取数据的缓冲区首地址
 *                  len： 待读取的数据字节数
 * Output         : data：保存读取的数据
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void do_attr_extract(uint32_t addr, uint8_t *data, uint16_t len)
{
    if(data != NULL)
    {
		read_eeprom(addr, data, len);
    }
}
/********************************************************************************
 * Function       : do_record_store
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : 记录存储的执行接口，将记录存储到FLASH或其他非易失存储体中
 * Calls          : None
 * Input          : addr：数据存储地址
 *                  data：待存储的数据
 *                  len： 待存储的数据字节数
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void do_record_store(uint8_t host, uint32_t addr, uint8_t *data, uint16_t len)
{
    if(data != NULL)
    {
    	if(host == E_RUNRECORD_LOG){
    		write_running_info(addr, data, len);
    	}else if(host == E_TIME_OF_USE_CURVE_LOG){
    		write_time_of_use(addr, data, len);
    	}else if(host == E_PEAKVALLEY_CURVE_LOG){
    		write_peak_valley(addr, data, len);
    	}else if(host == E_AC_LIMITED_PRODUCE_CURVE_LOG){
    		write_ac_limiting(addr, data, len);
    	}else if(host == E_OUTPUT_SMOOTH_CURVE_LOG){
    		write_output_smooth(addr, data, len);
    	}else if(host == E_FM_CURVE_LOG){
    		write_fm(addr, data, len);
    	}else if(host == E_MGRID_CURVE_LOG){
    		write_mgrid(addr, data, len);
    	}
    }
}

/********************************************************************************
 * Function       : do_record_extract
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : 记录提取的执行接口，将记录从FLASH或其他非易失存储体中提取出来
 * Calls          : None
 * Input          : addr：待读取的数据首地址
 *                  data：保存待读取数据的缓冲区首地址
 *                  len： 待读取的数据字节数
 * Output         : data：保存读取的数据
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void do_record_extract(uint8_t host, uint32_t addr, uint8_t *data, uint16_t len)
{
    if(data != NULL)
    {
    	if(host == E_RUNRECORD_LOG){
    		read_running_info(addr, data, len);
    	}else if(host == E_TIME_OF_USE_CURVE_LOG){
    		read_time_of_use(addr, data, len);
    	}else if(host == E_PEAKVALLEY_CURVE_LOG){
    		read_peak_valley(addr, data, len);
    	}else if(host == E_AC_LIMITED_PRODUCE_CURVE_LOG){
    		read_ac_limiting(addr, data, len);
    	}else if(host == E_OUTPUT_SMOOTH_CURVE_LOG){
    		read_output_smooth(addr, data, len);
    	}else if(host == E_FM_CURVE_LOG){
    		read_fm(addr, data, len);
    	}else if(host == E_MGRID_CURVE_LOG){
    		read_mgrid(addr, data, len);
    	}
    }
}


/********************************************************************************
 * Function       : do_record_erase
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : 擦除旧数据接口，返回擦除的字节数。FLASH存储新数据时须擦除旧有
 *                  数据，以块为单位擦除，每块4KB，因此返回4096。
 *                  其他存储体依照存储特性实现。若无需擦除则函数体固定为return 0;
 * Calls          : None
 * Input          : addr：待擦除数据的首地址
 * Output         : None
 * Return         : 擦除的字节数
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint16_t do_record_erase(uint8_t host, uint32_t addr, uint32_t len)
{
	if(host == E_RUNRECORD_LOG){
		clear_running_info();
	}else if(host == E_TIME_OF_USE_CURVE_LOG){
		clear_time_of_use();
	}else if(host == E_PEAKVALLEY_CURVE_LOG){
		clear_peak_valley();
	}else if(host == E_AC_LIMITED_PRODUCE_CURVE_LOG){
		clear_ac_limiting();
	}else if(host == E_OUTPUT_SMOOTH_CURVE_LOG){
		clear_output_smooth();
	}else if(host == E_FM_CURVE_LOG){
		clear_fm();
	}else if(host == E_MGRID_CURVE_LOG){
		clear_mgrid();
	}
    return len;
}


/********************************************************************************
 * Function       : getTime
 * Author         : Linfei
 * Date           : 2011.10.29
 * Description    : 时间戳获取接口
 * Calls          : None
 * Input          : time：存储读取的时间戳的结构体指针
 * Output         : time：保存读取的时间戳
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2011.10.29 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void getTime(LogTime *time)
{
    if(time == NULL)
    {
        return;
    }

    if(m_new_day_flag == 0)
    {
        time->Year   = (uint8_t)showTime.tm_year;
        time->Month  = (uint8_t)showTime.tm_mon;
        time->Date   = (uint8_t)showTime.tm_day;
        time->Hour   = (uint8_t)showTime.tm_hour;
        time->Minute = (uint8_t)showTime.tm_min;
        time->Second = (uint8_t)showTime.tm_sec;
        //printf("get new day: %d-%d-%d\n", time->Year, time->Month, time->Date);
    }
    else
    {
    	m_new_day_flag = 0;
        time->Year   = m_last_time.Year;
        time->Month  = m_last_time.Month;
        time->Date   = m_last_time.Date;
        time->Hour   = m_last_time.Hour;
        time->Minute = m_last_time.Minute;
        time->Second = m_last_time.Second;
        //printf("get last day: %d-%d-%d\n", time->Year, time->Month, time->Date);
    }
}
//设置日志时间为前一天
void setLogLastDaytime(LogTime *time)
{
    if(time == NULL)
    {
        return;
    }
    m_last_time.Year = time->Year;
    m_last_time.Month = time->Month;
    m_last_time.Date = time->Date;
    m_last_time.Hour = time->Hour;
    m_last_time.Minute = time->Minute;
    m_last_time.Second = time->Second;
    m_new_day_flag = 1;    
    //printf("set last day: %d-%d-%d\n", m_last_time.Year, m_last_time.Month, m_last_time.Date);
}
/***********************************************************************
 * Function       : getCRC32
 * Author         : linfei
 * Date           : 2011.10.31
 * Description    : 生成32位的CRC校验码，目前为ARM平台的硬件校验（非主流，
 *                  左移实现），若更换平台须修改该函数
 * Calls          : None
 * Input          : ptr: 待生成校验的数组指针
 *                  len: 待生成校验的数组长度
 * Output         : None
 * Return         : crc: 32位校验码，出错返回0
************************************************************************/
uint32_t getCRC32(volatile uint8_t *ptr, uint16_t len)
{
	return embf_get_crc32(ptr, len);
}

