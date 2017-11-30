#ifndef _TCPUTIL_H
#define _TCPUTIL_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include "errno.h"

class TcpUtil {
	public:
		TcpUtil();
		virtual ~TcpUtil();
		
		//创建SOCKET并连接服务器，非阻塞方式
		//参数一：服务器IP或者主机名(网址)
		//参数二：服务器端口
		//返回：0 表示成功；-1 表示失败
		int CreateSocket(char *ServerIP, unsigned short ServerPort, char *DeviceToBind = NULL);

		//连接服务器
		//返回：0 表示成功；-1 表示失败
		int Connect();

		int Connect2();

		//接收数据
		//参数一：接收数据的缓冲区
		//参数二：接收数据的缓冲区大小
		//参数三：设置超时时间
		//返回：0 正常断开连接; -1 表示异常; 否则表示收到的数据长度
		int ReceiveSocket(char *buffer, unsigned int len, unsigned int msec_timeout = 2000);
		
		//发送数据
		//参数一：要发送数据缓冲区
		//参数二：要发送数据缓冲区长度
		//参数三：设置接收超时时间
		//返回：表示发送出去的字节数，-1表示失败
        int SendSocket(const char *buffer, unsigned int len, unsigned int msec_timeout = 0);

		//关闭连接
		void CloseSocket();

		//重新连接
		int ReConnect();

        //检查连接状态
        //返回1在线
        //返回0掉线
        int GetConnectStatus();

        //返回socket
        int GetSocket();

		struct sockaddr_in sockaddr;

		int mFd;

	private:
		
		//寻找主机的ip地址
		//参数一：ip地址或者主机名
		//返回：0xffffffff表示无效的ip地址，否则有效
		unsigned long LookUpServerAddress(char *serverIP);

		//将socket绑定到指定的网卡
		//参数一：所需要绑定的socket
		//参数二：指定绑定的设备名（网卡在系统中的名称）
		int BindToNetDevice(int sock , char *DeviceName);
};

#endif /*TCPUTIL_H_ */
