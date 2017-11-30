/*
 * net_event_thread.cpp
 *
 *  Created on: 2017-1-21
 *      Author: root
 */

#include "includes.h"
#include "event.h"
#include "net_event_thread.h"
#include "com_io_monitor.h"

/*****************************************************************************
 libevent 与 libev 库不能同时使用, 也不能都加载，使用libevent需要把libev库去掉
 *****************************************************************************/

#define Http_Server_Port 	80
#define MEM_SIZE			2048
#define BACKLOG				100

struct event_base* base;
struct sock_ev {
	int port;
	struct event* read_ev;
	char client_ip[32];
	S_ModeBusBody_T *modebus_body;
	S_IEC104Body_T *iec104_body;
    //struct event* write_ev;
    //char* buffer;
};

int listen_sock[3];   //服务器套结字
uint16_t failed_accept_times = 0;  //
struct event *ev_accept[3];		//接受连接请求的事件对象
int server_init(void);
int server_close(void);
void Data_Parse(char *data_buf, char *user_ip, http_state *hs);

NetEventThread::NetEventThread()
{
	m_bStart = true;
	for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++){ g_iec104_ss[i] = NULL; }
	for(int i=0; i<3; i++) { listen_sock[i]=-1;}
}

NetEventThread::~NetEventThread()
{
	server_close();
}

void NetEventThread::stop_web()
{
	m_bStart = false;
}

void NetEventThread::run()
{
	Init_User();
	while(1)
	{
		if(server_init() < 0){
			printf("MASTER: wait 3s restart again!\n");
			server_close();
			sleep(3); // 3s
		}
	}
}

//从modebus用户登陆表中删除已断开的客户链接
void modebus_del_client(S_ModeBusBody_T *ss)
{
	if(ss == NULL) return;
	for(uint8_t i=0;i<TCP_MODBUS_LINK_NUM;i++)
	{
		if(g_modbus_ss[i] == ss)   //从登记表中找到对应的客户链接
		{
			g_modbus_ss[i]=NULL;
		}
	}
}

//增加客户链接到modebus登陆表
int modebus_add_client(struct sock_ev* ev)
{
	bool Run_OK = false;
	uint8_t i=0;

	for(i=0;i<TCP_MODBUS_LINK_NUM;i++){
		if(g_modbus_ss[i] == NULL){
			Run_OK = true;  //找到空位，可以登陆
			break;
		}
	}
	if(i==TCP_MODBUS_LINK_NUM) return -1;
	if(Run_OK == false) return -1;

	for(i=0;i<TCP_MODBUS_LINK_NUM;i++){
		if(g_modbus_ss[i] == NULL){
			ev->modebus_body = (S_ModeBusBody_T*)malloc(sizeof(S_ModeBusBody_T));
			if(ev->modebus_body != NULL){
				g_modbus_ss[i] = ev->modebus_body;
				System_Info.EMS_MONITOR_Modbus_TCP_Com_Count[i]=SIGNAL_FULL;
				return 0;
			}else{
				return -1;
			}
		}
	}
	return -1;
}

//从iec104用户登陆表中删除已断开的客户链接
void iec104_del_client(S_IEC104Body_T *ss)
{
	if(ss == NULL) return;
	for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++)
	{
		if(g_iec104_ss[i] == ss)   //从登记表中找到对应的客户链接
		{
			g_iec104_ss[i]=NULL;
		}
	}
}

//增加客户链接到iec104登陆表
int iec104_add_client(struct sock_ev* ev)
{
	bool Run_OK = false;
	uint8_t i=0;

	for(i=0;i<TCP_IEC104_LINK_NUM;i++){
		if(g_iec104_ss[i] == NULL){
			Run_OK = true;  //找到空位，可以登陆
			break;
		}
	}
	if(i==TCP_IEC104_LINK_NUM) return -1;
	if(Run_OK == false) return -1;

	for(i=0;i<TCP_IEC104_LINK_NUM;i++){
		if(g_iec104_ss[i] == NULL){
			ev->iec104_body = (S_IEC104Body_T*)malloc(sizeof(S_IEC104Body_T));
			if(ev->iec104_body != NULL){
				g_iec104_ss[i] = ev->iec104_body;
				return 0;
			}else{
				return -1;
			}
		}
	}
	return -1;
}

int server_close(void)
{
	int ret = -1;
	struct timeval tv;

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	for(int i=0; i<3; i++){
		if(listen_sock[i]!=-1)close(listen_sock[i]);
		if(ev_accept[i]!=NULL)event_del(ev_accept[i]);
	}
	if(base!=NULL){
		//event_base_loopbreak(base);  //立即退出
		while(ret == -1){
			ret = event_base_loopexit(base, &tv); //等待当前事件完成后退出
		}
	}
	return 0;
}

//不论在什么平台编写网络程序，都应该使用NONBLOCK将一个socket设置成非阻塞模式。
//这样可以保证你的程序至少不会在recv/send/accept/connect这些操作上发生block从而将整个网络服务都停下来
int SetNonblock(int fd)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL)) < 0) { //用来操作文件描述符的一些特性
        return flags;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        return -1;
    }

    return 0;
}

void release_sock_event(struct sock_ev* ev)
{
    event_del(ev->read_ev);
    if(ev->read_ev!=NULL)free(ev->read_ev);
    modebus_del_client(ev->modebus_body);
	if(ev->modebus_body!=NULL)free(ev->modebus_body);
    iec104_del_client(ev->iec104_body);
	if(ev->iec104_body!=NULL)free(ev->iec104_body);
    //if(ev->write_ev!=NULL)free(ev->write_ev);
    //if(ev->buffer!=NULL)free(ev->buffer);
    if(ev!=NULL)free(ev);
}

//这个函数当客户端的socket可读时由libevent调用
void on_read(int client_sock, short event, void *arg)
{
	struct sock_ev *ev = (struct sock_ev *)arg;
    char buf[MEM_SIZE];
    uint8_t buffer[256];
    int len, slen;

    //会把参数fd 所指的文件传送count个字节到buf指针所指的内存中
    //len = read(sock, buf, MEM_SIZE);
    len = recv(client_sock, buf, MEM_SIZE, 0);
    if (len <= 0) {
        /* 客户端断开连接，在这里移除读事件并且释放客户数据结构 */
    	printf("MASTER: len < 0 client %d disconnected\n", client_sock);
    	release_sock_event(ev);
        close(client_sock);
        return;
    }

    //以端口来区分服务器
    if(ev->port == System_Info.Modbus_Port){
    	if(len > 6){
    		mutex_for_timer.lock();
			slen = EMS_Receive_Data_From_Monitor_By_NET_Modbus((uint8_t*)buf, len, buffer);
			mutex_for_timer.unlock();
			if(slen > 0) send(client_sock, buffer, slen, 0);
			//for(uint8_t i=0;i<TCP_MODBUS_LINK_NUM;i++)
			//{
			//	if(g_modbus_ss[i]->sock == client_sock)
			//	{
			//		System_Info.EMS_MONITOR_Modbus_TCP_Com_Count[i] = SIGNAL_FULL;
			//	}
			//}
    	}
    }else if(ev->port == Http_Server_Port){
    	http_state hs;
    	Data_Parse(buf, ev->client_ip, &hs);
    	send(client_sock, hs.file, hs.left, 0);//发送请求资源
    	release_sock_event(ev);
        close(client_sock);
    }else if(ev->port == System_Info.IEC104_Port){
        if(len >= IEC_104_MIN_LENGTH + IEC_104_HEAD_LENGTH){
        	mutex_for_timer.lock();
            IO_Process_Data_From_PC_By_NET_IEC104((uint8_t *)buf, len, ev->iec104_body); // 接收数据处理
            mutex_for_timer.unlock();
			//for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++)
			//{
			//	if(g_iec104_ss[i]->sock == client_sock)
			//	{
			//		System_Info.EMS_MONITOR_IEC104_TCP_Com_Count[i] = SIGNAL_FULL;
			//	}
			//}
        }
    }
    usleep(2);
}

/*
   当有一个连接请求准备被接受时，这个函数将被libevent调用并传递给三个变量:
   int fd:触发事件的文件描述符.
   short event:触发事件的类型EV_TIMEOUT,EV_SIGNAL, EV_READ, or EV_WRITE.
   void* :由arg参数指定的变量.
   注意：发生错误时不要关闭srv_sock
*/
void on_accept(int srv_sock, short event, void *arg)
{
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct sock_ev* ev = NULL;

    //接受新连接
    client_sock = accept(srv_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock == -1) {
    	writeLog("MASTER: %d can not accept client connection\n", srv_sock);
    	failed_accept_times++;
    	return;
    }else{
    	failed_accept_times = 0;
    }

    ev = (struct sock_ev*)malloc(sizeof(struct sock_ev));
    if(ev == NULL){
    	close(client_sock);
    	return;
    }
    ev->read_ev = (struct event*)malloc(sizeof(struct event));
    if(ev->read_ev == NULL){
    	free(ev);
    	close(client_sock);
    	return;
    }

	ev->modebus_body = NULL;
	ev->iec104_body = NULL;
    if(srv_sock == listen_sock[0]){
    	if(modebus_add_client(ev)!=-1){
    		ev->modebus_body->sock = client_sock;
    		ev->port = System_Info.Modbus_Port;
    	}else{
    	    if(ev->read_ev!=NULL)free(ev->read_ev);
    	    if(ev!=NULL)free(ev);
    	    close(client_sock);
        	return;
    	}
    	ev->port = System_Info.Modbus_Port;
    	//printf("modbustcp accept ip: %s\n", inet_ntoa(client_addr.sin_addr));
    }else if(srv_sock == listen_sock[1]){
    	if(iec104_add_client(ev)!=-1){
    		IEC104Body_Reset(ev->iec104_body);
    		ev->iec104_body->sock = client_sock;
    		ev->port = System_Info.IEC104_Port;
    	}else{
    	    if(ev->read_ev!=NULL)free(ev->read_ev);
    	    if(ev!=NULL)free(ev);
    	    close(client_sock);
        	return;
    	}
    	//printf("iec104 accept ip: %s\n", inet_ntoa(client_addr.sin_addr));
    }else if(srv_sock == listen_sock[2]){
    	ev->port = Http_Server_Port;
    	//printf("web accept ip: %d.%d.%d.%d\n", ((uint32_t)client_addr.sin_addr.s_addr)&0xff,(((uint32_t)client_addr.sin_addr.s_addr)>>8)&0xff,
    		//	(((uint32_t)client_addr.sin_addr.s_addr)>>16)&0xff,(((uint32_t)client_addr.sin_addr.s_addr)>>24)&0xff);
    	if(((((uint32_t)client_addr.sin_addr.s_addr)>>24)&0xff) != 0){
        	sprintf(ev->client_ip, "%s", inet_ntoa(client_addr.sin_addr));  //获取ip,用户管理
    	}
    }

    //设置客户端socket为非阻塞模式，如果是开启了web访问，不要设置
    /*if (SetNonblock(client_fd) == -1) {
    	printf("failed to set client socket non-blocking\n");
        close(client_sock);
        return;
    }*/

    //配置事件，libevent将在客户端socket可读时调用on_read函数
    event_set(ev->read_ev, client_sock, EV_READ|EV_PERSIST, on_read, ev);
    //设置事件到base
    if(event_base_set(base, ev->read_ev)<0){
	    if(ev->read_ev!=NULL)free(ev->read_ev);
	    if(ev!=NULL)free(ev);
	    close(client_sock);
    	printf("MASTER: set read ev err!\n");
    	return;
    }
    //激活事件
    if(event_add(ev->read_ev, NULL)<0){
	    if(ev->read_ev!=NULL)free(ev->read_ev);
	    if(ev!=NULL)free(ev);
	    close(client_sock);
    	printf("MASTER: add read ev err!\n");
    	return;
    }
    //htonl,htons,inet_addr,与之相对应的函数是ntohl,ntohs,inet_ntoa
    //printf("MASTER: Port %d accepted connection from %s:%d\n", ev->port, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    usleep(2);
}

int server_init(void)
{
    struct sockaddr_in listen_addr[3];
    uint16_t svrPort[3];

    svrPort[0] = System_Info.Modbus_Port;   //默认502
    svrPort[1] = System_Info.IEC104_Port;   //默认2404
    svrPort[2] = Http_Server_Port;			//默认80

    //初始化libevent
    base = event_base_new();
    if(base == NULL) return -1;

    for(int i = 0; i < 3; i++)
    {
		//流套接字类型为SOCK_STREAM、数据报套接字类型为SOCK_DGRAM、原始套接字SOCK_RAW
		//第三个参数指定应用程序所使用的通信协议。
    	listen_sock[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (listen_sock[i] == -1) {
			//printf("MASTER: create server socket err\n");
			return -1;
		}

		memset(&listen_addr[i], 0, sizeof(listen_addr[i]));
		listen_addr[i].sin_family = AF_INET;
		listen_addr[i].sin_port = htons(svrPort[i]);

		char webIP[64];
		GetLocalIp("eth0", webIP);  //对外
	    listen_addr[i].sin_addr.s_addr = inet_addr(webIP);//htonl(INADDR_ANY);//inet_addr("192.168.1.200");

		//bind函数用于将套接字绑定到一个已知的地址上
		if (bind(listen_sock[i], (struct sockaddr*)&listen_addr[i], sizeof(listen_addr[i])) < 0) {
			//printf("MASTER: bind server socket err\n");
			close(listen_sock[i]);
			return -1;
		}

		printf("MASTER: bind IP (%s) Port (%d) \n", webIP, svrPort[i]);

		//执行listen 之后套接字进入被动模式
		//MaxConnections 连接请求队列的最大长度,队列满了以后，将拒绝新的连接请求
		if (listen(listen_sock[i], SOMAXCONN) == -1) {
			//printf("MASTER: listen server socket err\n");
			close(listen_sock[i]);
			return -1;
		}

		//设置socket为非阻塞模式, 这种情况下只能连接5个客户端
		/*if (SetNonblock(listen_fd[i]) == -1) {
			printf("failed to set server socket to non-blocking\n");
			server_close();
			return -1;
		}*/

		//设置fd可重用
		int bReuseaddr=1;
		if(setsockopt(listen_sock[i],SOL_SOCKET,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(bReuseaddr)) != 0)
		{
			//printf("MASTER: setsockopt error in reuseaddr[%d]\n", listen_sock[i]);
			close(listen_sock[i]);
			return -1;
		}

		ev_accept[i] = (struct event*)malloc(sizeof(struct event));
		if(ev_accept[i] == NULL) return -1;
		//配置事件
		event_set(ev_accept[i], listen_sock[i], EV_READ | EV_PERSIST, on_accept, NULL);
		//设置事件
		if(event_base_set(base, ev_accept[i])<0){
			free(ev_accept[i]);
			close(listen_sock[i]);
	    	//printf("MASTER: set accept ev err!\n");
	    	return -1;
	    }
		//添加事件
		if (event_add(ev_accept[i], NULL)<0) {
			free(ev_accept[i]);
			close(listen_sock[i]);
			//printf("MASTER: soket%d add accept ev err\n", i);
			return -1;
		}
    }

	printf("MASTER: start listening!\n");
	event_base_dispatch(base);  //开始libevent事件循环, 不要在线程中直接调用
	printf("MASTER: quit!\n");
    return 0;
}
