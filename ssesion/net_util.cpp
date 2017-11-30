/*
 * net_util.cpp
 *
 *  Created on: 2017-1-19
 *      Author: root
 */
#include "includes.h"

//获取本机IP地址
//if_name -- "eth0" ...
int GetLocalIp(const char* if_name, char *ipaddr)
{
    int sock_get_ip;
    //char ipaddr[50];
    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;

    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
         //printf("socket create failse...GetLocalIp!\n");
         return -1;
    }

    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, if_name, sizeof(ifr_ip.ifr_name) - 1);
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )
    {
         return -1;
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));

    close(sock_get_ip);
    return 0;
}

//修改本机IP地址
//if_name -- "eth0" ...
int SetLocalIp(const char* if_name, const char *ipaddr)
{
    int sock_set_ip;

    struct sockaddr_in sin_set_ip;
    struct ifreq ifr_set_ip;

    bzero( &ifr_set_ip,sizeof(ifr_set_ip));

    if( ipaddr == NULL )
        return -1;

    if((sock_set_ip = socket( AF_INET, SOCK_STREAM, 0 )) < 0)
    {
        perror("socket create failse...SetLocalIp!\n");
        return -1;
    }

    memset( &sin_set_ip, 0, sizeof(sin_set_ip));
    strncpy(ifr_set_ip.ifr_name, if_name, sizeof(ifr_set_ip.ifr_name)-1);

    sin_set_ip.sin_family = AF_INET;
    sin_set_ip.sin_addr.s_addr = inet_addr(ipaddr);
    memcpy( &ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));

    if( ioctl( sock_set_ip, SIOCSIFADDR, &ifr_set_ip) < 0 )
    {
        perror( "Not setup interface\n");
        return -1;
    }

    //设置激活标志
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;

    //get the status of the device
    if( ioctl( sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip ) < 0 )
    {
         perror("SIOCSIFFLAGS");
         return -1;
    }

    close( sock_set_ip );
    return 0;
}

//获取本机MAC地址
//if_name -- "eth0" ...
int GetLocalMac(const char* if_name, char* mac_addr)
{
    int sock_mac;

    struct ifreq ifr_mac;
    //char mac_addr[30];

    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_mac == -1)
    {
        perror("create socket falise...mac\n");
        return -1;
    }

    memset(&ifr_mac,0,sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, if_name, sizeof(ifr_mac.ifr_name)-1);

    if( (ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
    {
        //printf("mac ioctl error/n");
        return -1;
    }

    sprintf(mac_addr,"%02x%02x%02x%02x%02x%02x",
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);

    //printf("local mac:%s \n",mac_addr);

    close( sock_mac );
    return 0;
}

//获取子网掩码
//if_name -- "eth0" ...
int GetLocalNetMask(const char* if_name, char* netmask_addr)
{
    int sock_netmask;
    //char netmask_addr[50];

    struct ifreq ifr_mask;
    struct sockaddr_in *net_mask;

    sock_netmask = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_netmask == -1)
    {
        perror("create socket failture...GetLocalNetMask\n");
        return -1;
    }

    memset(&ifr_mask, 0, sizeof(ifr_mask));
    strncpy(ifr_mask.ifr_name, if_name, sizeof(ifr_mask.ifr_name )-1);

    if( (ioctl( sock_netmask, SIOCGIFNETMASK, &ifr_mask ) ) < 0 )
    {
        //printf("mac ioctl error\n");
        return -1;
    }

    net_mask = ( struct sockaddr_in * )&( ifr_mask.ifr_netmask );
    strcpy( netmask_addr, inet_ntoa( net_mask -> sin_addr ) );

    //printf("local netmask:%s\n",netmask_addr);

    close( sock_netmask );
    return 0;
}

//修改子NETMASK
//if_name -- "eth0" ...
int SetLocalNetMask(const char* if_name, const char *szNetMask)
{
    int sock_netmask;

    struct ifreq ifr_mask;
    struct sockaddr_in *sin_net_mask;

    sock_netmask = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_netmask == -1)
    {
        perror("Not create network socket connect\n");
        return -1;
    }

    memset(&ifr_mask, 0, sizeof(ifr_mask));
    strncpy(ifr_mask.ifr_name, if_name, sizeof(ifr_mask.ifr_name )-1);
    sin_net_mask = (struct sockaddr_in *)&ifr_mask.ifr_addr;
    sin_net_mask -> sin_family = AF_INET;
    inet_pton(AF_INET, szNetMask, &sin_net_mask ->sin_addr);

    if(ioctl(sock_netmask, SIOCSIFNETMASK, &ifr_mask ) < 0)
    {
        //printf("sock_netmask ioctl error\n");
        return -1;
    }

    return 0;
}

//获取GateWay
int GetGateWay(char* gateway)
{
    FILE *fp;
    char buf[512];
    char cmd[128];
    //char gateway[30];
    char *tmp;

    strcpy(cmd, "ip route");
    fp = popen(cmd, "r");
    if(NULL == fp)
    {
        perror("popen error");
        return -1;
    }
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
            ++ tmp;
        if(strncmp(tmp, "default", strlen("default")) == 0)
            break;
    }
    //sscanf(buf, "%*s%*s%s", gateway);
    //printf("default gateway:%s\n", gateway);
    pclose(fp);

    return 0;
}

//设置网关
int SetGateWay(const char *szGateWay)
{
    int ret = 0;
    char cmd[128];
    char buf[64];
    GetGateWay(buf);
    const char *strGW = buf;

    strcpy(cmd, "route del default gw ");
    strcat(cmd, strGW);
    ret = system(cmd);
    if(ret < 0)
    {
        perror("route error");
        return -1;
    }
    strcpy(cmd, "route add default gw ");
    strcat(cmd, szGateWay);

    ret = system(cmd);
    if(ret < 0)
    {
        perror("route error");
        return -1;
    }

    return ret;
}
