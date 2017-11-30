/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : MQTTRTU.h
 * Author         : xusa
 * Date           : 2014.12.22
 * Description    : mqtt协议的服务器接口部分，包括通讯层、协议解析层、应用服务层
 *                  主体调用
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2014.12.22 : 1.0.0 : xusa
 * Modification   : 初始代码编写
 ******************************************************************************/
#ifndef _MQTTRTU_H_
#define _MQTTRTU_H_

#include <string.h>
#include <stdint.h>
//#include "lwip/tcp.h"

#define MQTT_RCV_BUF_LEN   1536
#define MQTT_SEND_BUF_LEN  1536

typedef enum{
	MQTT_Reserved,
	MQTT_CONNECT,
	MQTT_CONNACK,
	MQTT_PUBLISH,
	MQTT_PUBACK,
	MQTT_PUBREC,
	MQTT_PUBREL,
	MQTT_PUBCOMP,
	MQTT_SUBSCRIBE,
	MQTT_SUBACK,
	MQTT_UNSUBSCRIBE,
	MQTT_UNSUBACK,
	MQTT_PINGREQ,
	MQTT_PINGRESP,
	MQTT_DISCONNECT,
}MQTT_CODE;


typedef enum{
	MQTT_NULL_DATA=0,  
	MQTT_INT8_DATA,  
	MQTT_UINT8_DATA,  
	MQTT_INT16_DATA,   
	MQTT_UINT16_DATA, 
	MQTT_INT32_DATA,   
	MQTT_UINT32_DATA,  
	MQTT_INT64_DATA,   
	MQTT_UINT64_DATA,  
	MQTT_FLOAT_DATA,   
	MQTT_DOUBLE_DATA,  
	MQTT_BOOL_DATA,    
	MQTT_STRING_DATA,  
	MQTT_BITSTRING_DATA,  
	MQTT_DTAETIME_DATA,   
	MQTT_SIMPLETIME_DATA,
	MQTT_STATECHANGE_DATA,
}MQTT_DATA_TYPE;


typedef enum{
	MQTT_DEV_NULL1,
	MQTT_DEV_INT,
	MQTT_DEV_CONTAINER,
	MQTT_DEV_POWER,
	MQTT_DEV_BOX, 
	MQTT_DEV_EM, 	
	MQTT_DEV_XB,      
	MQTT_DEV_METER, 
	MQTT_DEV_UPS,	
	MQTT_DEV_LOGGER, 
	MQTT_DEV_BATTERY,  
	MQTT_DEV_PLANT,   
	MQTT_DEV_PROTECTION,  
	MQTT_DEV_SPLITTING, 
	MQTT_DEV_SCINV, 
    MQTT_DEV_NULL2,	
	MQTT_DEV_EMU, 
	MQTT_DEV_CABINET, 
	MQTT_DEV_STH, 
	MQTT_DEV_SCPMD,
	MQTT_DEV_SCREEN,
	MQTT_DEV_ACPMD,
	MQTT_DEV_GPRS,
	MQTT_DEV_TRAKER,
}MQTT_DEV_TYPE;


typedef enum{
	MQTT_LINK_NULL,
	MQTT_LINK_OK,
	MQTT_LINK_CONNECT,
	MQTT_LINK_SUBSCRIBE,
	MQTT_LINK_PUBLISH_RUNDATA,
	MQTT_LINK_PUBLISH_FAULTDATA,
	MQTT_LINK_PUBLISH_HISTORDATA,
	MQTT_LINK_PUBLISH_PROPERTYDATA,
	MQTT_LINK_PUBLISH_FAULTWAVEDATA,
	MQTT_LINK_PUBLISH_UPGRADEDATA,
	MQTT_LINK_PUBLISH_DIAGNOSISDATA,
	MQTT_LINK_PUBLISH_SETDATA,
	MQTT_LINK_PUBLISH_SETPASSWORD,
	MQTT_LINK_PUBACK,
	MQTT_LINK_PING,
	MQTT_LINK_DISCONNECT,
	MQTT_LINK_PUBLISH_CURVE,
}MQTT_LINK_TYPE;

typedef enum{
	MQTT_PUBLICK_NULL,
	MQTT_PUBLICK_PASSWORD,
	MQTT_PUBLICK_SETPARAM,
	MQTT_PUBLICK_PCOFFLINE,
}MQTT_PUBLICK_TYPE;

typedef enum{
	MQTT_SEND_NULL,
	MQTT_SEND_SUCESS,
	MQTT_SEND_FAULT,
}MQTT_SEND_TYPE;


typedef enum{
	MQTT_PC_OFFLINE,
	MQTT_PC_ONLINE,
}MQTT_PC_STATE;

typedef struct
{
	void      *linkHandle;                     // 通信socket句柄
	uint32_t  publishtime;                     // 发布消息记时参数 超时未回复 则断开链接重新启动链接
	uint32_t  pingtime;                        // ping命令记时 如无后台数据来 则超时2分钟后发送一次ping命令 连续5次 及超时10分钟则断开链接
	uint16_t  sentcycle;                       // 发布消息时间间隔
	uint16_t  SUBmsgID;                        // 接收发布消息ID
	uint16_t  PUBmsgID;                        // 主动发布消息ID
	uint16_t  LinkPort;						   // 链接端口号
	uint16_t  rcvlen;                          // 收到数据长度
	uint16_t  sendlen;                         // 发送数据长度
	uint8_t   wirelesslink;                    // 无线通讯模式下收到发布消息的下一种状态
	uint8_t   pingnum;                         // ping命令发送计数 连续5次未接收到数据则认为链路失败 重新链接
	uint8_t   publishDevno;                    // 发布数据的设备总数
	uint8_t   publishIndex;                    // 发布数据的设备偏移
	uint8_t   publishResult;                   // 发布数据结果
	uint8_t   linkStatus;                      // 连接状态 0，无连接；1：链接成功；2：启动链接 3： 启动订阅  4： 启动发布实时数据 5：启动发布故障数据 6：启动发布历史数据 7：保持通讯阶段 ping包发送                                        
	uint8_t   linkName[64];                    // 链接域名
	uint8_t   linkIP[4];                       // 链接IP地址
	uint8_t   publickey[16];                   // AES128公钥
	uint8_t   aes128key[16];                   // AES128私钥
    uint8_t   rcvBuf[MQTT_RCV_BUF_LEN];        // 接收缓冲区
    uint8_t   sendBuf[MQTT_SEND_BUF_LEN];      // 发送缓冲区
	uint8_t   Qos;                             // MQTT发送质量
	uint8_t   DUP;                             // MQTT重发标志
	uint8_t   RETAIN;                          // MQTT保持标志
	uint8_t   linkDNS;                         // 是否支持DNS
	uint8_t   parasetresult;                   // 参数设置结果
	uint8_t   PCState;                         // PC是否在线
	uint8_t   MQTTStart;                       // MQTT首次上线
}MQTT_LINK_STR;

extern MQTT_LINK_STR MQTTLink;

extern const char *MQTT_user;
extern const char *MQTT_pass;

extern const char *MQTT_PubishTopic1;
extern const char *MQTT_PubishTopic2;
extern const char *MQTT_PubishTopic3;
extern const char *MQTT_PubishTopic4;
extern const char *MQTT_PubishTopic5;
extern const char *MQTT_PubishTopic6;
extern const char *MQTT_PubishTopic7;
extern const char *MQTT_PubishTopic8;
extern const char *MQTT_PubishTopic9;

//Topic1 对时
//Topic2 参数设置
//Topic3 密码设置
//Topic4 PC掉线
extern char MQTT_SubscribeTopic1[32];
extern char MQTT_SubscribeTopic2[32];
extern char MQTT_SubscribeTopic3[32];
extern char MQTT_SubscribeTopic4[32];
extern char MQTT_SubscribeTopic5[32];

extern char MQTT_PCSetParaID[64];

extern char MQTT_ClientID[32];
extern char MQTT_WillMessge[32];

//extern uint8_t MQTT_skey[16]; //AES128-公钥

void MQTT_Init(void);
void MQTT_Link_Start(void); 
void MQTT_Link_Close(void);
uint16_t MQTT_Encode_Length(uint16_t length);
uint16_t MQTT_Decode_Length(uint8_t *pbuf, uint16_t length);
void MQTT_pkg_encrypt(uint8_t *buf, uint16_t size, uint8_t *skey);
void MQTT_pkg_decrypt(uint8_t *buf, uint16_t size, uint8_t *skey);
uint16_t MQTT_PUBISH_DEVICEDATA(uint8_t *pbuf);
#endif

