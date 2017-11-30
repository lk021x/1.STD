/****************************************************************************** 
* Copyright (C), 1997-2012, SUNGROW POWER SUPPLY CO., LTD. 
* File name      :utils.h
* Author         :llemmx
* Date           :2012-11-02
* Description    :操作系统函数的封装、常用函数的封装
* Interface      :无
* Others         :无
*******************************************************************************
* History:        初稿
*------------------------------------------------------------------------------
* 2012-11-2 : 1.0.0 : llemmx
* Modification   :
*------------------------------------------------------------------------------
******************************************************************************/
#ifndef UTILS_H_
#define UTILS_H_

#include "global.h"

typedef struct
{
    uint32_t StdId;
    uint32_t ExtId;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t DLC;
    uint8_t Data[8];
    uint8_t FMI;
} t_embf_canmsgrx;

typedef struct
{
    uint32_t StdId;
    uint32_t ExtId;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t DLC;
    uint8_t Data[8];
} t_embf_canmsgtx;

extern uint8_t g_ip_buf1[4];    //DNS所用全局变量
extern uint8_t g_ip_buf2[4]; 

#ifdef __cplusplus
extern "C" {
#endif
int32_t embf_get_stksize_used(uint8_t prio);
uint32_t embf_get_tick(void);
void embf_delay(uint32_t p_time);
uint8_t embf_get_usage(void);
void embf_get_current_ver(char *buf,uint8_t size);
void init_run(void);
void enter_low_power(void);
int32_t task_prio_prompt(void);
int32_t task_prio_recover(void);

void init_eeprom();
int save_eeprom();
void bak_eeprom();

int32_t write_eeprom(uint32_t addr, const void* buf, uint32_t count);
int32_t read_eeprom(uint32_t addr, void* buf, uint32_t count);

int32_t read_running_info(uint32_t addr, void* buf, uint32_t count);
int32_t write_running_info(uint32_t addr, const void* buf, uint32_t count);
int32_t clear_running_info();

int32_t read_time_of_use(uint32_t addr, void* buf, uint32_t count);
int32_t write_time_of_use(uint32_t addr, const void* buf, uint32_t count);
int32_t clear_time_of_use();

int32_t read_peak_valley(uint32_t addr, void* buf, uint32_t count);
int32_t write_peak_valley(uint32_t addr, const void* buf, uint32_t count);
int32_t clear_peak_valley();

int32_t read_ac_limiting(uint32_t addr, void* buf, uint32_t count);
int32_t write_ac_limiting(uint32_t addr, const void* buf, uint32_t count);
int32_t clear_ac_limiting();

int32_t read_output_smooth(uint32_t addr, void* buf, uint32_t count);
int32_t write_output_smooth(uint32_t addr, const void* buf, uint32_t count);
int32_t clear_output_smooth();

int32_t read_fm(uint32_t addr, void* buf, uint32_t count);
int32_t write_fm(uint32_t addr, const void* buf, uint32_t count);
int32_t clear_fm();

int32_t read_mgrid(uint32_t addr, void* buf, uint32_t count);
int32_t write_mgrid(uint32_t addr, const void* buf, uint32_t count);
int32_t clear_mgrid();

void open_update_file(char *filename, char *mode);
int32_t read_update_file(uint32_t addr, void* buf, uint32_t count);
int32_t write_update_file(uint32_t addr, const void* buf, uint32_t count);
void close_update_file();
int32_t clear_update_file();

void watch_dog_inter_open(void);//打开内部看门狗

bool netline_connected(void);
void ads1247_read_period_proc(void);

void GetTcpAddr(uint8_t dns1[],uint8_t dns2[],const char *hostname,uint8_t index);
uint8_t embf_cryp_aes_ecb(uint8_t mode, uint8_t* key, uint16_t keysize, uint8_t* input, uint32_t ilength, uint8_t* output);//AES加密封装
uint8_t embf_bcd_to_dec(uint8_t bcd);//BCD码转换为十进制数
uint8_t embf_dec_to_bcd(uint8_t dec);//十进制数转换为BCD码
uint8_t embf_get_crc8(uint8_t *id, uint8_t len);//生成8位CRC校验码
uint32_t embf_get_crc32(volatile uint8_t *ptr, uint16_t len);//生成32位CRC检验码
uint16_t embf_get_crc16(volatile uint8_t *ptr, uint16_t len);//生成16位CRC检验码
uint16_t CRC_CCITT(uint8_t *pcData, uint8_t dwSize);//
uint8_t embf_crccheck(uint8_t* pbuf, uint16_t buflen, uint8_t crctype);//判断CRC校验是否正确
int32_t net_ioctl(uint32_t op, void *arg);
uint32_t ETH_CheckFrameReceived(void);
uint32_t CRC32(uint32_t* InputData, int len);

int CRYP_AES_ECB(uint8_t Mode,uint8_t *Key, uint16_t Keysize,uint8_t *Input, uint32_t Ilength,uint8_t *Output);

#ifdef __cplusplus
}
#endif

#define UTILS_IO_LED_RUN   0x00000001U

#define UTILS_IO_HIGH 0x00u
#define UTILS_IO_LOW  0x01u

void io_ctrl(uint32_t io,uint8_t flag);

#define MODE_ENCRYPT             ((uint8_t)0x01) //AES宏添加 
#define MODE_DECRYPT             ((uint8_t)0x00)

#define DSUCCESS     0
#define OPFAULT      (-1)
#define DEVBUSY      (-2)

extern pthread_mutex_t mutex_for_eeporm;
extern pthread_mutex_t mutex_for_eeporm_file;
extern pthread_mutex_t mutex_for_flash;

#endif
