/*
 * send_thread.h
 *
 *  Created on: 2017-1-22
 *      Author: root
 */

#ifndef _NET_NTP_THREAD_H_
#define _NET_NTP_THREAD_H_

#include "MThread.h"

/*NTP服务器\端口号*/
#define NVR_NTP_SERVER_IP       (char*)"cn.pool.ntp.org"	/*国家授时中心*/
#define NVR_NTP_PORT_STR        (char*)"123"          		/*NTP专用端口号字符串*/

/*NTP包头数据*/
#define NVR_NTP_PCK_LEN				48
#define NVR_NTP_LI 					0
#define NVR_NTP_VN 					3
#define NVR_NTP_MODE 				3
#define NVR_NTP_STRATUM 			0
#define NVR_NTP_POLL 				4
#define NVR_NTP_PREC 				-6

#define NVR_NTP_JAN_1970 	  	0x83aa7e80/* 1900年～1970年之间的时间秒数 */
#define NVR_NTPFRAC(x)			(4294 * (x) + ((1981 * (x)) >> 11))/*NTPFRAC宏是用于计算x*4294.967296的非精确写法，这样就避免了浮点计算*/
#define NVR_NTPUSEC(x)			(((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))/*USEC是上面过程的反过程，表示x/4294.967296*/

/*NTP时间格式*/
typedef struct _NVR_NTP_TIME
{
	unsigned int coarse;	//1900年1月1日0时0分以来的秒数
	unsigned int fine;		//微秒的4294.967296(=2^32/10^6)倍
} NVR_NTP_TIME;

/*NTP数据格式包大小为48字节*/
typedef struct _NVR_NTP_PACKET
{
	unsigned char leap_ver_mode;/*分为三段:前2位为闰秒指示，一般填0;
								  中间3位为NTP协议版本号，为3;
								  最后3位为通信模式。为3，表示是client.*/

	unsigned char startum;		//NTP服务器阶级，0表示不指定，1表示校准用原子钟。为0。
	char poll;					//表示测试间隔为2^poll_intervals秒,为4。
	char precision;				//表示本机时钟精度为2^local_precision秒。local_precision通常为负数，为-6。

	int root_delay;				//可正可负。2^16表示一秒。具体含义参见RFC1305。4~7
	int root_dispersion;		//只可为正。2^16表示一秒。具体含义参见RFC1305。8~11
	int reference_identifier;	//具体含义参见RFC1305。一般填0。12~15

	/*本地时间*/
	NVR_NTP_TIME reference_timestamp;//具体含义参见RFC1305。一般填0。16~19:20~23
	NVR_NTP_TIME originage_timestamp;//上次发出时刻 24~27:28~31
	NVR_NTP_TIME receive_timestamp;	 //接收时刻 32~35:36~39
	NVR_NTP_TIME transmit_timestamp; //发出时刻 40~43:44~47

}NVR_NTP_PACKET;

typedef struct _NVR_NTP_PACKET_STR
{
	NVR_NTP_PACKET ntpPacket;			//NTP数据包
	char ntpPacketBuf[NVR_NTP_PCK_LEN];	//NTP数据包缓冲区
}NVR_NTP_PACKET_STR;

class NetNtpThread : public MThread
{
public:
	NetNtpThread();
	virtual ~NetNtpThread();

	int SetServer(char *IP, uint16_t Port);
    int check_ntp_now(); /*立即执行ntp对时*/
private:
	bool m_bStart;
	char srvIP[32];
	int srvPort;
    int64_t Time_cur,Time_last;
    uint16_t cnt;
    uint16_t retry;
    pthread_mutex_t *pMutex;

    NVR_NTP_PACKET_STR g_ntpPacketStr;

    void ntp_packet_init(char *ntpPacketBuf); /*功能：初始化NTP包*/
    int get_ntp_time(char *nvr_ntp_server, int port);  /*功能：获取NTP时间,返回值：返回1成功，0失败*/
    int set_local_time();/* 功能：修改本地时间,返回值：1则成功,0失败*/
    inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);
	void run();
};


#endif /* _NET_NTP_THREAD_H_ */
