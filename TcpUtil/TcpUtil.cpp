#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include "TcpUtil.h"

TcpUtil::TcpUtil()
{
	mFd=-1;
	memset(&sockaddr, 0x00, sizeof(sockaddr));
}

TcpUtil::~TcpUtil()
{

}

unsigned long TcpUtil::LookUpServerAddress(char *serverIP)
{
	struct hostent *host;
	unsigned long laddr = inet_addr(serverIP);
	//
	if(laddr != INADDR_NONE)
	{
		return laddr;
	}
	//
	host = gethostbyname(serverIP);
	if(host == NULL)
	{
		return INADDR_NONE;
	}
	in_addr server_addr = *(struct in_addr *)host->h_addr;
	//printf("TcpUtil:: server ip: %s\n",inet_ntoa(server_addr));
	return server_addr.s_addr;
}

int TcpUtil::CreateSocket(char *serverIP, unsigned short port, char *DeviceToBind)
{
	if(0 == serverIP || port <= 0)
	{
		//printf("TcpUtil::IP or Port err!\n");
		return -1;
	}
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	if(0 == strlen(serverIP)){
		sockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}else{
		sockaddr.sin_addr.s_addr=inet_addr(serverIP);
	}
	//域名查找IP
	//sockaddr.sin_addr.s_addr = LookUpServerAddress(serverIP);
	//if(INADDR_NONE == sockaddr.sin_addr.s_addr)
	//{
	//	return -1;
	//}
	mFd = socket(AF_INET, SOCK_STREAM, 0);
	if(mFd < 0)
	{
		//printf("TcpUtil::socket create err!\n");
		return -1;
	}

	//设置非阻塞, 多重连接
#if 1
	int sock_flag = fcntl(mFd, F_GETFL, 0);
	sock_flag = fcntl(mFd, F_SETFL, sock_flag | O_NONBLOCK);
	if(sock_flag < 0)
	{
		CloseSocket();
		//printf("TcpUtil::fcntl O_NONBLOCK err!\n");
		return -1;
	}
#else
	int imode = 1;
	ioctl(sock_fd, FIONBIO, &imode);
#endif

/*
	int ret;
	fd_set read_set,write_set;
	struct timeval timeout;

	//接收超时时间, 阻塞模式下有意义
	unsigned int msec_timeout = 100; //100ms
	FD_ZERO(&read_set);
	FD_SET(mFd,&read_set);
	timeout.tv_sec = msec_timeout / 1000;
	timeout.tv_usec = (msec_timeout % 1000) * 1000;
	ret = select(mFd + 1, &read_set, NULL, NULL, &timeout);
	if(ret <= 0)
	{
		return -1;
	}

	//发送超时时间, 阻塞模式下有意义
	FD_ZERO(&write_set);
	FD_SET(mFd, &write_set);
	timeout.tv_sec = msec_timeout / 1000;
	timeout.tv_usec = (msec_timeout % 1000) * 1000;
	ret = select(mFd + 1, NULL, &write_set, NULL, &timeout);
	if(ret <= 0)
	{
		return -1;
	}

	//设置非阻塞, 多重连接
	int sock_flag = fcntl(mFd, F_GETFL);
	sock_flag = fcntl(mFd, F_SETFL, sock_flag | O_NONBLOCK);
	if(sock_flag < 0)
	{
		CloseSocket();
		return -1;
	}
	//设置多重绑定
	//bool bSet=true;
	//setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bSet, sizeof(bool));

	//绑定到指定网卡
	BindToNetDevice(mFd, DeviceToBind);

	//设置保持在线
	int keepalive=1;
	if(setsockopt(mFd, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keepalive,sizeof(keepalive))<0) return -1;
*/
	return 0;
}

int TcpUtil::Connect2()
{
	int ret = 0;
	ret = connect(mFd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
	return ret;
}

int TcpUtil::Connect()
{
	int ret = 0;

	if(connect(mFd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
	    if(errno == EINPROGRESS) //连接正在进行中 115
	    {
	    	int times = 0;
	    	while (times++ < 5)
			{
	    		fd_set rfds, wfds;
	    		struct timeval tv;
	    		//printf("errno = %d\n", errno);
				FD_ZERO(&rfds);
				FD_ZERO(&wfds);
				FD_SET(mFd, &rfds);
				FD_SET(mFd, &wfds);

				// set select() time out
				tv.tv_sec = 2; //2s超时
				tv.tv_usec = 0;
				int selres = select(mFd + 1, &rfds, &wfds, NULL, &tv);
	            switch (selres)
	            {
					case -1:
						//printf("select error\n");
						ret = -1;
						break;
					case 0:
						//printf("select time out\n");
						ret = -1;
						break;
					default:
						if(FD_ISSET(mFd, &rfds) || FD_ISSET(mFd, &wfds))
						{
							connect(mFd, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
							int err = errno;
							if  (err == EISCONN)
							{
								//printf("connect finished.\n");
								ret = 0;
								return 0;
							}
							else
							{
								//printf("connect failed. errno = %d\n", errno);
								//printf("FD_ISSET(sock_fd, &rfds): %d\n FD_ISSET(sock_fd, &wfds): %d\n", FD_ISSET(mFd, &rfds) , FD_ISSET(mFd, &wfds));
								ret = errno;
							}
						}
				}
				if (-1 != selres && (ret != 0)){
					//printf("check connect result again... %d\n", times);
					continue;
				}else{
					break;
				}
			}
	    	if(times >=5) return -1;
	    	if(ret == 0){
	    		return 0;
	    	}else{
	    		return -1;
	    	}
	    }
	    else
	    {
	        //printf("connect to host failed.\n");
	        ret = errno;
	        return -1;
	    }
	}
	else
	{
		return 0; //连接成功
	}

	return 0;
}

void TcpUtil::CloseSocket()
{
	if(mFd != -1)
	{
		close(mFd);
		mFd = -1;
	}
}

int TcpUtil::ReceiveSocket(char *buffer, unsigned int len, unsigned int msec_timeout)
{
	int ret;
	fd_set read_set;
	struct timeval timeout;
	memset(buffer, 0x00, len);
	if(mFd == -1)
	{
		//printf("TcpUtil::FD err!\n");
		return -127;
	}
	if(msec_timeout != 0)
	{
		FD_ZERO(&read_set);
		FD_SET(mFd,&read_set);
		timeout.tv_sec = msec_timeout / 1000;
		timeout.tv_usec = (msec_timeout % 1000) * 1000;
		ret = select(mFd + 1, &read_set, NULL, NULL, &timeout);
		if(ret <= 0)
		{
			//printf("TcpUtil::ReceiveSocket sock(%d) ret(%d) msec_timeout(%d)\n",mFd,ret,msec_timeout);
			return -128;
		}
	}
	return recv(mFd, buffer, len, 0);
}

int TcpUtil::SendSocket(const char *buffer, unsigned int len, unsigned int msec_timeout)
{
	int ret;
	fd_set write_set;
	struct timeval timeout;

	if(mFd == -1)
	{
		//printf("TcpUtil::FD err!\n");
		return -127;
	}
	if(msec_timeout != 0)
	{
		FD_ZERO(&write_set);
		FD_SET(mFd, &write_set);
		timeout.tv_sec = msec_timeout / 1000;
		timeout.tv_usec = (msec_timeout % 1000) * 1000;
		ret = select(mFd + 1, NULL, &write_set, NULL, &timeout);
		if(ret <= 0)
		{
			//printf("TcpUtil::SendSocket ret(%d) msec_timeout(%d) timeout(%d %d)\n",ret,msec_timeout,(int)timeout.tv_sec,(int)timeout.tv_usec);
			return -128;
		}
	}
	ret =  send(mFd, buffer, len, 0);
	return ret;
}



int TcpUtil::ReConnect()
{
	CloseSocket();
	mFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(mFd < 0) return -1;
	if(connect(mFd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		CloseSocket();
		//printf("TcpUtil::RE-connect s err!\n");
		return -1;
	}
	return 0;
}

int TcpUtil::GetConnectStatus()
{
    struct tcp_info info;
    int len = sizeof(info);

    getsockopt(mFd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*) & len);
    if((info.tcpi_state == TCP_ESTABLISHED)) {
        return 1;
    } else {
        return 0;
    }
}

int TcpUtil::GetSocket()
{
	return mFd;
}

//绑定到网卡: 如eth0
int TcpUtil::BindToNetDevice(int sock, char *DeviceName)
{
	if(DeviceName != NULL)
	{
		struct ifreq if_ppp;
		memset(&if_ppp, 0, sizeof(if_ppp));
		strncpy(if_ppp.ifr_name,DeviceName,IFNAMSIZ);
		if(setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&if_ppp, sizeof(if_ppp)) != 0)
		{
			//printf("socket:%d bind to netdevice failed!\r\n",mFd);
			return -1;
		}
	}
	return -1;
}

