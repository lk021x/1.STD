/*
 * UdpUtil.cpp
 *
 *  Created on: 2017-1-21
 *      Author: root
 */

#include "includes.h"
#include "UdpUtil.h"

#define IFNAME "eth0"
#define MCAST_PORT 10000

struct sockaddr_in broadcast_addr; //广播地址
int udp_sock;

int udp_init(void)
{
	int ret = -1;
	int sock = -1;
	int j = -1;
	int so_broadcast = 1;
	struct ifreq *ifr;
	struct ifconf ifc;
	char buffer[1024];

	//建立数据报套接字
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
	{
		//printf("UdpUtil: create socket failed\n");
		return -1;
	}

	// 获取所有套接字接口
	ifc.ifc_len = sizeof(buffer);
	ifc.ifc_buf = buffer;
	if(ioctl(sock, SIOCGIFCONF, (char *) &ifc) < 0)
	{
		//printf("UdpUtil: ioctl SIOCGIFCONF error\n");
		close(sock);
		return -1;
	}
	ifr = ifc.ifc_req;
	for(j = ifc.ifc_len / sizeof(struct ifreq); --j >= 0; ifr++)
	{
		if (!strcmp(ifr->ifr_name, "eth0")){
			if (ioctl(sock, SIOCGIFFLAGS, (char *) ifr) < 0){
				//printf("UdpUtil: ioctl-get flag failed\n");
			}
			break;
		}
	}

	//将使用的网络接口名字复制到ifr.ifr_name中，由于不同的网卡接口的广播地址是不一样的，因此指定网卡接口
	//strncpy(ifr.ifr_name, IFNAME, strlen(IFNAME));
	//发送命令，获得网络接口的广播地址
	if(ioctl(sock, SIOCGIFBRDADDR, ifr) == -1)
	{
		//printf("UdpUtil: ioctl SIOCGIFBRDADDR error\n");
		close(sock);
		return -1;
	}
	//将获得的广播地址复制到broadcast_addr
	memcpy(&broadcast_addr, (char *)&ifr->ifr_broadaddr, sizeof(struct sockaddr_in));
	//设置广播端口号
	//printf("\nUdpUtil: Broadcast-IP: %s\n", inet_ntoa(broadcast_addr.sin_addr));
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons(MCAST_PORT);

	//默认的套接字描述符sock是不支持广播，必须设置套接字描述符以支持广播
	ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
	if(ret < 0)
	{
		//printf("UdpUtil: set SO_BROADCAST error\n");
		close(sock);
		return -1;
	}
	return sock;
}

int send_udp(char *buf, int len)
{
	int slen;
	slen = sendto(udp_sock, buf, len, 0, (struct sockaddr*) &broadcast_addr, sizeof(broadcast_addr));
	if (slen < 0)
	{
		return -1;
	}
	return slen;
}
