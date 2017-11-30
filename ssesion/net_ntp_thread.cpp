/*
 * net_ntp_thread.cpp
 *
 *  Created on: 2017-1-22
 *      Author: root
 */

#include "includes.h"
#include "net_ntp_thread.h"
#include <netinet/in.h>


NetNtpThread::NetNtpThread()
{
	m_bStart = true;
    Time_cur=0u;
    Time_last=0u;
    cnt=0;
    retry=2;
    sprintf(srvIP, "%s", NVR_NTP_SERVER_IP);
    srvPort=123;
    memset(&g_ntpPacketStr, 0x00, sizeof(NVR_NTP_PACKET_STR));

	pMutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);  //互斥锁
	pthread_mutex_init(pMutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

NetNtpThread::~NetNtpThread()
{
	pthread_mutex_destroy(pMutex);
	if(pMutex!=NULL)free(pMutex);
	pMutex = NULL;
}

inline int NetNtpThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	cur = (int64_t)tv.tv_sec*1000+(int64_t)tv.tv_usec/1000; //毫秒

	if(last==0 || cur<last){last=cur;return 0;}
	if(cur > last && (cur-last) >= ms){ //定时时间到
		last = cur;
		return 1;
	}else{
		return 0;
	}

	return 0;
}

/*功能：初始化NTP包*/
void NetNtpThread::ntp_packet_init(char *ntpPacketBuf)
{
	long tmp_wrd;
	time_t timer;

	memset((char*)ntpPacketBuf, 0, sizeof(NVR_NTP_PACKET_STR));

	/*设置16字节的包头*/
	tmp_wrd = htonl((NVR_NTP_LI << 30)|(NVR_NTP_VN << 27)|(NVR_NTP_MODE << 24)|(NVR_NTP_STRATUM << 16)|(NVR_NTP_POLL << 8)|(NVR_NTP_PREC & 0xff));
	memcpy(ntpPacketBuf, &tmp_wrd, sizeof(tmp_wrd));

	/*设置Root Delay、Root Dispersion*/
	tmp_wrd = htonl(1<<16);
	memcpy(&ntpPacketBuf[4], &tmp_wrd, sizeof(tmp_wrd));
	memcpy(&ntpPacketBuf[8], &tmp_wrd, sizeof(tmp_wrd));

	/*设置Timestamp部分*/
	/*获取当前时间秒数*/
	time(&timer);

	/*设置Transmit Timestamp coarse*/
	tmp_wrd = htonl(NVR_NTP_JAN_1970 + (long)timer);
	memcpy(&ntpPacketBuf[40], &tmp_wrd, sizeof(tmp_wrd));

	/*设置Transmit Timestamp fine*/
	tmp_wrd = htonl((long)NVR_NTPFRAC(timer));
	memcpy(&ntpPacketBuf[44], &tmp_wrd, sizeof(tmp_wrd));
}

/*功能：获取NTP时间,返回值：返回1成功，0失败*/
int NetNtpThread::get_ntp_time(char *nvr_ntp_server, int port)
{
	int sockfd=-1, recv_size = 0;
	//struct addrinfo hints, *res = NULL;//res用于指向返回addrinfo链表
	struct sockaddr_in server_addr;
	socklen_t server_addr_len=0;

	fd_set readfds;
	struct timeval time_out;

	//memset(&hints, 0, sizeof(hints));
	//hints.ai_family   = AF_UNSPEC;	//协议无关
	//hints.ai_socktype = SOCK_DGRAM;	//数据报
	//hints.ai_protocol = IPPROTO_UDP;//为17:UDP

	//如果是域名需要执行下面的来获取ip
	//调用getaddrinfo()函数，获取地址信息res
	/*int getaddrinfo( const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result );
	返回0成功，非零失败*/
	//if(0 != getaddrinfo(nvr_ntp_server, srvPort, &hints, &res))
	//{
	//	printf("NTP: struct getaddrinfo err\n");
	//	return 0;
	//}

	/*创建套接字*/
	/*int socket(int domain, int type, int protocol);
	用于创建套接字，失败返回-1。
	参数domain：一般为PF_INET 或 AF_INET 表示IPv4网络协议；
	参数type:		TCP通信为 SOCK_STREAM,UDP通信为 SOCK_DGRAM;
	参数protocol: 一般为0；
	*/
	//sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0 )
	{
		//printf("NTP: create socket err\n");
		return 0;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(nvr_ntp_server); //"127.0.0.1"
	server_addr.sin_port=htons(port);
	server_addr_len=sizeof(server_addr);

	/*初始化NTP数据包格式*/
	ntp_packet_init(g_ntpPacketStr.ntpPacketBuf);
	if (sizeof(g_ntpPacketStr.ntpPacketBuf) != NVR_NTP_PCK_LEN)
	{
		//printf("NTP: packet length err\n");
		close(sockfd);
		return 0;
	}
	/* ssize_t sendto(int s,const void *buf,size_t len,int flags, const struct sockaddr *to, socklen_t tolen);
	   用于UDP通信发送，失败返回-1，成功返回发送字节数。
	   参数s:		套接字描述符；
	   参数buf:	发送缓冲区；
	   参数len:	发送缓冲区大小；
	   参数flags:一般0；
	   参数to:指定欲传送的网络地址；
	   参数tolen:传送的网络地址的长度：sizeof(网络地址)
	*/
	/*客户端给服务器端发送NTP协议数据包*/
	//if (sendto(sockfd, g_ntpPacketStr.ntpPacketBuf, sizeof(g_ntpPacketStr.ntpPacketBuf), 0, res->ai_addr, res->ai_addrlen) < 0)
	if (sendto(sockfd, g_ntpPacketStr.ntpPacketBuf, sizeof(g_ntpPacketStr.ntpPacketBuf), 0, (struct sockaddr*)&server_addr, server_addr_len) < 0)
	{
		//printf("sendto NTP packet err\n");
		close(sockfd);
		return 0;
	}

	/*
	   int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); 后两个参数可用NULL代替
	*/
	/*
	   void FD_CLR(int fd, fd_set *set);  //清除某一文件描述符
	   int FD_ISSET(int fd, fd_set *set); //测试文件描述符是否准备就绪（可写还是可读）
	   void FD_SET(int fd, fd_set *set);  //将一个文件描述符加入文件描述富集
	   void FD_ZERO(fd_set *set);         //清空文件描述符集
	*/

	/*调用select()函数，并设定超时时间为10s*/
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	time_out.tv_sec=2;
	time_out.tv_usec=0;
	if (select(sockfd + 1, &readfds, NULL, NULL, &time_out) > 0)
	{
		/*ssize_t recvfrom(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
		  用于UDP通信接收，失败返回-1，成功返回接收字节数。
		  若是有设置接收超时限制的话,也会返回-1，errno为EAGAIN(11)

		  参数s:		套接字描述符；
		  参数buf:	接收缓冲区；
		  参数len:	接收缓冲区大小；
		  参数flags:一般0；
		  参数from:存放自发来连接的网络地址；
		  参数fromlen:连接的网络地址的长度：
		*/
		/*接收服务器端的信息*/
		//if ((recv_size = recvfrom(sockfd, g_ntpPacketStr.ntpPacketBuf,sizeof(g_ntpPacketStr.ntpPacketBuf), 0, res->ai_addr, &res->ai_addrlen)) < 0)
		if ((recv_size = recvfrom(sockfd, g_ntpPacketStr.ntpPacketBuf,sizeof(g_ntpPacketStr.ntpPacketBuf), 0, (struct sockaddr*)&server_addr, &server_addr_len)) < 0)
		{
			//printf("recvfrom NTP packet");
			close(sockfd);
			return 0;
		}

		if (recv_size < NVR_NTP_PCK_LEN)
		{
			//printf("recvfrom NTP packet length");
			close(sockfd);
			return 0;
		}

		/* 设置接收NTP包的数据结构 */
		g_ntpPacketStr.ntpPacket.leap_ver_mode 				= ntohl(g_ntpPacketStr.ntpPacketBuf[0]);
		g_ntpPacketStr.ntpPacket.startum 					= ntohl(g_ntpPacketStr.ntpPacketBuf[1]);
		g_ntpPacketStr.ntpPacket.poll 						= ntohl(g_ntpPacketStr.ntpPacketBuf[2]);
		g_ntpPacketStr.ntpPacket.precision 					= ntohl(g_ntpPacketStr.ntpPacketBuf[3]);
		g_ntpPacketStr.ntpPacket.root_delay 				= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[4]));
		g_ntpPacketStr.ntpPacket.root_dispersion 			= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[8]));
		g_ntpPacketStr.ntpPacket.reference_identifier 		= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[12]));
		g_ntpPacketStr.ntpPacket.reference_timestamp.coarse = ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[16]));
		g_ntpPacketStr.ntpPacket.reference_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[20]));
		g_ntpPacketStr.ntpPacket.originage_timestamp.coarse = ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[24]));
		g_ntpPacketStr.ntpPacket.originage_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[28]));
		g_ntpPacketStr.ntpPacket.receive_timestamp.coarse 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[32]));
		g_ntpPacketStr.ntpPacket.receive_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[36]));
		g_ntpPacketStr.ntpPacket.transmit_timestamp.coarse 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[40]));
		g_ntpPacketStr.ntpPacket.transmit_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[44]));

		close(sockfd);
		return 1;
	} /* end of if select */

	close(sockfd);
	return 0;
}

/* 功能：修改本地时间,返回值：1则成功,0失败*/
int NetNtpThread::set_local_time(void)
{
	struct timeval tv;

	/*获取NTP时间包*/
#ifdef AUTO
	if(!nvr_get_ntp_time(NVR_NTP_SERVER_IP, NVR_NTP_PORT_STR))
#else
	if(!get_ntp_time(srvIP, srvPort))
#endif
	{
		//printf("get ntp packet failed\n");
		return 0;
	}

	/*更新本地时间*/
	tv.tv_sec  = g_ntpPacketStr.ntpPacket.transmit_timestamp.coarse - NVR_NTP_JAN_1970;
	tv.tv_usec = NVR_NTPUSEC(g_ntpPacketStr.ntpPacket.transmit_timestamp.fine);
	if (settimeofday(&tv, NULL) < 0){
		return 0;
	}else{
		FILE * fp;
		if ((fp = popen("hwclock -w -u", "w") ) == NULL){   //写入硬件
			//printf("POPEN error: %s\n", strerror(errno));
			mutex_for_timer.lock();
			set_sys_time();
			get_sys_time();
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			{
				Send_Data_To_PCS_0x10_01[i]=true;
			}
			for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
			{
				Send_Data_To_PV_0x10_01[i]=true;
			}
			delay10s=true;
			Clear_Data = true;                              //判断是否是新的一天，是否要清除前一天的数据
			mutex_for_timer.unlock();
			return 0;
		}else{
			pclose(fp);
			return 1;
		}
	}
	return 0;
}

int NetNtpThread::check_ntp_now()
{
	pthread_mutex_lock(pMutex);
	retry = 2;
	pthread_mutex_unlock(pMutex);
	return 0;
}

int NetNtpThread::SetServer(char *IP, uint16_t Port)
{
	memset(srvIP, 0x00, sizeof(srvIP));
	sprintf(srvIP, "%s", IP);
	srvPort = Port;
	//printf("NTP: ip(%s) port(%d)\n", srvIP, srvPort);
	return 0;
}

void NetNtpThread::run()
{
	if(System_Info.NTP.enable == 0xAA) retry = 2;
	while(1)
	{
		if(System_Info.NTP.enable == 0xAA)
		{
			if(CheckTimeout(10000, Time_cur, Time_last))
			{
				cnt++;
				if(cnt == 360) retry = 2;  //1h校准

				if(retry > 0)
				{
					/*调整本地时间*/
					if (set_local_time() == 1){
						pthread_mutex_lock(pMutex);
						retry = 0;
						pthread_mutex_unlock(pMutex);
						cnt = 0;
						printf("+++ NTP Check Time Success! +++\n");
					}else{
						pthread_mutex_lock(pMutex);
						retry--;
						pthread_mutex_unlock(pMutex);
						if(retry == 0) cnt = 0;
						printf("+++ NTP Check Time Failed! +++\n");
					}
				}
			}
			sleep(1);
		}
		else
		{
			cnt = 0;
			retry = 0;
			sleep(10);
		}
	}
}
