/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : iapv2.c
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : IAP-V2升级模块，用于实现设备在线升级，支持多级升级和广播
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *------------------------------------------------------------------------------
 ******************************************************************************/
#include "includes.h"

/* 单纯工作在从模式，没有两级，只有单级 */
#if (IAP_BOTH_HOST_SLAVE == 0)
    #define IAP_TWO_LEVEL_MODE 0
#endif

#define  MODBUS_CMD_03       0x03 /* 读保持寄存器 */
#define  MODBUS_CMD_10       0x10 /* 写多个寄存器 */
#define  MODBUS4X_ADDR       (20002 - 1)

#define  IAP_COM_TRYTIMES    20    /* 通信不上重试次数 *///20131025suzd由于DSP擦除时间测试出比较长，因而需要将时间设长,10改为20
#define  IAP_CNTBUF_LEN      100   /* 多级升级时数据包缓冲区 */
#define  IAP_REPEAT_CNT      50   /* 重复下发相同包最大次数，超过则出错 */

/*  升级命令 */
#define IAP_UPDATE         0x0003
#define IAP_NOCMD          0x0000

/* 升级状态 */
#define IAP_NO_UPDATE      0x0001 /* 未升级 */
#define IAP_UPDATING       0x0002 /* 正在升级 */
#define IAP_FAULT          0x0003 /* 故障 */
#define IAP_SUCCESSED      0x0004 /* 升级成功，适用于本级自身升级 */
#define IAP_SUBUPDATING    0x0005 /* 正在向下级升级 */
#define IAP_SUBUPEND       0x0006 /* 向下升级结束 */
#define IAP_UPEND          0x0007 /* 本级升级结束，适用于本级非自身升级 */

/* 升级异常 */
#define IAP_NO_LOGIN       0x00000001 /* 未登陆 */
#define IAP_PASSWORD_WRONG 0x00000002 /* 密码错误 */
#define IAP_TIMEOUT        0x00000004 /* 超时退出 */
#define IAP_EXIT           0x00000008 /* 退出 */
#define IAP_MODEL_WRONG    0x00000010 /* 型号不匹配 */
#define IAP_VER_WRONG      0x00000020 /* 固件版本不匹配 */
#define IAP_AUTO_FAILED    0x00000040 /* 自动升级失败 */
#define IAP_NOSPACE        0x00000080 /* 程序空间不够 */
#define IAP_PKG_LOST       0x00000100 /* 缺包 */
#define IAP_CRCCHK_FAILED  0x00000200 /* CRC校验失败 */
#define IAP_COM_ERROR      0x00000400 /* 从机通信不上 */

#if IAP_BOTH_HOST_SLAVE
#define IAP_MAX_DEVNUM        30  /* 最多的下级设备数量 */
#define IAP_CMD_READ_IAPVER       0   /* 读取协议版本号 */
#define IAP_CMD_READ_STATUS       1   /* 读取属性、升级状态、升级异常 */
#define IAP_CMD_READ_SOFTVER      2   /* 读取软件版本、自动升级标志、校验值、总长度、每包长度、总包数、包计数 */
#define IAP_CMD_READ_LOST         3   /* 读取缺包信息 */
#define IAP_CMD_SET_LOGIN         4   /* 登录、设置超时时间、传输命令 */
#define IAP_CMD_SET_LOGOUT        5   /* 退出登录，中止升级 */
#define IAP_CMD_SET_DEST          6   /* 设置目标计数、目标地址 */
#define IAP_CMD_SET_SOFTVER       7   /* 设置软件版本、自动升级标志、校验值、总长度、每包长度、总包数、包计数 */
#define IAP_CMD_SET_DATASEND      8   /* 发送升级数据 */
#define IAP_CMD_EXIT              9   /* 升级结束 */
#endif

uint16_t iap_status;                          /* 升级状态 */
uint32_t iap_faults;                          /* 升级故障 */
uint32_t iap_rltbit;                          /* 本级非自身设备升级结果，按位表示：0失败，1成功 */
uint16_t iap_cmd;                             /* 命令 */
uint16_t iap_ovtime = 0;                      /* 超时时间 */
uint16_t iap_ovtime_cnt;                      /* 超时计数 */
uint16_t iap_trancmd;                         /* 传输命令：广播、查询 */
uint16_t iap_reset_cnt;                       /* 软件复位 */
uint32_t iap_crc32;                           /* 效验值 */
uint32_t iap_len_sum;                         /* 总长度 */
uint16_t iap_pkglen;                          /* 每包长度 */
uint16_t iap_pkg_sum;                         /* 总包数 */
uint16_t iap_pkg_lost;                        /* 缺包数 */
uint16_t iap_auto_flg = 0x55;                 /* 自动升级标志:0x55自动升级，0xAA强制升级 */
uint32_t iap_pkgbit[PKG_CNT_BUFLEN] = { 0 };  /* 包接收位表 */
uint16_t iap_cntbuf[IAP_CNTBUF_LEN];          /* 升级数据包计数缓冲区 */
uint8_t  iap_cntbuflen;                       /* 缓冲区长度 */
char     iap_softver[30];                     /* 软件版本号 */
uint16_t iap_level;                           /* 目标级数：0表示直接通讯 */
bool     iap_isself_flg = true;               /* 待升级对象是否是自身标志，默认是对自身升级 */
bool     iap_chkcrc_flg = false;              /* crc校验标志 */
bool     iap_chkver_flg = false;              /* 是否检验过固件版本标志 */
bool     iap_verok_flg = false;               /* 固件版本匹配标志 */
bool     iap_filesaved_flg = false;           /* 文件接收完成标志 */
uint8_t  iap_sec_time_cnt = 0;                /* 秒计时用 */

#if IAP_BOTH_HOST_SLAVE
uint16_t iap_addr1[IAP_MAX_DEVNUM];           /* 目标地址 */
uint8_t  iap_addr1num;                        /* 目标地址个数 */
uint16_t iapsl_resend_cnt = 0;                /* 向下升级重发计时 */
uint8_t  iapsl_resend_try = 0;                /* 向下重试次数 */
uint8_t  iapsl_precmdtype = IAP_CMD_EXIT;     /* 主机上次下发的命令 */
uint8_t  iapsl_cmdtype = IAP_CMD_EXIT;        /* 主机本次下发的命令 */
uint16_t iapsl_curdev = 0;                    /* 设备索引 */
uint16_t iapsl_status[IAP_MAX_DEVNUM];        /* 升级状态 */
uint32_t iapsl_faults[IAP_MAX_DEVNUM];        /* 升级故障 */
uint16_t iapsl_pkg_cnt;                       /* 包计数，从1开始计数 */
uint16_t iapsl_sent_cnt;                      /* 已发送包个数 */
uint16_t iapsl_max_pkgcnt;                    /* 向下最大发包计数 */
uint8_t  iapsl_stage;                         /* 主机对从机升级所处的阶段：0:传输前握手阶段，1:传输中，2:传输后补包阶段 */
bool     iapsl_broadcasting = false;          /* 正在广播标志 */
bool     iapsl_retflg = true;                 /* 从机返回数据标志 */
uint8_t  iapsl_start_cmd = 0;             /* 启动从机升级命令 */
uint16_t iapsl_brperiod;                      /* 向下广播周期 */
uint8_t  iapsl_first_send;                    /* 第一包标志 */
uint8_t  iapsl_multisend_lost;                /* 广播补包标志 */
uint8_t  iapsl_newupdate_flg;                 /* 非断点续传标志 */
#endif

/* 模块内函数声明 */
uint8_t  iapv2_level0_checkver(char *checkver);
//uint8_t  iapv2_check_ver(char *checkver);
uint8_t  iapv2_get_pkgbit(uint16_t pkg_cnt);
uint16_t iapv2_get_emptyidx(void);
//uint16_t iapv2_get_maxidx(void);
uint16_t iapv2_get_total_lostcnt(void);
void     iapv2_set_pkgbit(uint16_t pkg_cnt);
void     iapv2_get_pkgattr_bycnt(uint16_t pkgcnt, uint32_t *pkgaddr, uint16_t *pkglen);
bool     iapv2_checkver(char *version);
bool     iapv2_check_pkg(void);
bool     iapv2_dest_isself(char *checkver);
void     iapv2_over_proc(void);
void     iapv2_sec_proc(void);

#if IAP_BOTH_HOST_SLAVE
uint8_t  iapv2_package_cmd(uint8_t devaddr, uint8_t cmd_type, uint8_t buf[]);
#if IAP_TWO_LEVEL_MODE
uint8_t  iapv2_level1_checkver(char *checkver);
#endif
void     iapv2_find_next_updatingdev(void);
void     iapv2_send_cmd(uint8_t devaddr, uint8_t cmd_type, uint8_t buf[]);
void     iapv2_err_proc(void);
void     iapv2_enterstage1_preproc(void);
void     iapv2_exitstage1_proc(void);
void     iapv2_slave_start(void);
void     iapv2_refresh_pkgcnt(void);
void     iapv2_slave_exit(void);
#endif

#if IAP_BOTH_HOST_SLAVE

/********************************************************************************
 * Function       : iapv2_dest_isself
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 判断升级目标是否是自身 
 * Calls          : None
 * Input          : checkver：升级文件的固件版本
 *                  self: 自身设备类型
 * Output         : None
 * Return         : true：是对自身升级，false：对其他设备升级
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
bool iapv2_dest_isself(char *checkver)
{
    bool isself = false;
    uint8_t len = 30;
    uint8_t vidx;
    char selfver[30] = { 0 };
    
    /* 获取自身固件版本 */
    get_firmversion(0, 0, 0, selfver);

    /* 从后往前找到第一个特征字符串"_V" */
    for(vidx = 0; vidx < len; vidx++)
    {
        if((checkver[len - vidx - 1] == 'V') && 
           (checkver[len - vidx - 2] == '_') && (vidx != 0))
        {
            if(checkver[len - vidx] != '_')
            {
                break;
            }
        }
    }

    if(vidx != len)
    {
        vidx = len - vidx;
        /* 固件版本的"_V"前半部分匹配，则认为是对自身升级 */
        if(strncmp(checkver, selfver, vidx) == 0)
        {
            isself = true;
        }
    }
    
    return isself;
}

/********************************************************************************
 * Function       : iapv2_level0_checkver
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 与设备本身的版本号比较，检测软件版本号是否合法
 * Calls          : None
 * Input          : checkver：升级文件的固件版本
 * Output         : None
 * Return         : 0：软件版本号合法，新版本替换老版本;
 *                  1：软件版本号合法，老版本替换新版本；
 *                  2：软件版本号不合法
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint8_t iapv2_level0_checkver(char *checkver)
{
    uint8_t chkrlt = 2;
    char firmver[30] = { 0 };
    uint8_t len = 30;
    uint8_t vidx;
    uint8_t firmnum = 0;

    /* 从后往前找到第一个特征字符串"_V" */
    for(vidx = 0; vidx < len; vidx++)
    {
        if((checkver[len - vidx - 1] == 'V') && 
           (checkver[len - vidx - 2] == '_') && (vidx != 0))
        {
            if(checkver[len - vidx] != '_')
            {
                break;
            }
        }
    }

    if(vidx != len)
    {
        vidx = len - vidx;
        firmnum = get_firmnum(0, 0);
        for(uint8_t i = 0; i < firmnum; i++)
        {
            get_firmversion(0, 0, i, firmver);
            if(strncmp(checkver, firmver, vidx) == 0)
            {
                /* 升级文件的固件版本和设备当前的前半部分匹配 */
                if((iap_auto_flg == 0x55) && \
                        (strncmp(&checkver[vidx], &firmver[vidx], 30 - vidx) <= 0))
                {
                    /* 老版本替换新版本，或相同版本替换 */
                    chkrlt = 1;
                }
                else
                {
                    /* 新版本替换老版本 */
                    chkrlt = 0;
                }
                break;
            }
        }
    }
    return chkrlt;
}

#if IAP_TWO_LEVEL_MODE

/********************************************************************************
 * Function       : iapv2_level1_checkver
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 检测软件版本号是否合法，不合法则置相应的iapsl_status
 * Calls          : None
 * Input          : checkver：升级文件的固件版本
 * Output         : None
 * Return         : 匹配成功的个数
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint8_t iapv2_level1_checkver(char *checkver)
{
    char firmver[30] = { 0 };
    uint8_t len = 30;
    uint8_t vidx;
    uint8_t firmnum = 0;
    uint8_t i;
    uint8_t veroknum = 0;

    /* 从后往前找到第一个特征字符串"_V" */
    for(vidx = 0; vidx < len; vidx++)
    {
        if((checkver[len - vidx - 1] == 'V') && 
           (checkver[len - vidx - 2] == '_') && (vidx != 0))
        {
            if(checkver[len - vidx] != '_')
            {
                break;
            }
        }
    }

    if(vidx != len)
    {
        vidx = len - vidx;
        for(uint8_t didx = 0; didx < iap_addr1num; didx++)
        {
            firmnum = get_firmnum(1, iap_addr1[didx]);
            for(i = 0; i < firmnum; i++)
            {
                get_firmversion(1, iap_addr1[didx], i, firmver);

                if(strncmp(checkver, firmver, vidx) == 0)
                {
                    /* 升级文件的固件版本和设备当前的前半部分匹配 */
                    if((iap_auto_flg == 0x55) && \
                            (strncmp(&checkver[vidx], &firmver[vidx], 30 - vidx) <= 0))
                    {
                        /* 老版本替换新版本，或相同版本替换 */
                        iapsl_status[didx] = IAP_FAULT;
                        iapsl_faults[didx] = IAP_AUTO_FAILED;
                    }
                    else
                    {
                        /* 新版本替换老版本 */
                        iapsl_status[didx] = 0;
                        iapsl_faults[didx] = 0;
                        veroknum++;
                    }
                    break;
                }
            }
            if(i == firmnum)
            {
                /* 版本错误 */
                iapsl_status[didx] = IAP_FAULT;
                iapsl_faults[didx] = IAP_VER_WRONG;
            }
        }
    }
    return veroknum;
}
#endif

/********************************************************************************
 * Function       : iapv2_checkver
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 检测软件版本号是否合法，不合法则置相应的iapsl_status
 * Calls          : None
 * Input          : version：升级文件的固件版本
 * Output         : None
 * Return         : true：匹配
 *                  false：不匹配
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
bool iapv2_checkver(char *version)
{
    bool verok = false;
    if(iap_level == 0)
    {
        uint8_t checkrlt = iapv2_level0_checkver(version);
        if(checkrlt == 0)
        {
            verok = true;
        }
        else if(checkrlt == 2)
        {
            iap_status = IAP_FAULT;
            iap_faults |= IAP_VER_WRONG;
        }
        else if((checkrlt == 1) && (iap_auto_flg == 0x55))
        {
            iap_status = IAP_FAULT;
            iap_faults |= IAP_AUTO_FAILED;
        }
    }
#if IAP_TWO_LEVEL_MODE
    else
    {
        uint8_t i;
        if(iapv2_level1_checkver(version) == 0)
        {
            /* 所有设备匹配均不成功，检测错误类型 */
            for(i = 0; i < iap_addr1num; i++)
            {
                if(iapsl_faults[i] == IAP_AUTO_FAILED)
                {
                    break;
                }
            }
            if(i == iap_addr1num)
            {
                iap_status = IAP_FAULT;
                iap_faults |= IAP_VER_WRONG;
            }
            else
            {
                iap_status = IAP_FAULT;
                iap_faults |= IAP_AUTO_FAILED;
            }
        }
        else
        {
            verok = true;
        }
    }
#endif
    return verok;
}
#endif

/********************************************************************************
 * Function       : iapv2_set_pkgbit
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 设置包计数对应的已接收标志 
 * Calls          : None
 * Input          : pkg_cnt：包计数
 * Output         : None
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iapv2_set_pkgbit(uint16_t pkg_cnt)
{
    uint16_t idx = (pkg_cnt - 1) >> 5;
    uint32_t offset = 1 << ((pkg_cnt - 1) % 32);
    iap_pkgbit[idx] |= offset;
    uint8_t subidx = ((pkg_cnt - 1) % 32) >> 3;
    uint8_t sub_pkgbit = (iap_pkgbit[idx] >> (8 * subidx)) & UINT8_MAX;

    /* 将修改的位表元素存入EEPROM */
    iapv2_do_attr_store(IAP_ATTR_BASE + 62 + (idx << 2) + subidx, &sub_pkgbit, 1);
}

/********************************************************************************
 * Function       : iapv2_get_pkgbit
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 获取包计数对应的包已接收标志 
 * Calls          : None
 * Input          : pkg_cnt：包计数
 * Output         : None
 * Return         : 0表示未接收，1表示已接收
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint8_t iapv2_get_pkgbit(uint16_t pkg_cnt)
{
    uint8_t bitstatus = 0;
    if(pkg_cnt > 0)
    {
        uint16_t idx = (pkg_cnt - 1) >> 5;
        uint32_t offset = 1 << ((pkg_cnt - 1) % 32);
        if((iap_pkgbit[idx] & offset) != 0)
        {
            bitstatus = 1;
        }
    }
    return bitstatus;
}

/********************************************************************************
 * Function       : iapv2_get_total_lostcnt
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 获取缺包总数 
 * Calls          : None
 * Input          : None
 * Output         : None
 * Return         : 缺包总数
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint16_t iapv2_get_total_lostcnt(void)
{
    uint16_t lostcnt = 0;
    for(uint16_t i = 0; i < iap_pkg_sum; i++)
    {
        if(iapv2_get_pkgbit(i + 1) == 0)
        {
            lostcnt++;
        }
    }
    return lostcnt;
}

/********************************************************************************
 * Function       : iapv2_get_max_setidx
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 获取位表中置位的最大包计数 
 * Calls          : None
 * Input          : None
 * Output         : None
 * Return         : 置位的最大包计数
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint16_t iapv2_get_max_setidx(void)
{
    uint16_t max_idx = 0;
    int16_t i;
    uint8_t cmpnum = 32;
    uint16_t max_pkg_cnt = iap_pkg_sum >> 5;
    bool end_notfull = false;

    if((iap_pkg_sum % 32) != 0)
    {
        max_pkg_cnt++;
        end_notfull = true;
    }

    for(i = max_pkg_cnt - 1; i >= 0; i--)
    {
        if(iap_pkgbit[i] != 0x00000000)
        {
            break;
        }
    }

    /* 末尾包，只检测末尾不满32个的包位 */
    if((i == max_pkg_cnt - 1) && (end_notfull == true))
    {
        cmpnum = iap_pkg_sum % 32;
    }
    if(i >= 0)
    {
        for(int8_t j = cmpnum - 1; j >= 0; j--)
        {
            if((iap_pkgbit[i] & (1 << j)) != 0)
            {
                max_idx = (i << 5) + j + 1;
                break;
            }
        }
    }

    return max_idx;
}

/********************************************************************************
 * Function       : iapv2_get_emptyidx
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 获取第一个缺的包的索引
 * Calls          : None
 * Input          : None
 * Output         : None
 * Return         : 缺包索引，0表示没有缺包
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint16_t iapv2_get_emptyidx(void)
{
    uint16_t empty_idx = 0;
    uint16_t max_pkg_cnt = iap_pkg_sum >> 5;
    uint16_t i;
    uint8_t cmpnum = 32;
    bool end_notfull = false;

    if(iap_pkg_sum != 0)
    {
        if((iap_pkg_sum % 32) != 0)
        {
            max_pkg_cnt++;
            end_notfull = true;
        }
        for(i = 0; i < max_pkg_cnt; i++)
        {
            if(iap_pkgbit[i] != UINT32_MAX)
            {
                break;
            }
        }
    
        /* 末尾包，只检测末尾不满32个的包位 */
        if((i == max_pkg_cnt - 1) && (end_notfull == true))
        {
            cmpnum = iap_pkg_sum % 32;
        }
        if(i != max_pkg_cnt)
        {
            for(uint8_t j = 0; j < cmpnum; j++)
            {
                if((iap_pkgbit[i] & (1 << j)) == 0)
                {
                    empty_idx = (i << 5) + j + 1;
                    break;
                }
            }
        }
    }

    return empty_idx;
}

/********************************************************************************
 * Function       : iapv2_get_pkgattr_bycnt
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 根据包计数获取该包起始地址和包长
 * Calls          : None
 * Input          : pkgcnt：包计数
 * Output         : pkgaddr：包起始地址
 *                  pkglen：包长
 * Return         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
void iapv2_get_pkgattr_bycnt(uint16_t pkgcnt, uint32_t *pkgaddr, uint16_t *pkglen)
{
    *pkgaddr = (pkgcnt - 1) * iap_pkglen;
    *pkglen = iap_pkglen;
    if(pkgcnt == iap_pkg_sum)
    {
        if((iap_len_sum % iap_pkglen) != 0)
        {
            *pkglen = iap_len_sum % iap_pkglen;
        }
    }
}

/********************************************************************************
 * Function       : iapv2_range_chk
 * Author         : Linfei
 * Date           : 2012.11.05
 * Description    : 判断协议地址是否为IAP-V2协议地址范围
 * Calls          : None
 * Input          : beginaddr：起始寄存器地址
 *                  len：寄存器个数
 *                  cmd：modbus命令
 * Output         : None
 * Return         : 0：非IAP-V2地址，1：IAP-V2地址
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2012.11.05 : 1.0.0 : Linfei
 * Modification   : 初始代码编写。
 *********************************************************************************/
uint8_t iapv2_range_chk(uint16_t beginaddr, uint8_t num, uint8_t cmd)
{
    /* 地址有效返回1，无效返回0 */
    if(cmd == MODBUS_CMD_03)
    {
        if(beginaddr >= 19999 && beginaddr + num - 1 <= MODBUS4X_ADDR + 174)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if(cmd == MODBUS_CMD_10)
    {
        if(beginaddr >= MODBUS4X_ADDR + 8 && beginaddr + num - 1 <= MODBUS4X_ADDR + 165)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

uint8_t iapv2_read_data(uint16_t addr, uint8_t sbuf[], uint8_t num)
{
    uint16_t curaddr = addr;
    uint8_t len = num << 1;
    uint16_t index = 0;
    uint16_t regaddr = MODBUS4X_ADDR;
    /* 协议版本号 */
    if((curaddr >= 19999) && (curaddr <= 20000))
    {
        sbuf[index]     = 0x01;
        sbuf[index + 1] = 0x00;
        sbuf[index + 2] = 0x02;
        sbuf[index + 3] = 0x00;
        index += 4;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 保留 */
    if(curaddr == regaddr)
    {
        sbuf[index]     = 0x00;
        sbuf[index + 1] = 0x00;
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 0级MCU个数 */
    if(curaddr == (regaddr + 1)) //0x4E22
    {
        uint8_t num = iapv2_getlevel0_devaddr(iap_softver, NULL);
        sbuf[index]     = HIBYTE(num);
        sbuf[index + 1] = LOBYTE(num);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 属性:支持解密，IAP，BOOT */
    if(curaddr == regaddr + 2)  //0x4E23
    {
        sbuf[index]     = 0x00;
#if IAP_BOTH_HOST_SLAVE
        sbuf[index + 1] = 0x0B; /* 支持解密，支持IAP，不支持Boot，支持动态调节波特率 */
#else
        sbuf[index + 1] = 0x04; /* 不支持解密，不支持IAP，支持Boot */
#endif
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 升级状态 */
    if(curaddr == regaddr + 3) //0x4E24
    {
        sbuf[index]     = HIBYTE(iap_status);
        sbuf[index + 1] = LOBYTE(iap_status);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 升级异常 */
    if((curaddr >= regaddr + 4) && (curaddr <= regaddr + 5))  //0x4E25, 0x4E26
    {
        sbuf[index]     = HIBYTE(LOWORD(iap_faults));
        sbuf[index + 1] = LOBYTE(LOWORD(iap_faults));
        sbuf[index + 2] = HIBYTE(HIWORD(iap_faults));
        sbuf[index + 3] = LOBYTE(HIWORD(iap_faults));
        index += 4;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 升级成功标志 */
    if((curaddr >= regaddr + 6) && (curaddr <= regaddr + 7)) //0x4E27, 0x4E28
    {
        sbuf[index]     = HIBYTE(LOWORD(iap_rltbit));
        sbuf[index + 1] = LOBYTE(LOWORD(iap_rltbit));
        sbuf[index + 2] = HIBYTE(HIWORD(iap_rltbit));
        sbuf[index + 3] = LOBYTE(HIWORD(iap_rltbit));
        index += 4;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 超时时间 */
    if(curaddr == regaddr + 13)
    {
        sbuf[index]     = HIBYTE((iap_ovtime / 60));
        sbuf[index + 1] = LOBYTE((iap_ovtime / 60));
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 保留 */
    if((curaddr >= regaddr + 14) && (curaddr <= regaddr + 15))
    {
        sbuf[index]     = 0x00;
        sbuf[index + 1] = 0x00;
        sbuf[index + 2] = 0x00;
        sbuf[index + 3] = 0x00;
        index += 4;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 传输命令 */
    if(curaddr == regaddr + 16)
    {
        sbuf[index]     = HIBYTE(iap_trancmd);
        sbuf[index + 1] = LOBYTE(iap_trancmd);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 目标级数 */
    if(curaddr == regaddr + 17)
    {
        sbuf[index]     = HIBYTE(iap_level);
        sbuf[index + 1] = LOBYTE(iap_level);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 保留 */
    if(curaddr == regaddr + 18)
    {
        sbuf[index]     = 0x00;
        sbuf[index + 1] = 0x00;
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 目标地址 */
    if(curaddr == regaddr + 19)
    {
        sbuf[index]     = 0x00;
        sbuf[index + 1] = 0x00;
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 软件版本号 */
    if((curaddr >= regaddr + 20) && (curaddr <= regaddr + 34))
    {
        memcpy(&sbuf[index], iap_softver, 30);
        index += 30;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 自动升级 */
    if(curaddr == regaddr + 35)  //0x4E44
    {
        sbuf[index]     = HIBYTE(iap_auto_flg);
        sbuf[index + 1] = LOBYTE(iap_auto_flg);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 保留 */
    if(curaddr == regaddr + 36)
    {
        sbuf[index]     = 0x00;
        sbuf[index + 1] = 0x00;
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 校验值 */
    if((curaddr >= regaddr + 37) && (curaddr <= regaddr + 38))
    {
        sbuf[index]     = HIBYTE(LOWORD(iap_crc32));
        sbuf[index + 1] = LOBYTE(LOWORD(iap_crc32));
        sbuf[index + 2] = HIBYTE(HIWORD(iap_crc32));
        sbuf[index + 3] = LOBYTE(HIWORD(iap_crc32));
        index += 4;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 总长度 */
    if((curaddr >= regaddr + 39) && (curaddr <= regaddr + 40))
    {
        sbuf[index]     = HIBYTE(LOWORD(iap_len_sum));
        sbuf[index + 1] = LOBYTE(LOWORD(iap_len_sum));
        sbuf[index + 2] = HIBYTE(HIWORD(iap_len_sum));
        sbuf[index + 3] = LOBYTE(HIWORD(iap_len_sum));
        index += 4;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 每包长度 */
    if(curaddr == regaddr + 41)
    {
        sbuf[index]     = HIBYTE(iap_pkglen);
        sbuf[index + 1] = LOBYTE(iap_pkglen);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 总包数 */
    if(curaddr == regaddr + 42)
    {
        sbuf[index]     = HIBYTE(iap_pkg_sum);
        sbuf[index + 1] = LOBYTE(iap_pkg_sum);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 包计数 */
    if(curaddr == regaddr + 43)
    {
        uint16_t tmpcnt = iapv2_get_max_setidx();
        sbuf[index]     = HIBYTE(tmpcnt);
        sbuf[index + 1] = LOBYTE(tmpcnt);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 向下发包计数 */
    if(curaddr == regaddr + 165)
    {
#if IAP_BOTH_HOST_SLAVE
        sbuf[index]     = HIBYTE(iapsl_sent_cnt);
        sbuf[index + 1] = LOBYTE(iapsl_sent_cnt);
#else
        sbuf[index]     = 0x00;
        sbuf[index + 1] = 0x00;
#endif
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 缺包计数 */
    uint16_t lostpkg = iapv2_get_emptyidx();
    uint32_t pkgaddr = 0;
    uint16_t pkglen = 0;
    iapv2_get_pkgattr_bycnt(lostpkg, &pkgaddr, &pkglen);
    if(curaddr == regaddr + 169)
    {
        sbuf[index]     = HIBYTE(lostpkg);
        sbuf[index + 1] = LOBYTE(lostpkg);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 缺包起始地址 */
    if((curaddr >= regaddr + 170) && (curaddr <= regaddr + 171))
    {
        sbuf[index]     = HIBYTE(LOWORD(pkgaddr));
        sbuf[index + 1] = LOBYTE(LOWORD(pkgaddr));
        sbuf[index + 2] = HIBYTE(HIWORD(pkgaddr));
        sbuf[index + 3] = LOBYTE(HIWORD(pkgaddr));
        index += 4;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 缺包数据长度 */
    if(curaddr == regaddr + 172)
    {
        sbuf[index]     = HIBYTE(pkglen);
        sbuf[index + 1] = LOBYTE(pkglen);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }

    /* 缺包总数 */
    if(curaddr == regaddr + 173)
    {
        sbuf[index]     = HIBYTE(iap_pkg_lost);
        sbuf[index + 1] = LOBYTE(iap_pkg_lost);
        index += 2;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        return index;
    }
    return index;
}

/* 返回执行结果，0表示出错，1表示正确，最高bit位为1表示无需回复主机 */
uint8_t iapv2_write_data(uint16_t addr, uint8_t rbuf[], uint8_t num)
{
    uint8_t ret = 0, ex = 0;
    uint16_t curaddr = addr;
    uint8_t len = num << 1;
    uint16_t index = 0;
    uint8_t cmd;
    uint16_t regaddr = MODBUS4X_ADDR;
    uint32_t curpkgaddr;
    uint16_t curpkglen;

    /* 命令，密码 */
    if((curaddr >= regaddr + 8) && (curaddr <= regaddr + 12))
    {
        if((rbuf[index + 2] == 0xA1) && (rbuf[index + 3] == 0x90) &&
                (rbuf[index + 4] == 0x75) && (rbuf[index + 5] == 0xF7) &&
                (rbuf[index + 6] == 0x09) && (rbuf[index + 7] == 0xA2) &&
                (rbuf[index + 8] == 0xC7) && (rbuf[index + 9] == 0x43))
        {
        	//rec: 00 00 00 00 00 19 01 10 4e 29 00 09 12 00 aa a1 90 75 f7 09 a2 c7 43 00 05 00 00 00 00 00 00
            if(rbuf[index + 1] == 0xAA)   //升级中
            {
                iap_status = IAP_UPDATING;
                iap_faults = 0;
                iap_level = 0;
                iap_chkver_flg = false;
                iap_cntbuflen = 0;         /* 缓冲区复位 */
                iap_isself_flg = true;     /* 默认是对自身升级 */
                iap_filesaved_flg = false; /* 默认文件未接收完成 */
                iap_rltbit = 0;
#if IAP_BOTH_HOST_SLAVE
                iap_addr1num = 0;
                iapsl_precmdtype = IAP_CMD_EXIT;
                iapsl_cmdtype = IAP_CMD_EXIT;
                iapsl_start_cmd = 0;
                iapsl_sent_cnt = 0;
#endif
            }
            else if(rbuf[index + 1] == 0x55) //不升级
            {
                iap_faults = 0;
                iap_status = IAP_NO_UPDATE;
                iap_ovtime_cnt = 0;
            }
        }
        else  //密码错误, 返回故障
        {
            iap_status = IAP_FAULT;
            iap_faults |= IAP_PASSWORD_WRONG;
        }
        index += 10;
    }
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 以下操作必须在正在升级状态下才能有效 */
    if(iap_status != IAP_UPDATING)
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    //printf("updating\n");

    /* 超时时间 */
    if(curaddr == regaddr + 13)  //0x4E2E
    {
        cmd = rbuf[index + 1];
        if((cmd >= 1) && (cmd <= 30))
        {
            iap_ovtime = cmd * 60;
        }
        else
        {
            iap_ovtime = 300; /* 默认超时值 */
        }
        index += 2;
    }

    //判断是否结束
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    iap_ovtime_cnt = iap_ovtime;  //超时时间

    /* 保留 */
    if((curaddr >= regaddr + 14) && (curaddr <= regaddr + 15)) //0x4E2F,0x4E30
    {
        index += 4;
    }

    //判断是否结束
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 传输命令 */
    if(curaddr == regaddr + 16)  //0x4E31
    {
        cmd = rbuf[index + 1];
        if(cmd == 0)
        {
            iap_trancmd = 0;
        }
        else
        {
            iap_trancmd = 1;
        }
        index += 2;
    }

    //判断是否结束
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 目标级数：如为0级则是对自身升级，如为1则是对下级升级 */
    //自身升级: 00 00 00 00 00 0d 01 10 4e 32 00 03 06 00 00 ff ff 00 01
    if(curaddr == regaddr + 17)  //0x4E32
    {
        iap_level = (rbuf[index] << 8) | rbuf[index + 1];
        index += 2;
    }

    //判断是否结束
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 保留 */
    if(curaddr == regaddr + 18)
    {
        index += 2;
    }

    //判断是否结束
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 目标地址 */
    if(curaddr == regaddr + 19)  //0x4E34
    {
#if IAP_TWO_LEVEL_MODE
        if(iap_level == 1)
        {
            if(iap_addr1num < IAP_MAX_DEVNUM)
            {
                iap_addr1[iap_addr1num] = (rbuf[index] << 8) | rbuf[index + 1];
                /* 用于判断该地址是否存在对应设备，若固件版本个数为0，则无该设备 */
                if(get_firmnum(1, iap_addr1[iap_addr1num]) != 0) 
                {
                    /* 检验是否地址重复，只保留唯一地址 */
                    if(iap_addr1[iap_addr1num] != 0)
                    {
                        uint8_t i;
                        for(i = 0; i < iap_addr1num; i++)
                        {
                            if(iap_addr1[iap_addr1num] == iap_addr1[i])
                            {
                                break;
                            }
                        }
                        if(i == iap_addr1num)
                        {
                            iap_addr1num++;
                        }
                    }
                }
            }
        }
#endif
        index += 2;
    }

    //判断是否结束
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 软件版本号 */
    if((curaddr >= regaddr + 20) && (curaddr <= regaddr + 34))  //0x4E35~0x4E43
    {
    	//rec: 00 00 00 00 00 35 01 10 4e 35 00 17 2e 4c 43 44 5f 45 4d 53 32 30 30 2d 56 31 31 5f 56 31 5f 41 5f 4d 00 00 00 00 00 00 00 00 00 00 aa 00 01 c4 55 ec af d0 f0 00 22 00 d0 2a da
        iap_chkver_flg = false;
        memcpy(iap_softver, &rbuf[index], 30);
        iapv2_do_attr_store(IAP_ATTR_BASE + 14, (uint8_t *)iap_softver, 30);
        index += 30;
        //printf("ver: %s\n", iap_softver);
    }

    //判断是否结束
    if (index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 自动升级 */
    if(curaddr == regaddr + 35)  //0x4E44
    {
        iap_auto_flg = (rbuf[index] << 8) | rbuf[index + 1];
        iapv2_do_attr_store(IAP_ATTR_BASE + 44, (uint8_t *)&iap_auto_flg, 2);
        index += 2;
    }
    
    /* 判断是否符合升级条件 */
    if(iap_chkver_flg == false)
    {
        iap_chkver_flg = true;
#if IAP_BOTH_HOST_SLAVE
        if(iap_auto_flg != 0xA5A5) /* backdoor*/
        {
            iap_isself_flg = iapv2_dest_isself(iap_softver);  /* 判断是否自身升级 */
            iap_verok_flg  = iapv2_checkver(iap_softver); /* 版本校验，并修改升级状态 */
            //printf("iap_isself_flg %d iap_verok_flg %d\n", iap_isself_flg,iap_verok_flg);
        }
        else
        {
            iap_verok_flg = true;
        }
#endif
    }

    //判断是否结束
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 保留 */
    if(curaddr == regaddr + 36)
    {
        index += 2;
    }

    //判断是否结束
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 校验值 */
    if((curaddr >= regaddr + 37) && (curaddr <= regaddr + 38))  //0x4E46,0x4E47
    {
        iap_crc32 = (rbuf[index] << 8) | rbuf[index+1];
        iap_crc32 |= (rbuf[index+2] << 24) | (rbuf[index+3] << 16);
        iapv2_do_attr_store(IAP_ATTR_BASE + 46, (uint8_t *)&iap_crc32, 4);
        index += 4;
        //printf("iap_crc32 0x%x\n", iap_crc32);
    }

    //判断是否结束
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 总长度 */
    if((curaddr >= regaddr + 39) && (curaddr <= regaddr + 40)) //0x4E48,0x4E49, 传过来的是16的倍数，比实际的长度长
    {
        iap_len_sum = (rbuf[index] << 8) | rbuf[index+1];
        iap_len_sum |= (rbuf[index+2] << 24) | (rbuf[index+3] << 16);
        iapv2_do_attr_store(IAP_ATTR_BASE + 50, (uint8_t *)&iap_len_sum, 4);
        index += 4;
    }

    //判断是否结束
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 每包长度 */
    if(curaddr == regaddr + 41)  //0x4E4A
    {
        iap_pkglen = (rbuf[index] << 8) | rbuf[index + 1];
        iapv2_do_attr_store(IAP_ATTR_BASE + 54, (uint8_t *)&iap_pkglen, 2);
        index += 2;
        //printf("iap_pkglen 0x%x\n", iap_pkglen);
    }

    //判断是否结束
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 总包数 */
    if(curaddr == regaddr + 42)  //0x4E4B
    {
        iap_pkg_sum = (rbuf[index] << 8) | rbuf[index + 1];
        index += 2;
        iapv2_do_attr_store(IAP_ATTR_BASE + 56, (uint8_t *)&iap_pkg_sum, 2);

        /* 总包数被设定，说明要进行一个新的升级过程，清除位表，设置缺包总数为总包数 */
        iap_pkg_lost = iap_pkg_sum;
        memset(iap_pkgbit, 0, sizeof(iap_pkgbit));
        iapv2_do_attr_store(IAP_ATTR_BASE + 62, (uint8_t *)iap_pkgbit, (iap_pkg_sum >> 3) + 4);
        //printf("iap_pkg_lost %d\n", iap_pkg_lost);
    }

    //判断是否结束
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

#if IAP_BOTH_HOST_SLAVE
    if(iapsl_start_cmd == 0)
    {
        iapsl_start_cmd = 1;
        /* 对本级非自身设备升级，且版本校验通过，则启动对本级非自身设备升级交互 */
        if((iap_level == 0) && (iap_verok_flg == true) && (iap_isself_flg == false))
        {
            iap_addr1num = iapv2_getlevel0_devaddr(iap_softver, iap_addr1);
            if(iap_addr1num != 0)
            {
                iapv2_slave_start();
            }
            else
            {
                iap_status = IAP_FAULT;
                iap_faults |= IAP_MODEL_WRONG;
            }
        }
    }
#endif

    /* 以下操作必须在正在升级状态下才能有效，且设置过包数和目标地址 */
    if((iap_status != IAP_UPDATING) || (iap_pkg_sum == 0)
#if IAP_BOTH_HOST_SLAVE
       || ((iap_level != 0) && (iap_addr1num == 0))
#endif
      )
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }

    /* 包计数，本包起始地址，本包数据长度，数据包 */
    if((curaddr >= regaddr + 43) && (curaddr <= regaddr + 165))  //0x4E4C~
    {
        uint16_t iap_pkg_cnt = (rbuf[index] << 8) | rbuf[index + 1];
        index += 2;
        curpkgaddr = IAP_FLASH_BASE + (rbuf[index] << 8) + rbuf[index+1];
        curpkgaddr += (rbuf[index+2] << 24) + (rbuf[index+3] << 16);
        index += 4;
        curpkglen = (rbuf[index] << 8) | rbuf[index + 1];
        index += 2;
        //printf("iap_pkg_cnt %d, curpkglen %d, iap_pkg_sum %d\n", iap_pkg_cnt,curpkglen,iap_pkg_sum);
        if((iap_pkg_cnt > 0) && (iap_pkg_cnt <= iap_pkg_sum))
        {
            /* 第一次接收数据，初始化FLASH缓冲区 */
            if(iap_pkg_lost == iap_pkg_sum)
            {
                iapv2_erase_all_program();
                open_update_file(iap_softver, (char*)"wb");
            }

            if(iapv2_get_pkgbit(iap_pkg_cnt) == 0) /* 该包未接收过 */
            {
                /* 解密，存数据，对本级升级，则由本级解密；对下级升级，则本级不解密 */
#if IAP_BOTH_HOST_SLAVE
                if(iap_level == 0)  //解码
                {
                    iap_pkg_decrypt(&rbuf[index], curpkglen);
                	if(iap_pkg_lost == 1) {
                		for(int j=curpkglen-1; j >= curpkglen-16; j--) {
                			if(rbuf[index+j] == 0xFF) ex++;
                		}
                		//printf("dat:");for(int j=0; j<curpkglen; j++) printf(" %02x", rbuf[index+j]);printf("\r\n");
                	}
                }
#endif
                iapv2_set_pkgbit(iap_pkg_cnt);     /* 位表对应包置位 */
                iapv2_do_program_store(curpkgaddr, &rbuf[index], curpkglen-ex);
                iap_pkg_lost--;                    /* 更新缺包数 */

                /* 将包计数加入包计数缓区 */
                if(iap_cntbuflen < IAP_CNTBUF_LEN)
                {
                    iap_cntbuf[iap_cntbuflen] = iap_pkg_cnt;
                    iap_cntbuflen++;

#if IAP_BOTH_HOST_SLAVE
                    /* 记录接收缓冲区中的最大包计数 */
                    if(iapsl_max_pkgcnt < iap_pkg_cnt)
                    {
                        iapsl_max_pkgcnt = iap_pkg_cnt;
                    }
#endif
                }
            }

            if(iap_pkg_lost == 0)
            {
            	close_update_file();
            	iap_len_sum -= ex;       //数据总长度修正
                iap_chkcrc_flg = true;   //包接收完成
            }
        }
        index += curpkglen;
    }
    if(index < len)
    {
        curaddr = (index >> 1) + addr;
    }
    else
    {
        ret = (uint8_t)(index != 0);
        return ret;
    }
    ret = (uint8_t)(index != 0);
    return ret;
}

bool iapv2_check_pkg(void)
{
    uint8_t buf[32];
    uint32_t i = 0, lcrc = 0, *ubuf;
    uint32_t addr = IAP_FLASH_BASE;
    ubuf = (uint32_t *)buf;

    open_update_file(iap_softver, (char*)"rb");
    for (i = 0; i < iap_len_sum / 32; ++i)
    {
        iapv2_do_program_extract(addr, buf, 32);
#if IAP_TWO_LEVEL_MODE
        if(iap_level == 1)
        {
            iap_pkg_decrypt(buf, 32);
        }
#endif
        lcrc = CRC32(ubuf, 8);
        addr += 32;
    }

    i = iap_len_sum % 32;
    if (i > 0)
    {
        memset(buf, 0, 32);
        iapv2_do_program_extract(addr, buf, i);
#if IAP_TWO_LEVEL_MODE
        if(iap_level == 1)
        {
            iap_pkg_decrypt(buf, i);
        }
#endif
        if ((i % 4) == 0)
        {
            lcrc = CRC32(ubuf, i / 4);
        }
        else
        {
            lcrc = CRC32(ubuf, (i / 4) + 1);
        }
    }
    close_update_file();
    printf("iap_crc32 0x%x, lcrc 0x%x\n", iap_crc32, lcrc);
    if(iap_crc32 == lcrc)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* 升级结束后处理函数，清除数据 */
void iapv2_over_proc(void)
{
    memset(iap_softver, 0, sizeof(iap_softver));
    memset(iap_pkgbit, 0, sizeof(iap_pkgbit));
    iap_auto_flg = 0x55;
    iap_crc32 = 0;
    iap_len_sum = 0;
    iap_pkglen = 0;
    iap_pkg_sum = 0;
    iap_pkg_lost = 0;
    iapv2_do_attr_store(IAP_ATTR_BASE + 14, (uint8_t *)iap_softver, 30);
    iapv2_do_attr_store(IAP_ATTR_BASE + 44, (uint8_t *)&iap_auto_flg, 2);
    iapv2_do_attr_store(IAP_ATTR_BASE + 46, (uint8_t *)&iap_crc32, 4);
    iapv2_do_attr_store(IAP_ATTR_BASE + 50, (uint8_t *)&iap_len_sum, 4);
    iapv2_do_attr_store(IAP_ATTR_BASE + 54, (uint8_t *)&iap_pkglen, 2);
    iapv2_do_attr_store(IAP_ATTR_BASE + 56, (uint8_t *)&iap_pkg_sum, 2);
    iapv2_do_attr_store(IAP_ATTR_BASE + 62, (uint8_t *)iap_pkgbit, (iap_pkg_sum >> 3) + 4);
}

uint8_t iapv2_init(void)
{
    uint8_t updated = 0;
    uint16_t status = 0;
    uint16_t cmd = 0;
    iapv2_do_attr_extract(IAP_ATTR_BASE + 8, (uint8_t *)&status, 2);
    if (status == IAP_SUCCESSED)
    {
        /* 升级成功，将所有配置数据清除 */
        updated = 1;
        iap_status = IAP_SUCCESSED;
        iapv2_over_proc();
    }
    else
    {
        iap_status = IAP_NO_UPDATE;
        iapv2_do_attr_extract(IAP_ATTR_BASE + 14, (uint8_t *)iap_softver, 30);
        iapv2_do_attr_extract(IAP_ATTR_BASE + 44, (uint8_t *)&iap_auto_flg, 2);
        iapv2_do_attr_extract(IAP_ATTR_BASE + 46, (uint8_t *)&iap_crc32, 4);
        iapv2_do_attr_extract(IAP_ATTR_BASE + 50, (uint8_t *)&iap_len_sum, 4);
        iapv2_do_attr_extract(IAP_ATTR_BASE + 54, (uint8_t *)&iap_pkglen, 2);
        iapv2_do_attr_extract(IAP_ATTR_BASE + 56, (uint8_t *)&iap_pkg_sum, 2);
        if((iap_pkg_sum != 0) && (((iap_pkg_sum >> 3) + 4) < (uint16_t)sizeof(iap_pkgbit)))
        {
            iapv2_do_attr_extract(IAP_ATTR_BASE + 62, (uint8_t *)iap_pkgbit, (iap_pkg_sum >> 3) + 4);
        }
        iap_pkg_lost = iapv2_get_total_lostcnt();
        if((uint32_t)(iap_pkglen * iap_pkg_sum) < iap_len_sum)
        {
            iapv2_over_proc();
        }

        if((iap_auto_flg != 0x55) && (iap_auto_flg != 0xAA))
        {
            iap_auto_flg = 0x55;
        }
    }
    iap_faults = 0;

    /* 复位EEPROM中存储的升级命令和升级状态 */
    cmd = IAP_NOCMD;
    iapv2_do_attr_store(IAP_ATTR_BASE + 6, (uint8_t *)&cmd, 2);
    status = IAP_NO_UPDATE;
    iapv2_do_attr_store(IAP_ATTR_BASE + 8, (uint8_t *)&status, 2);
    return updated;
}

void iapv2_10ms_proc(void)
{
    if(iap_sec_time_cnt < 100)
    {
        iap_sec_time_cnt++;
    }
    else
    {
        iap_sec_time_cnt = 0;
        iapv2_sec_proc();
    }

#if 0//IAP_BOTH_HOST_SLAVE
    uint16_t timeout;
    if(iapsl_retflg == false)
    {
        iapsl_resend_cnt++;
        if((iapsl_stage == 1) && (iapsl_broadcasting == true))
        {
            /* 广播时，第一包延时3s，其余延时iapsl_brperiod */
            if((iapsl_first_send == 1) && (iapsl_newupdate_flg == 1))
            {
                timeout = 300;
            }
            else
            {
                timeout = iapsl_brperiod;
            }

            if(iapsl_resend_cnt == timeout)
            {
                iapsl_first_send = 0;
                /* 广播中 */
                if(iapsl_pkg_cnt == iap_pkg_sum)
                {
                    iapv2_exitstage1_proc();
                }
                else
                {
                    iapv2_refresh_pkgcnt();
                }
                iapsl_retflg = true;
                iapsl_resend_cnt = 0;
            }
        }
        else if((iapsl_stage == 2) && (iapsl_multisend_lost == 1) && (iapsl_cmdtype == IAP_CMD_SET_DATASEND))
        {
            if(iapsl_resend_cnt == iapsl_brperiod)
            {
                iapsl_resend_cnt = 0;
                iapsl_precmdtype = iapsl_cmdtype;
                iapsl_cmdtype = IAP_CMD_READ_LOST;
                iapsl_retflg = true;
            }
        }
        else
        {
            if(iapsl_resend_cnt == 200)
            {
                /* 超时主机重发命令 */
                iapsl_retflg = true;
                iapsl_resend_cnt = 0;

                /* 超时通信不上 */
                if(iapsl_resend_try == IAP_COM_TRYTIMES)
                {
                    iapsl_resend_try = 0;
                    iapsl_status[iapsl_curdev] = IAP_FAULT;
                    iapsl_faults[iapsl_curdev] |= IAP_COM_ERROR;
                    iapv2_err_proc();
                }
                iapsl_resend_try++;
            }
        }
        if(iapsl_cmdtype != IAP_CMD_SET_LOGOUT)
        {
            /* 检测到主机出错或上位机退出登录时，向下发送退出登录命令 */
            if((iap_status == IAP_NO_UPDATE) || (iap_status == IAP_FAULT))
            {
                iapsl_cmdtype = IAP_CMD_SET_LOGOUT;
                iapsl_broadcasting = false;
                iapsl_curdev = 0;
                iapv2_find_next_updatingdev();
                if(iapsl_curdev == iap_addr1num)
                {
                    iapv2_slave_exit();
                }
                iapsl_retflg = true;
            }
        }
    }
    else
    {
        iapsl_resend_cnt = 0;
    }
#endif
}

void iapv2_sec_proc(void)
{
    uint16_t cmd = 0;
    uint16_t status = 0;

    if(iap_reset_cnt != 0)
    {
        iap_reset_cnt--;
        if (iap_reset_cnt == 0)
        {
            //复位
        	FILE * fp;
    		if ((fp = popen("reboot", "w") ) != NULL){
    			pclose(fp);
    		}
        }
    }

    /* 超时计数，超时后设定升级状态为故障超时 */
    if (iap_ovtime_cnt != 0)
    {
        iap_ovtime_cnt--;
        if (iap_ovtime_cnt == 0)
        {
            iap_status = IAP_FAULT;
            iap_faults |= IAP_TIMEOUT;
        }
    }

    if(iap_chkcrc_flg == true)  //整体校验
    {
        //if(iapv2_check_pkg() == true)
        if(1)
        {
            iap_ovtime_cnt = 0;
            iap_filesaved_flg = true;  //文件接收完成
            if(iap_level == 0)
            {
                /* 对自身升级 */
                if(iap_isself_flg == true)
                {
                    iap_reset_cnt = 3;
                    cmd = IAP_UPDATE;
                    iapv2_do_attr_store(IAP_ATTR_BASE + 6, (uint8_t *)&cmd, 2);
                    status = IAP_SUCCESSED;
                    iapv2_do_attr_store(IAP_ATTR_BASE + 8, (uint8_t *)&status, 2);
                    save_eeprom();
                }
                else
                {
                    /* 对本级非自身设备升级，已经开始，此处只是文件接收完成 */
                }
            }
#if IAP_TWO_LEVEL_MODE
            else
            {
                /* 对下级设备升级，数据初始化，iapsl_status在固件版本检查中已经初始化 */
                iap_status = IAP_SUBUPDATING;
                iapv2_slave_start();
            }
#endif
        }
        else
        {
            iap_status = IAP_FAULT;
            iap_faults |= IAP_CRCCHK_FAILED;
            iap_ovtime_cnt = 0;
            printf("iapv2_over_proc\n");
            iapv2_over_proc();
        }
        iap_chkcrc_flg = false;
    }
}

#if IAP_BOTH_HOST_SLAVE

/* 启动从机升级程序 */
void iapv2_slave_start(void)
{
    iapsl_cmdtype = IAP_CMD_READ_IAPVER;
    iapsl_precmdtype = IAP_CMD_READ_IAPVER;
    iapsl_resend_try = 0;
    iapsl_resend_cnt = 0;
    iapsl_stage = 0;
    iapsl_retflg = true;
    iapsl_broadcasting = false;
    memset(iapsl_status, 0, IAP_MAX_DEVNUM);
    memset(iapsl_faults, 0, IAP_MAX_DEVNUM);
    iapsl_curdev = 0;
    iapsl_pkg_cnt = 1;
    iapsl_sent_cnt = iapsl_pkg_cnt;
    iapsl_brperiod = iapv2_get_broadcast_period() / 10;
    uint8_t num = iapv2_getlevel0_devaddr(iap_softver, NULL);
    if((iap_level == 0) && (num > 1))
    {
        iap_ovtime += (num - 1) * 300;
    }
    iapsl_first_send = 1;
    iapsl_max_pkgcnt = 0;
    iapsl_multisend_lost = 0;
    iapsl_newupdate_flg = 0;
}

/* 退出从机升级 */
void iapv2_slave_exit(void)
{
    /* 异常:向下升级结束 */
    iapsl_precmdtype = IAP_CMD_EXIT;
    iapsl_cmdtype = IAP_CMD_EXIT;
    uint8_t devok_cnt = 0;
    if(iap_level == 0)
    {
        iap_status = IAP_UPEND;

        /* 置本级设备升级结果 */
        for(uint8_t i = 0; i < iap_addr1num; i++)
        {
            if(iapsl_status[i] == IAP_SUCCESSED)
            {
                iap_rltbit |= (1 << i);
                devok_cnt++;
            }
        }
    }
    else
    {
        iap_status = IAP_SUBUPEND;
        /* 向下升级，接收正确时，不清除数据 */
    }
    /* 清除升级数据 */
    if(devok_cnt == iap_addr1num)
    {
        iapv2_over_proc();
    }
}

/* 下发包的包计数刷新 */
void iapv2_refresh_pkgcnt(void)
{
    if((iap_level == 0) && (iap_isself_flg == false))
    {
        if(iap_cntbuflen != 0)
        {
            iapsl_pkg_cnt = iap_cntbuf[iap_cntbuflen - 1];
            iap_cntbuflen--;
        }
        else
        {
            if(iap_pkg_lost == 0)  /* 主机接收完上位机发送的升级数据包 */
            {
                if(iapsl_max_pkgcnt != iap_pkg_sum)
                {
                    iapsl_max_pkgcnt++;
                    iapsl_pkg_cnt = iapsl_max_pkgcnt;
                }
            }
            else
            {
            if(iapv2_get_pkgbit(iapsl_pkg_cnt + 1) == 1)
            {
                iapsl_pkg_cnt++;
            }
        }
    }
    }
    else
    {
        iapsl_pkg_cnt++;
    }
}

/* 定位到下一个状态为正在升级的设备 */
void iapv2_find_next_updatingdev(void)
{
    while(iapsl_curdev < iap_addr1num)
    {
        if(iapsl_status[iapsl_curdev] == IAP_UPDATING)
        {
            break;
        }
        iapsl_curdev++;
    }
}

/* 握手阶段(0)结束，进入传输阶段(1)前预处理 */
void iapv2_enterstage1_preproc(void)
{
    /* 握手结束，选择传输方式，并开始数据传输 */
    iapsl_cmdtype = IAP_CMD_SET_DATASEND;

    /* 定位到首个状态为正在升级的设备，传输结束后从这一设备开始补包 */
    iapsl_curdev = 0;
    iapv2_find_next_updatingdev();

    /* 握手结束，没有一个设备处于正在升级状态 */
    if(iapsl_curdev == iap_addr1num)
    {
        /* 异常:向下升级结束 */
        iapv2_slave_exit();
    }
    else
    {
        if(iap_addr1num > 1)
        {
            iapsl_broadcasting = true;
        }
        else
        {
            iapsl_broadcasting = false;
        }
        iapsl_stage = 1;
    }
    iapsl_sent_cnt = iapsl_pkg_cnt;
}

/* 广播阶段(0)结束，进入补包阶段(2)处理 */
void iapv2_exitstage1_proc(void)
{
    /* 数据传输结束，查询缺包 */
    iapsl_precmdtype = IAP_CMD_SET_DATASEND;
    iapsl_cmdtype = IAP_CMD_READ_LOST;
    iapsl_broadcasting = false;
    iapsl_stage = 2;
    if(iap_addr1num > 1)
    {
        iapsl_multisend_lost = 1;
    }
}

/* 出错处理: 超时或者数据交互错误 */
void iapv2_err_proc(void)
{
    if(iapsl_stage == 0)
    {
        iapsl_curdev = (iapsl_curdev + 1) % iap_addr1num;
        if(iapsl_curdev == 0)
        {
            iapv2_enterstage1_preproc();
        }
        else
        {
            iapsl_cmdtype = IAP_CMD_READ_IAPVER;
        }
    }
    else if(iapsl_stage == 1)
    {
        /* 异常:向下升级结束 */
        iapv2_slave_exit();
    }
    else
    {
        iapv2_find_next_updatingdev();
        if(iapsl_curdev == iap_addr1num)
        {
            /* 异常:向下升级结束 */
            iapv2_slave_exit();
        }
        else
        {
            iapsl_precmdtype = IAP_CMD_SET_DATASEND;
            iapsl_cmdtype = IAP_CMD_READ_LOST;
        }
    }
}

/* 作为主机，对下级设备进行IAP升级 */
void iapv2_call_slave(uint8_t sendbuf[], uint8_t len)
{
    if(iapsl_retflg == true && iapsl_cmdtype != IAP_CMD_EXIT)
    {
        iapv2_send_cmd(iap_addr1[iapsl_curdev], iapsl_cmdtype, sendbuf);
        iapsl_retflg = false;
    }
}

/* 判断是否正在向下升级 */
bool iapv2_slave_updating(void)
{
    bool ret = false;
    if(iapsl_cmdtype != IAP_CMD_EXIT)
    {
        ret = true;
    }
    return ret;
}

/* 判断是否正在升级 */
bool iapv2_updating(void)
{
    return (bool)(iap_status == IAP_UPDATING);
}

extern uint16_t embf_get_crc16(volatile uint8_t *ptr,uint16_t len);

/* 向下级设备发送IAP升级命令 */
void iapv2_send_cmd(uint8_t devaddr, uint8_t cmd_type, uint8_t buf[])
{
    uint8_t len;
    uint16_t crc;
    len = iapv2_package_cmd(devaddr, cmd_type, buf);
    if(len != 0)
    {
        crc = embf_get_crc16(buf, len);
        buf[len]     = LOBYTE(crc);
        buf[len + 1] = HIBYTE(crc);
        iapv2_do_call_slave(buf, len + 2);
        iapsl_retflg = false;
    }
}

/* 从机返回数据处理 */
void iapv2_slave_return(uint8_t buf[])
{
    uint16_t regaddr;
    uint16_t tmp_pkg_lost;
    uint32_t tmpcrc32;
    uint16_t tmpcnt;
    if((buf[0] == iap_addr1[iapsl_curdev]) && (iapsl_broadcasting == false))
    {
        if(buf[1] == MODBUS_CMD_03)
        {
            if((buf[2] == 8) && (iapsl_cmdtype == IAP_CMD_READ_STATUS))
            {
                iapsl_status[iapsl_curdev] = (buf[5] << 8) + buf[6];
                iapsl_faults[iapsl_curdev] = (buf[9] << 24) + (buf[10] << 16) + (buf[7] << 8) + buf[8];
                if((iapsl_precmdtype == IAP_CMD_READ_SOFTVER) || (iapsl_precmdtype == IAP_CMD_SET_SOFTVER))
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    iapsl_curdev = (iapsl_curdev + 1) % iap_addr1num;
                    if(iapsl_curdev == 0)
                    {
                        iapv2_enterstage1_preproc();
                    }
                    else
                    {
                        /* 继续下一台设备握手 */
                        iapsl_cmdtype = IAP_CMD_READ_IAPVER;
                    }
                    iapsl_retflg = true;
                }
                else if((iapsl_precmdtype == IAP_CMD_READ_LOST) || (iapsl_precmdtype == IAP_CMD_READ_STATUS))
                {
                    /* 文件传输结束，查询状态 */
                    iapsl_precmdtype = iapsl_cmdtype;

                    /* 状态非正在升级，表示从机收完升级数据后状态已更新，读下一台 */
                    if(iapsl_status[iapsl_curdev] != IAP_UPDATING)
                    {
                        iapv2_find_next_updatingdev();
                    }

                    if(iapsl_curdev == iap_addr1num)
                    {
                        /* 向下升级结束 */
                        iapv2_slave_exit();
                    }
                    else
                    {
                        iapsl_cmdtype = IAP_CMD_READ_STATUS;
                    }
                    iapsl_retflg = true;
                }
            }
            else if((buf[2] == 10) && (iapsl_cmdtype == IAP_CMD_READ_LOST))
            {
                iapsl_pkg_cnt = (buf[3] << 8) + buf[4];     /* 缺包计数 */
                tmp_pkg_lost = (buf[11] << 8) + buf[12];  /* 缺包总数 */
                if((iapsl_precmdtype == IAP_CMD_SET_DATASEND) || (iapsl_precmdtype == IAP_CMD_READ_LOST))
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    if((tmp_pkg_lost != 0) && (iapsl_pkg_cnt != 0))
                    {
                        iapsl_cmdtype = IAP_CMD_SET_DATASEND;
                    }
                    else
                    {
                        /* 定位到下一台处于正在升级状态的设备 */
                        iapsl_curdev++;
                        iapv2_find_next_updatingdev();
                        if(iapsl_curdev == iap_addr1num)
                        {
                            /* 向下升级结束，查设备状态 */
                            iapsl_cmdtype = IAP_CMD_READ_STATUS;

                            /* 定位到第一台处于正在升级状态的设备 */
                            iapsl_curdev = 0;
                            iapv2_find_next_updatingdev();
                        }
                        else
                        {
                            /* 继续下一台设备补包 */
                            iapsl_cmdtype = IAP_CMD_READ_LOST;
                        }
                    }
                    iapsl_retflg = true;
                }
            }
            else if((buf[2] == 48) && (iapsl_cmdtype == IAP_CMD_READ_SOFTVER))
            {
                tmpcrc32 = (buf[39] << 24) + (buf[40] << 16) + (buf[37] << 8) + buf[38];
                tmpcnt = (buf[49] << 8) + buf[50];
                if(iapsl_precmdtype == IAP_CMD_SET_DEST)
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    if((tmpcrc32 == iap_crc32) && (tmpcnt != 0) &&
                            (strncmp(iap_softver, (char *)&buf[3], 30) == 0))
                    {
                        /* 读取的版本号、校验码一致，且包计数不为0，断点续传 */
                        iapsl_cmdtype = IAP_CMD_READ_STATUS;
                        if((iapsl_curdev == 0) || (tmpcnt < iapsl_pkg_cnt))
                        {
                            iapsl_pkg_cnt = tmpcnt; /* 记录最小包计数 */
                        }
                    }
                    else
                    {
                        /* 新的升级过程 */
                        iapsl_newupdate_flg = 1;
                        iapsl_cmdtype = IAP_CMD_SET_SOFTVER;
                        iapsl_pkg_cnt = 1;
                    }
                    iapsl_retflg = true;
                }
            }
            else if((buf[2] == 4) && (iapsl_cmdtype == IAP_CMD_READ_IAPVER))
            {
                if((buf[3] == 0x01) && (buf[4] == 0x00) && (buf[5] == 0x02) && (buf[6] == 0x00))
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    iapsl_cmdtype = IAP_CMD_SET_LOGIN;
                    iapsl_retflg = true;
                }
                else
                {
                    iapsl_status[iapsl_curdev] = IAP_FAULT;
                    iapsl_faults[iapsl_curdev] |= IAP_EXIT;
                    iapv2_err_proc();
                }
                iapsl_retflg = true;
            }
        }
        else if(buf[1] == MODBUS_CMD_10)
        {
            regaddr = (buf[2] << 8) + buf[3] - MODBUS4X_ADDR;
            if((regaddr == 43) && (iapsl_cmdtype == IAP_CMD_SET_DATASEND))
            {
                if(iapsl_precmdtype == IAP_CMD_READ_LOST)
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    iapsl_cmdtype = IAP_CMD_READ_LOST;
                    iapsl_retflg = true;
                }
                else if((iapsl_precmdtype == IAP_CMD_READ_STATUS) || (iapsl_precmdtype == IAP_CMD_SET_DATASEND))
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    if(iapsl_pkg_cnt == iap_pkg_sum)
                    {
                        /* 数据传输结束，查询缺包 */
                        iapsl_cmdtype = IAP_CMD_READ_LOST;
                        iapsl_stage = 2;
                    }
                    else
                    {
                        iapv2_refresh_pkgcnt();
                    }
                    iapsl_retflg = true;
                }
            }
            else if((regaddr == 8) && (buf[5] == 0x09) && (iapsl_cmdtype == IAP_CMD_SET_LOGIN))
            {
                if(iapsl_precmdtype == IAP_CMD_READ_IAPVER)
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    iapsl_cmdtype = IAP_CMD_SET_DEST;
                    iapsl_retflg = true;
                }
            }
            else if((regaddr == 8) && (buf[5] == 0x05) && (iapsl_cmdtype == IAP_CMD_SET_LOGOUT))
            {
                iapsl_curdev++;
                iapv2_find_next_updatingdev();
                if(iapsl_curdev == iap_addr1num)
                {
                    iapsl_curdev = 0;
                    iapsl_precmdtype = IAP_CMD_EXIT;
                    iapsl_cmdtype = IAP_CMD_EXIT;
                }
                iapsl_retflg = true;
            }
            else if((regaddr == 17) && (buf[5] == 0x03) && (iapsl_cmdtype == IAP_CMD_SET_DEST))
            {
                if(iapsl_precmdtype == IAP_CMD_SET_LOGIN)
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    iapsl_cmdtype = IAP_CMD_READ_SOFTVER;
                    iapsl_retflg = true;
                }
            }
            else if((regaddr == 20) && (buf[5] == 0x17) && (iapsl_cmdtype == IAP_CMD_SET_SOFTVER))
            {
                if(iapsl_precmdtype == IAP_CMD_READ_SOFTVER)
                {
                    iapsl_precmdtype = iapsl_cmdtype;
                    iapsl_cmdtype = IAP_CMD_READ_STATUS;
                    iapsl_retflg = true;
                }
            }
        }
        else if(buf[1] == (MODBUS_CMD_10 | 0x80))
        {
            iapsl_status[iapsl_curdev] = IAP_FAULT;
            iapsl_faults[iapsl_curdev] |= IAP_EXIT;
            iapv2_err_proc();
            iapsl_retflg = true;
        }
    }
    if(iapsl_retflg == true)
    {
        iapsl_resend_cnt = 0;
        iapsl_resend_try = 0;
    }
}

/* 打包IAP主机命令 */
uint8_t iapv2_package_cmd(uint8_t devaddr, uint8_t cmd_type, uint8_t buf[])
{
    uint8_t cmd;
    uint16_t regaddr;
    uint8_t regnum = 0;
    uint8_t idx = 0;
    uint8_t err = 0;
    uint32_t curpkgaddr;  /* 当前发送的包起始地址 */
    uint16_t curpkglen;   /* 当前发送的包长度 */
    switch(cmd_type)
    {
    case IAP_CMD_READ_IAPVER:
        cmd = MODBUS_CMD_03;
        regaddr = 0x4E1F;
        regnum = 0x02;
        break;
    case IAP_CMD_READ_STATUS:
        cmd = MODBUS_CMD_03;
        regaddr = MODBUS4X_ADDR + 2;
        regnum = 0x04;
        break;
    case IAP_CMD_READ_SOFTVER:
        cmd = MODBUS_CMD_03;
        regaddr = MODBUS4X_ADDR + 20;
        regnum = 0x18;
        break;
    case IAP_CMD_READ_LOST:
        cmd = MODBUS_CMD_03;
        regaddr = MODBUS4X_ADDR + 169;
        regnum = 0x05;
        break;
    case IAP_CMD_SET_LOGIN:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 8;
        regnum = 0x09;
        buf[7] = 0x00;
        buf[8] = 0xAA;
        buf[9] = 0xA1;
        buf[10] = 0x90;
        buf[11] = 0x75;
        buf[12] = 0xF7;
        buf[13] = 0x09;
        buf[14] = 0xA2;
        buf[15] = 0xC7;
        buf[16] = 0x43;
        buf[17] = 0x00;
        buf[18] = iap_ovtime / 60;
        buf[19] = 0x00;
        buf[20] = 0x00;
        buf[21] = 0x00;
        buf[22] = 0x00;
        /* 单台则查询传输，否则广播 */
        buf[23] = 0x00;
        if(iap_addr1num > 1)
        {
            buf[24] = 0x01;
        }
        else
        {
            buf[24] = 0x00;
        }
        break;
    case IAP_CMD_SET_LOGOUT:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 8;
        regnum = 0x05;
        buf[7] = 0x00;
        buf[8] = 0x55;
        buf[9] = 0xA1;
        buf[10] = 0x90;
        buf[11] = 0x75;
        buf[12] = 0xF7;
        buf[13] = 0x09;
        buf[14] = 0xA2;
        buf[15] = 0xC7;
        buf[16] = 0x43;
        break;
    case IAP_CMD_SET_DEST:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 17;
        regnum = 0x03;
        buf[7] = 0x00;
        buf[8] = 0x00;
        buf[9] = 0x00;
        buf[10] = 0x00;
        buf[11] = 0x00;
        buf[12] = devaddr;
        break;
    case IAP_CMD_SET_SOFTVER:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 20;
        regnum = 0x17;
        memcpy(&buf[7], iap_softver, 30);
        buf[37] = 0x00;
        buf[38] = 0xAA;  /* 0级设备已经判断并通过版本号匹配，此处强制升级 */
        buf[39] = 0x00;
        buf[40] = 0x00;
        buf[41] = HIBYTE(LOWORD(iap_crc32));
        buf[42] = LOBYTE(LOWORD(iap_crc32));
        buf[43] = HIBYTE(HIWORD(iap_crc32));
        buf[44] = LOBYTE(HIWORD(iap_crc32));
        buf[45] = HIBYTE(LOWORD(iap_len_sum));
        buf[46] = LOBYTE(LOWORD(iap_len_sum));
        buf[47] = HIBYTE(HIWORD(iap_len_sum));
        buf[48] = LOBYTE(HIWORD(iap_len_sum));
        buf[49] = HIBYTE(iap_pkglen);
        buf[50] = LOBYTE(iap_pkglen);
        buf[51] = HIBYTE(iap_pkg_sum);
        buf[52] = LOBYTE(iap_pkg_sum);
        break;
    case IAP_CMD_SET_DATASEND:
        if(iapv2_get_pkgbit(iapsl_pkg_cnt) == 1)
        {
            if(iapsl_sent_cnt < iap_pkg_sum)
            {
                iapsl_sent_cnt++;
            }
            cmd = MODBUS_CMD_10;
            regaddr = MODBUS4X_ADDR + 43;
            iapv2_get_pkgattr_bycnt(iapsl_pkg_cnt, &curpkgaddr, &curpkglen);
            regnum  = 4 + (curpkglen >> 1);
            buf[7]  = HIBYTE(iapsl_pkg_cnt);
            buf[8]  = LOBYTE(iapsl_pkg_cnt);
            buf[9]  = HIBYTE(LOWORD(curpkgaddr));
            buf[10] = LOBYTE(LOWORD(curpkgaddr));
            buf[11] = HIBYTE(HIWORD(curpkgaddr));
            buf[12] = LOBYTE(HIWORD(curpkgaddr));
            buf[13] = HIBYTE(curpkglen);
            buf[14] = LOBYTE(curpkglen);
            iapv2_do_program_extract(IAP_FLASH_BASE + curpkgaddr, &buf[15], curpkglen);
        }
        else
        {
            iapv2_refresh_pkgcnt();
            err = 1;
        }
        break;
    default:
        err = 1;
        break;
    }
    if(err == 0)
    {
        if(((iapsl_stage == 1) && (iapsl_broadcasting == true)) || \
           ((iapsl_stage == 2) && (iapsl_multisend_lost == 1) && (cmd_type == IAP_CMD_SET_DATASEND)))
        {
            buf[0] = 0x00;
        }
        else
        {
            buf[0] = devaddr;
        }
        buf[1] = cmd;
        buf[2] = HIBYTE(regaddr);
        buf[3] = LOBYTE(regaddr);
        buf[4] = 0x00;
        buf[5] = regnum;
        if(cmd == MODBUS_CMD_10)
        {
            buf[6] = regnum << 1;
            idx = 7 + buf[6];
        }
        else
        {
            idx = 6;
        }
    }
    return idx;
}

#endif


